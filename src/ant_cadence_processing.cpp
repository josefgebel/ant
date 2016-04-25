// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// CADENCE BASE ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

#include "ant_constants.h"
#include "am_split_string.h"
#include "ant_cadence_processing.h"


// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antCadenceProcessing::antCadenceProcessing
(
    void
)
{
    reset();
}

bool antCadenceProcessing::isCadenceSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_CAD_DEVICE_HEAD        ) ||
                  deviceID.startsWith( C_SPCAD_DEVICE_HEAD      ) ||
                  deviceID.startsWith( C_POWER_ONLY_DEVICE_HEAD ) ||
                  deviceID.startsWith( C_WT_POWER_DEVICE_HEAD   ) ||
                  deviceID.startsWith( C_CT_POWER_DEVICE_HEAD   ) ||
                  deviceID.startsWith( C_CTF_POWER_DEVICE_HEAD  );
    return result;
}

bool antCadenceProcessing::appendCadenceSensor
(
    const amString &deviceID
)
{
    bool result = isCadenceSensor( deviceID );
    return result;
}

void antCadenceProcessing::reset
(
    void
)
{
    cadenceTable.clear();
    cadenceSensorTable.clear();
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// CADENCE
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//

void antCadenceProcessing::setCadence
(
    const amString &sensorID,
    unsigned int       value
)
{
    if ( cadenceTable.count( sensorID ) > 0 )
    {
        cadenceTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
    }
    cadenceTable[ sensorID ] = value;
}

unsigned int antCadenceProcessing::getCadence
(
    const amString &sensorID
)
{
    unsigned int cadence = 0;
    if ( cadenceTable.count( sensorID ) > 0 )
    {
        cadence = cadenceTable[ sensorID ];
    }
    return cadence;
}

unsigned int antCadenceProcessing::computeCadence
(
    unsigned int previousCadence,
    unsigned int deltaRevolutionCount,
    unsigned int deltaEventTime
)
{
    unsigned int cadence = previousCadence;
    if ( deltaEventTime > 0 )
    {
        double cadenceAsDouble  = ( double ) deltaRevolutionCount;
        cadenceAsDouble        *= 61440.0;      // 1024 * 60  (unit is revolutions per minute, 1 minute = 60 seconds)
        cadenceAsDouble        /= ( double ) deltaEventTime;
        cadence                 = ( unsigned int ) round( cadenceAsDouble );
    }
    return cadence;
}

// ----------------------------------------------------------------------------
// Read a line from the deviceIDs file
// If the line contains a cadence device definition
//     Return true.
// Else:
//     Return false.
// ----------------------------------------------------------------------------
bool antCadenceProcessing::evaluateDeviceLine
(
    const amSplitString &words
)
{
    bool         result  = false;
    unsigned int nbWords = words.size();

    if ( nbWords > 2 )
    {
        amString deviceType = words[ 0 ];
        amString deviceName = words[ 1 ];
        if ( ( deviceType == C_CADENCE_DEVICE_ID ) && isCadenceSensor( deviceName ) )
        {
            result = appendCadenceSensor( deviceName );
        }
    }
    return result;
}

