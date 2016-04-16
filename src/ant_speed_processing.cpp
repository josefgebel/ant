// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// SPEED BASE ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <iomanip>

#include "ant_constants.h"
#include "b2t_utils.h"
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
    const std::string &sensorID
)
{
    double nbMagnets = ( nbMagnetsTable.count( sensorID ) == 0 ) ? nbMagnetsDefault : nbMagnetsTable[ sensorID ];
    return nbMagnets;
}

void antSpeedProcessing::setNbMagnets
(
    const std::string &sensorID,
    double             value
)
{
    if ( nbMagnetsTable.count( sensorID ) == 0 )
    {
        nbMagnetsTable.insert( std::pair<std::string, double>( sensorID, nbMagnetsDefault ) );
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
    const std::string &sensorID
)
{
    double result = ( startsWith( sensorID, C_SPEED_OBSOLETE_HEAD ) || startsWith( sensorID, C_SPEED_DEVICE_HEAD    ) ||
                      startsWith( sensorID, C_SPCAD_DEVICE_HEAD   ) || startsWith( sensorID, C_WT_POWER_DEVICE_HEAD ) );
    return result;
}

bool antSpeedProcessing::isMakeshiftSpeedSensor
(
    const std::string &sensorID
)
{
    double result = ( startsWith( sensorID, C_CAD_DEVICE_HEAD      ) || startsWith( sensorID, C_POWER_ONLY_DEVICE_HEAD ) ||
                      startsWith( sensorID, C_CT_POWER_DEVICE_HEAD ) || startsWith( sensorID, C_CTF_POWER_DEVICE_HEAD  ) );
    return result;
}

bool antSpeedProcessing::isSpeedSensor
(
    const std::string &sensorID
)
{
    double result = ( isPureSpeedSensor( sensorID ) || isMakeshiftSpeedSensor( sensorID ) );
    return result;
}

double antSpeedProcessing::getWheelCircumference
(
    const std::string &sensorID
)
{
    double wheelCircumference = ( wheelCircumferenceTable.count( sensorID ) == 0 ) ? wheelCircumferenceDefault : wheelCircumferenceTable[ sensorID ];
    return wheelCircumference;
}

void antSpeedProcessing::setWheelCircumference
(
    const std::string &sensorID,
    double             value
)
{
    if ( wheelCircumferenceTable.count( sensorID ) == 0 )
    {
        wheelCircumferenceTable.insert( std::pair<std::string, double>( sensorID, wheelCircumferenceDefault ) );
    }
    wheelCircumferenceTable[ sensorID ] = value;
}

double antSpeedProcessing::getSpeed
(
    const std::string &sensorID
)
{
    if ( speedTable.count( sensorID ) == 0 )
    {
        speedTable.insert( std::pair<std::string, double>( sensorID, 0 ) );
    }
    double speed = speedTable[ sensorID ];
    return speed;
}

void antSpeedProcessing::setSpeed
(
    const std::string &sensorID,
    double             value
)
{
    if ( speedTable.count( sensorID ) == 0 )
    {
        speedTable.insert( std::pair<std::string, double>( sensorID, 0 ) );
    }
    speedTable[ sensorID ] = value;
}


bool antSpeedProcessing::appendSpeedSensor
(
    const std::string &sensorID,
    double             wheelCircumference,
    double             nbMagnets
)
{
    bool result = isSpeedSensor( sensorID );
    if ( result )
    {
        if ( wheelCircumferenceTable.count( sensorID ) == 0 )
        {
            wheelCircumferenceTable.insert( std::pair<std::string, double>( sensorID, wheelCircumference ) );
        }
        if ( nbMagnetsTable.count( sensorID ) == 0 )
        {
            nbMagnetsTable.insert         ( std::pair<std::string, double>( sensorID, nbMagnets ) );
        }
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
    speedTable.clear();
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

// ----------------------------------------------------------------------------
// Read a line from the deviceIDs file
// If the line contains a speed device definition of a speed sensor (not a
//     makeshift cadence speed sensor), such as SPCAD, SPB7 or PWRB11
//     (Wheel-Torque Power Meter) read the device ID, the wheel circumference
//     and number of magnets, and store the values in the appropriate tables.
//     Return true.
// Else:
//     Return false.
// ----------------------------------------------------------------------------
bool antSpeedProcessing::evaluateDeviceLine
(
    const amSplitString &words
)
{
    bool         result  = false;
    unsigned int nbWords = words.size();

    if ( nbWords > 1 ) 
    {   
        std::string deviceType = words[ 0 ];
        std::string deviceName = words[ 1 ];
        if ( ( deviceType == C_SPEED_DEVICE_ID ) && isPureSpeedSensor( deviceName ) )
        {
            double dArg1 = wheelCircumferenceDefault;
            double dArg2 = nbMagnetsDefault;
            if ( nbWords > 2 )
            {
                dArg1 = strToDouble( words[ 2 ] );
                if ( nbWords > 3 )
                {
                    dArg2 = strToDouble( words[ 3 ] );
                }
            }
            result = appendSpeedSensor( deviceName, dArg1, dArg2 );
        }
    }
    return result;
}

