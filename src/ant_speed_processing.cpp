// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// SPEED BASE ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

#include "ant_constants.h"
#include "am_split_string.h"
#include "ant_speed_processing.h"

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antSpeedProcessing::antSpeedProcessing
(
    void
)
{
    nbMagnetsDefault          = C_NB_MAGNETS_DEFAULT;
    wheelCircumferenceDefault = C_WHEEL_CIRCUMFERENCE_DEFAULT;
    reset();
}

double antSpeedProcessing::getNbMagnets
(
    const amString &sensorID
)
{
    double nbMagnets = ( nbMagnetsTable.count( sensorID ) == 0 ) ? nbMagnetsDefault : nbMagnetsTable[ sensorID ];
    return nbMagnets;
}

void antSpeedProcessing::setNbMagnets
(
    const amString &sensorID,
    double          value
)
{
    if ( nbMagnetsTable.count( sensorID ) == 0 )
    {
        nbMagnetsTable.insert( std::pair<amString, double>( sensorID, nbMagnetsDefault ) );
    }
    nbMagnetsTable[ sensorID ] = value;
}

// ---------------------------------------------------------------------------------
// Return true if the sensor is a 'pure' (i.e. not a makeshift) speed sensors
// Currently supported pure speed sensors are SPB7, SPCAD790, PWRB11
// (currently supported makeshift speed sensors are CAD7A, PWRB10, PWRB12, PWRB20).
// ---------------------------------------------------------------------------------
bool antSpeedProcessing::isPureSpeedSensor
(
    const amString &sensorID
)
{
    double result = ( sensorID.startsWith( C_SPEED_OBSOLETE_HEAD ) || sensorID.startsWith( C_SPEED_DEVICE_HEAD    ) ||
                      sensorID.startsWith( C_SPCAD_DEVICE_HEAD   ) || sensorID.startsWith( C_WT_POWER_DEVICE_HEAD ) );
    return result;
}

bool antSpeedProcessing::isMakeshiftSpeedSensor
(
    const amString &sensorID
)
{
    double result = ( sensorID.startsWith( C_CAD_DEVICE_HEAD      ) || sensorID.startsWith( C_POWER_ONLY_DEVICE_HEAD ) ||
                      sensorID.startsWith( C_CT_POWER_DEVICE_HEAD ) || sensorID.startsWith( C_CTF_POWER_DEVICE_HEAD  ) );
    return result;
}

bool antSpeedProcessing::isSpeedSensor
(
    const amString &sensorID
)
{
    double result = ( isPureSpeedSensor( sensorID ) || isMakeshiftSpeedSensor( sensorID ) );
    return result;
}

double antSpeedProcessing::getWheelCircumference
(
    const amString &sensorID
)
{
    double wheelCircumference = ( wheelCircumferenceTable.count( sensorID ) == 0 ) ? wheelCircumferenceDefault : wheelCircumferenceTable[ sensorID ];
    return wheelCircumference;
}

void antSpeedProcessing::setWheelCircumference
(
    const amString &sensorID,
    double          value
)
{
    if ( wheelCircumferenceTable.count( sensorID ) == 0 )
    {
        wheelCircumferenceTable.insert( std::pair<amString, double>( sensorID, wheelCircumferenceDefault ) );
    }
    wheelCircumferenceTable[ sensorID ] = value;
}

double antSpeedProcessing::getSpeed
(
    const amString &sensorID
)
{
    if ( speedTable.count( sensorID ) == 0 )
    {
        speedTable.insert( std::pair<amString, double>( sensorID, 0 ) );
    }
    double speed = speedTable[ sensorID ];
    return speed;
}

void antSpeedProcessing::setSpeed
(
    const amString &sensorID,
    double          value
)
{
    if ( speedTable.count( sensorID ) == 0 )
    {
        speedTable.insert( std::pair<amString, double>( sensorID, 0 ) );
    }
    speedTable[ sensorID ] = value;
}


bool antSpeedProcessing::appendSpeedSensor
(
    const amString &sensorID,
    double          wheelCircumference,
    double          nbMagnets
)
{
    bool result = isSpeedSensor( sensorID );
    if ( result )
    {
        if ( wheelCircumferenceTable.count( sensorID ) == 0 )
        {
            wheelCircumferenceTable.insert( std::pair<amString, double>( sensorID, wheelCircumference ) );
        }
        if ( nbMagnetsTable.count( sensorID ) == 0 )
        {
            nbMagnetsTable.insert         ( std::pair<amString, double>( sensorID, nbMagnets ) );
        }
        setUseAsSpeedSensor( sensorID, true );
    }
    return result;
}

void antSpeedProcessing::reset
(
    void
)
{
    resetNbMagnetsDefault();
    resetWheelCircumferenceDefault();
    nbMagnetsTable.clear();
    wheelCircumferenceTable.clear();
    usedAsSpeedSensorTable.clear();
    speedTable.clear();
    setMaxZeroTime( C_MAX_ZERO_TIME );
}

void antSpeedProcessing::setUseAsSpeedSensor
(
    const amString &sensorID,
    bool            value
)
{
    if ( usedAsSpeedSensorTable.count( sensorID ) == 0 )
    {
        usedAsSpeedSensorTable.insert( std::pair<amString, double>( sensorID, value ) );
    }
    usedAsSpeedSensorTable[ sensorID ] = value;
}

bool antSpeedProcessing::isUsedAsSpeedSensor
(
    const amString &sensorID
)
{
    bool result = false;
    if ( usedAsSpeedSensorTable.count( sensorID ) == 0 )
    {
        usedAsSpeedSensorTable.insert( std::pair<amString, double>( sensorID, result ) );
    }
    result = usedAsSpeedSensorTable[ sensorID ];
    return result;
}

double antSpeedProcessing::computeSpeed
(
    double        previousSpeed,
    unsigned int  deltaWheelRevolutionCount,
    unsigned int  deltaBikeSpeedEventTime,
    double        wheelCircumference,
    unsigned int  numberOfMagnets,
    unsigned int &zeroTimeCount,
    unsigned int  maxZeroTimeCount
)
{
    double speed = previousSpeed;
    if ( deltaBikeSpeedEventTime == 0 )
    {
        zeroTimeCount++;
        if ( zeroTimeCount > maxZeroTimeCount )
        {
            speed = 0;
        }
    }
    else if ( numberOfMagnets > 0 )
    {
        zeroTimeCount = 0;
        speed  = ( double)  deltaWheelRevolutionCount;
        speed /= ( double ) deltaBikeSpeedEventTime;
        speed /= ( double ) numberOfMagnets;
        speed *= 1024.0;
        speed *= wheelCircumference;
    }
    return speed;
}

int antSpeedProcessing::readDeviceFileLine1
(
    const char *line,
    amString   &errorMessage
)
{
    amSplitString words;
    unsigned int  nbWords   = words.split( line, C_COMMENT_SYMBOL_AS_STRING );
    int           errorCode = 0;

    if ( nbWords > 1 )
    {
        amString deviceType = words[ 0 ];
        if ( deviceType == C_SPEED_DEVICE_ID )
        {
            amString deviceName = words[ 1 ];
            if ( isPureSpeedSensor( deviceName ) )
            {
                double curCircumference = wheelCircumferenceDefault;
                double curNbMagnets     = nbMagnetsDefault;
                if ( nbWords > 2 )
                {
                    curCircumference = words[ 2 ].toDouble();
                    if ( ( curCircumference < C_MIN_CIRCUMFERENCE ) || ( curCircumference > C_MAX_CIRCUMFERENCE ) )
                    {
                        errorMessage     += "WARNING: Value for wheel circumference (";
                        errorMessage     += amString( curCircumference, 4 );
                        errorMessage     += " m) is outside of recommended range [";
                        errorMessage     += amString( C_MIN_CIRCUMFERENCE, 4 );
                        errorMessage     += ", ";
                        errorMessage     += amString( C_MAX_CIRCUMFERENCE, 4 );
                        errorMessage     += "].\n";
                        curCircumference  = wheelCircumferenceDefault;
                        errorCode         = E_BAD_PARAMETER_VALUE;
                        errorMessage     += "             The value has been set to its default (";
                        errorMessage     += amString( curCircumference, 4 );
                        errorMessage     += " m).\n";
                    }
                    if ( nbWords > 3 )
                    {
                        curNbMagnets = words[ 3 ].toDouble();
                        if ( ( curNbMagnets < C_MIN_NB_MAGNETS ) || ( curNbMagnets > C_MAX_NB_MAGNETS ) )
                        {
                            errorMessage += ( errorCode ? "         " : "WARNING: " );
                            errorMessage += "Value for number of magnets (";
                            errorMessage += amString( curNbMagnets, 0 );
                            errorMessage += ") is outside of recommended range [";
                            errorMessage += amString( C_MIN_NB_MAGNETS );
                            errorMessage += ", ";
                            errorMessage += amString( C_MAX_NB_MAGNETS );
                            errorMessage += "].\n";
                            curNbMagnets  = nbMagnetsDefault;
                            errorCode     = E_BAD_PARAMETER_VALUE;
                            errorMessage += "             The value has been set to its default (";
                            errorMessage += amString( curNbMagnets, 0 );
                            errorMessage += ").\n";
                        }
                    }
                    if ( errorCode )
                    {
                        errorMessage += "         Line in file: \"";
                        errorMessage += line;
                        errorMessage += "\".\n";
                    }
                }
                appendSpeedSensor( deviceName, curCircumference, curNbMagnets );
            }
        }
    }
    return errorCode;
}

