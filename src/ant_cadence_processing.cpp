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

int antCadenceProcessing::readDeviceFileLine1
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
        if ( deviceType == C_CADENCE_DEVICE_ID )
        {
            amString deviceName = words[ 1 ];
            if ( isCadenceSensor( deviceName ) )
            {
                appendCadenceSensor( deviceName );
            }
        }
    }
    return errorCode;
}

