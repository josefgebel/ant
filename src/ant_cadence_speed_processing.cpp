// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// SPEED THROUGH CADENCE ANT PROCESSOR class methods definition
//
// This class processes cadence sensors (CAD7A, PWRB10, PWRB12, PWRB20) which are used as makeshift speed sensors
// by exploiting cadence and gear ratio to compute the number of wheel revolutions.
// The cadence data supply the time values.
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

#include "am_split_string.h"
#include "ant_cadence_speed_processing.h"

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antCadenceSpeedProcessing::antCadenceSpeedProcessing
(
    void
) : antSpeedProcessing(),
    antCadenceProcessing()
{
    nbMagnetsDefault = C_GEAR_RATIO_DEFAULT;
    reset();
}

bool antCadenceSpeedProcessing::appendCadenceSpeedSensor
(
    const amString &sensorID,
    double          wheelCircumference,
    double          gearRatio
)
{
    bool result = sensorID.startsWith( C_CAD_DEVICE_HEAD        ) ||
                  sensorID.startsWith( C_POWER_ONLY_DEVICE_HEAD ) ||
                  sensorID.startsWith( C_WT_POWER_DEVICE_HEAD   ) ||
                  sensorID.startsWith( C_CT_POWER_DEVICE_HEAD   ) ||
                  sensorID.startsWith( C_CTF_POWER_DEVICE_HEAD  );
    if ( result )
    {
        antSpeedProcessing::appendSpeedSensor( sensorID, wheelCircumference, gearRatio );
        setUseAsSpeedSensor( sensorID, true );
    }
    return result;
}

bool antCadenceSpeedProcessing::isMakeshiftSpeedSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_CAD_DEVICE_HEAD        ) ||
                  deviceID.startsWith( C_POWER_ONLY_DEVICE_HEAD ) ||
                  deviceID.startsWith( C_CT_POWER_DEVICE_HEAD   ) ||
                  deviceID.startsWith( C_CTF_POWER_DEVICE_HEAD  );
    return result;
}

void antCadenceSpeedProcessing::reset
(
    void
)
{
    antCadenceProcessing::reset();
    antSpeedProcessing::reset();
    resetGearRatioDefault();
}

double antCadenceSpeedProcessing::computeSpeed
(
    unsigned int cadence,
    double       wheelCircumference,
    double       gearRatio
)
{
    double speed = 0;
    if ( ( wheelCircumference > 0 ) && ( gearRatio > 0 ) )
    {
        speed = wheelCircumference * cadence * gearRatio / 60.0;
    }
    return speed;
}

int antCadenceSpeedProcessing::readDeviceFileLine1
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
        if ( ( deviceType == C_SPEED_DEVICE_ID ) )
        {
            amString deviceName = words[ 1 ];
            if ( isMakeshiftSpeedSensor( deviceName ) )
            {
                double curCircumference = wheelCircumferenceDefault;
                double curGearRatio     = gearRatioDefault;
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
                        curGearRatio = words[ 3 ].toDouble();
                        if ( ( curGearRatio < C_MIN_GEAR_RATIO ) || ( curGearRatio > C_MAX_GEAR_RATIO ) )
                        {
                            errorMessage += ( errorCode ? "         " : "WARNING: " );
                            errorMessage += "Value for gear ratio (";
                            errorMessage += amString( curGearRatio, 8 );
                            errorMessage += ") is outside of recommended range [";
                            errorMessage += amString( C_MIN_GEAR_RATIO, 8 );
                            errorMessage += ", ";
                            errorMessage += amString( C_MAX_GEAR_RATIO, 8 );
                            errorMessage += "].\n";
                            curGearRatio  = gearRatioDefault;
                            errorCode     = E_BAD_PARAMETER_VALUE;
                            errorMessage += "             The value has been set to its default (";
                            errorMessage += amString( curGearRatio, 4 );
                            errorMessage += ").\n";
                        }
                    }
                }
                appendCadenceSpeedSensor( deviceName, curCircumference, curGearRatio );
            }
        }
        if ( errorCode )
        {
            errorMessage += "         Line in file: \"";
            errorMessage += line;
            errorMessage += "\".\n";
        }
    }
    return errorCode;
}

