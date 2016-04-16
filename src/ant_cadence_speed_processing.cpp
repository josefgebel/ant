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

#include <iostream>

#include "b2t_utils.h"
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
    const std::string &sensorID,
    double             wheelCircumference,
    double             gearRatio
)
{
    bool result = startsWith( sensorID, C_CAD_DEVICE_HEAD        ) ||
                  startsWith( sensorID, C_POWER_ONLY_DEVICE_HEAD ) ||
                  startsWith( sensorID, C_WT_POWER_DEVICE_HEAD   ) ||
                  startsWith( sensorID, C_CT_POWER_DEVICE_HEAD   ) ||
                  startsWith( sensorID, C_CTF_POWER_DEVICE_HEAD  );
    if ( result )
    {
        antSpeedProcessing::appendSpeedSensor( sensorID, wheelCircumference, gearRatio );
        setUseAsSpeedSensor( sensorID, true );
    }
    return result;
}

void antCadenceSpeedProcessing::setUseAsSpeedSensor
(
    const std::string &sensorID,
    bool               value
)
{
    if ( speedCadenceSensorTable.count( sensorID ) == 0 ) 
    {        
        speedCadenceSensorTable.insert( std::pair<std::string, double>( sensorID, value ) );  
    }    
    speedCadenceSensorTable[ sensorID ] = value;
}

bool antCadenceSpeedProcessing::isUsedAsSpeedSensor
(
    const std::string &sensorID
)
{
    bool result = false;
    if ( speedCadenceSensorTable.count( sensorID ) == 0 ) 
    {        
        speedCadenceSensorTable.insert( std::pair<std::string, double>( sensorID, result ) );  
    }        
    result = speedCadenceSensorTable[ sensorID ];
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

// ----------------------------------------------------------------------------
// Read a line from the deviceIDs file
// If the line contains a speed device definition of a cadence sensor (which
//     uses cadence to compute speed - Note: this does not include SPCAD790)
//     read the device ID, the wheel circumference and gear ratio, and store
//     the values in the appropriate tables.
//     Also, set "usedAsSpeedSensor" true.
//     Return true.
// Else:
//     Return false.
// ----------------------------------------------------------------------------
bool antCadenceSpeedProcessing::evaluateDeviceLine
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
        if ( ( deviceType == C_SPEED_DEVICE_ID ) && isCadenceSensor( deviceName ) )
        {
            double dArg1 = wheelCircumferenceDefault;
            double dArg2 = gearRatioDefault;
            if ( nbWords > 2 )
            {
                dArg1 = strToDouble( words[ 2 ] );
                if ( nbWords > 3 )
                {
                    dArg2 = strToDouble( words[ 3 ] );
                }
            }
            result = appendCadenceSpeedSensor( deviceName, dArg1, dArg2 );
        }
    }
    return result;
}

