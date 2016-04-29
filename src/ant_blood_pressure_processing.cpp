// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_blood_pressure_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// BLOOD PRESSURE ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antBloodPressureProcessing::antBloodPressureProcessing
(
    void
) : antProcessing()
{
    setCurrentDeviceType( "BLOOD" );
    reset();
}

void antBloodPressureProcessing::reset
(
    void
)
{
    antProcessing::reset();
}

bool antBloodPressureProcessing::isBloodPressureSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_BLDPR_DEVICE_HEAD );
    return result;
}

bool antBloodPressureProcessing::appendBloodPressureSensor
(
    const amString &sensorID
)
{
    bool result = isBloodPressureSensor( sensorID );

    if ( result )
    {
        if ( !isRegisteredDevice( sensorID ) )
        {
            registerDevice( sensorID );
        }
    }

    return result;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// BLOOD PRESSURE
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antBloodPressureProcessing::processBloodPressureSensor
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result       = OTHER_DEVICE;
    amString  sensorID        = amString( C_BLDPR_DEVICE_HEAD ) + deviceIDNo;
    unsigned int dataPage     = 0;
    bool         commonPage   = false;
    bool         outputPageNo = true;

    if ( isRegisteredDevice( sensorID ) )
    {
        dataPage = byte2UInt( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }
        if ( dataPage == C_NON_EXISTENT_DATA_PAGE )
        {
            ;
        }
        else
        {
            commonPage = true;
            result     = BLOOD_PRESSURE_SENSOR;
        }
    }

    if ( result == BLOOD_PRESSURE_SENSOR )
    {
        createOutputHeader( sensorID, timeStampBuffer );
        if ( commonPage )
        {
            commonPage = processCommonPages( sensorID, payLoad, outputPageNo );
            if ( !commonPage )
            {
                result = OTHER_DEVICE;
            }
        }
        else
        {
            createBLDPRResultString( dataPage );
        }
        appendOutputFooter( getVersion() );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = deviceIDNo.toInt();
            createUnknownDeviceTypeString( C_BLDPR_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

amDeviceType antBloodPressureProcessing::processBloodPressureSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    amString      sensorID;
    amString      semiCookedString;
    amString      timeStampBuffer;
    amString      curVersion   = getVersion();
    amSplitString words;
    unsigned int  nbWords      = words.split( inputBuffer );
    unsigned int  startCounter = 0;
    unsigned int  counter      = 0;
    unsigned int  dataPage     = 0;
    bool          commonPage   = false;
    bool          outputPageNo = true;

    if ( nbWords > 7 )
    {
        sensorID         = words[ counter++ ];                     //  0
        timeStampBuffer  = words[ counter++ ];                     //  1
        semiCookedString = words[ counter++ ];                     //  2
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }
        if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isBloodPressureSensor( sensorID ) )
        {
            startCounter = counter;
            dataPage     = words[ counter++ ].toUInt();            //  3
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Data Page", dataPage );
            }
            if ( words[ counter ] == C_UNSUPPORTED_DATA_PAGE )
            {
                result = UNKNOWN_DEVICE;
            }
            else
            {
                counter = startCounter;
                if ( dataPage == C_NON_EXISTENT_DATA_PAGE )
                {
                    ;
                }
                else
                {
                    commonPage = true;
                    result     = BLOOD_PRESSURE_SENSOR;
                }
            }
        }
    }

    if ( result == BLOOD_PRESSURE_SENSOR )
    {
        if ( nbWords > counter )
        {
            curVersion = words.back();
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        createOutputHeader( sensorID, timeStampBuffer );
        if ( commonPage )
        {
            commonPage = processCommonPagesSemiCooked( words, startCounter, outputPageNo );
            if ( !commonPage )
            {
                result = OTHER_DEVICE;
            }
        }
        else
        {
            createBLDPRResultString( dataPage );
        }
        appendOutputFooter( curVersion );
    }
    else if ( result == UNKNOWN_DEVICE )
    {
        result = processUnsupportedDataPage( words );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            setOutBuffer( inputBuffer );
        }
    }

    return result;
}

//---------------------------------------------------------------------------------------------------
//
// processSensor
//
// Depending on the device type call a subroutine to process the payload data and put
// the result string into the resultBuffer.
//
// Parameters:
//    int             deviceType        IN   Device type
//    const amString &deviceID          IN   Device ID (number).
//    const amString &timeStampBuffer   IN   Time stamp.
//    BYTE            payLoad[]         IN   Array of bytes with the data to be converted.
//
// Return amDeviceType SPEED_SENSOR, CADENCE_SENSOR, POWER_METER, AERO_SENSOR, or HEART_RATE_METER
//             if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
//---------------------------------------------------------------------------------------------------
amDeviceType antBloodPressureProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_BLDPR_TYPE )
    {
        result = processBloodPressureSensor( deviceIDNo, timeStampBuffer, payLoad );
    }
    else
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = deviceIDNo.toInt();
            createUnknownDeviceTypeString( deviceType, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

amDeviceType antBloodPressureProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( !inputBuffer.empty() )
    {
        if ( isBloodPressureSensor( inputBuffer ) )
        {
            result = processBloodPressureSensorSemiCooked( inputBuffer );
        }
        else
        {
            resetOutBuffer();
            if ( outputUnknown )
            {
                setOutBuffer( inputBuffer );
            }
        }
    }

    return result;
}

void antBloodPressureProcessing::readDeviceFileLine
(
    const char *line
)
{
    amSplitString words;
    unsigned int  nbWords = words.split( line, C_COMMENT_SYMBOL_AS_STRING );

    if ( nbWords > 1 )
    {
        amString deviceType = words[ 0 ];

        if ( deviceType == C_INCLUDE_FILE )
        {
            amString      curFileName = words.concatenate( 1 );
            std::ifstream devicesIncludeFileStream( curFileName.c_str() );
            if ( devicesIncludeFileStream.fail() )
            {
                appendErrorMessage( "ERROR while opening devices ID include file \"" );
                appendErrorMessage( curFileName );
                appendErrorMessage( "\".\n" );
                errorCode = E_READ_FILE_NOT_OPEN;
            }
            else
            {
                readDeviceFileStream( devicesIncludeFileStream );
                devicesIncludeFileStream.close();
            }
        }
        else if ( deviceType == C_BLOOD_PRESSURE_DEVICE_ID )
        {
            amString deviceName = words[ 1 ];
            if ( isBloodPressureSensor( deviceName ) )
            {
                appendBloodPressureSensor( deviceName );
            }
        }
    }
}

void antBloodPressureProcessing::createBLDPRResultString
(
    unsigned int dataPage
)
{
    if ( outputAsJSON )
    {
        appendJSONItem( "data page", dataPage );
    }
    else
    {
        appendOutput( dataPage );
    }
}

