// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_hrm_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// HRM ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antHRMProcessing::antHRMProcessing
(
    void
) : antProcessing()
{
    reset();
    setCurrentDeviceType( "HRM" );
}

void antHRMProcessing::reset
(
    void
)
{
    antProcessing::reset();
    previousHeartRateTable.clear();
    eventTimeTable.clear();
    heartBeatTimeTable.clear();
    heartRateSensorTable.clear();
}

bool antHRMProcessing::isHeartRateSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_HEART_RATE_DEVICE_ID );
    return result;
}

bool antHRMProcessing::appendHRMSensor
(
    const amString &sensorID
)
{
    bool result = isHeartRateSensor( sensorID );
    if ( result )
    {
        if ( !isRegisteredSensor( sensorID ) )
        {
            heartRateSensorTable.insert( std::pair<amString, bool>( sensorID, true ) );
        }
    }
    return result;
}

bool antHRMProcessing::isRegisteredSensor
(
    const amString &deviceID
)
{
    bool result = ( heartRateSensorTable.count( deviceID ) > 0 );
    return result;
}


//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// HRM
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antHRMProcessing::processHRMSensor
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
    unsigned int dataPage                          = 0;
    unsigned int heartBeatEventTime                = 0;
    unsigned int heartRate                         = 0;
    unsigned int heartBeatCount                    = 0;
    unsigned int deltaHeartBeatEventTime           = 0;
    unsigned int deltaHeartBeatCount               = 0;
    unsigned int rollOver                          = 0;
    unsigned int additionalData1                   = 0;
    unsigned int additionalData2                   = 0;
    unsigned int additionalData3                   = 0;
    unsigned int totalHeartBeatCount               = 0;
    double       totalHeartBeatEventTime           = 0;
    double       additionalDoubleData1             = 0;
    bool         rollOverHappened                  = false;
    bool         commonPage                        = false;
    bool         outputPageNo                      = true;
    amDeviceType result                            = OTHER_DEVICE;
    amString     sensorID                          = amString( C_HRM_DEVICE_HEAD) + deviceIDNo;


    if ( isRegisteredDevice( sensorID ) )
    {
        if ( ( eventTimeTable.count    ( sensorID ) == 0 ) ||
             ( eventCountTable.count   ( sensorID ) == 0 ) ||
             ( operatingTimeTable.count( sensorID ) == 0 ) )
        {
            eventTimeTable.insert( std::pair<amString, double>( sensorID, 0.0 ) );
            eventCountTable.insert( std::pair<amString, double>( sensorID, 0.0 ) );
            operatingTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
        }

        dataPage = byte2UInt( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        heartRate = byte2UInt( payLoad[ 7 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Heart Rate", payLoad[ 7 ], heartRate );
        }

        heartBeatEventTime         = byte2UInt( payLoad[ 5 ], payLoad[ 4 ] );
        rollOver                   = 65536;  // 256^2
        deltaHeartBeatEventTime    = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, heartBeatEventTime );
        totalHeartBeatEventTime    = totalTimeTable[ sensorID ] + ( ( double ) deltaHeartBeatEventTime ) / 1024.0;
        totalTimeTable[ sensorID ] = totalHeartBeatEventTime;
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Heart Beat Even Time", payLoad[ 5 ], payLoad[ 4 ], heartBeatEventTime );

            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Heart Beat Even Time", deltaHeartBeatEventTime, auxBuffer );
        }

        heartBeatCount              = byte2UInt( payLoad[ 6 ] );
        rollOver                    = 256;  // 256 = 1 Byte
        deltaHeartBeatCount         = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, heartBeatCount );
        totalHeartBeatCount         = totalCountTable[ sensorID ] + deltaHeartBeatCount;
        totalCountTable[ sensorID ] = totalHeartBeatCount;
        if ( diagnostics )
        {
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Heart Beat Count", deltaHeartBeatCount, auxBuffer );
        }

        int dataPageMod128 = dataPage & 0x0F;
        switch ( dataPageMod128 )
        {
            case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                     result = HEART_RATE_METER;
                     break;

            case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                     result          = HEART_RATE_METER;
                     additionalData2 = byte2UInt( payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ] ); // Operating Time
                     rollOver        = 16777216;  // 256^3
                     additionalData1 = getDeltaInt( rollOverHappened, sensorID, rollOver, operatingTimeTable, additionalData2 );
                                       // deltaOperatingTime
                     if ( diagnostics )
                     {
                         double cumOperatingTimeH = ( double ) additionalData2 / 3600.0;
                         sprintf( auxBuffer, " (%2.2lfh)", cumOperatingTimeH );
                         appendDiagnosticsLine( "Cumulative Operating Time", payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ], additionalData2, auxBuffer );
                         *auxBuffer = 0;
                         if ( rollOverHappened )
                         {
                             sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                         }
                         appendDiagnosticsLine( "Delta Cumulative Operating Time", additionalData1, auxBuffer );
                     }
                     break;

            case  2: // - - Page 2: Manufacturer Information - - - - - - - - - - - -
                     result          = HEART_RATE_METER;
                     additionalData1 = byte2UInt( payLoad[ 1 ] );                // Manufacturer ID
                     additionalData2 = byte2UInt( payLoad[ 3 ], payLoad[ 2 ] );  // Serial Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Manufacturer ID", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Serial Number", payLoad[ 3 ], payLoad[ 2 ], additionalData2 );
                     }
                     break;

            case  3: // - - Page 3: Product Information  - - - - - - - - - - - - - -
                     result          = HEART_RATE_METER;
                     additionalData1 = byte2UInt( payLoad[ 1 ] );   // H/W Version
                     additionalData2 = byte2UInt( payLoad[ 2 ] );   // S/W Version
                     additionalData3 = byte2UInt( payLoad[ 3 ] );   // Model Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Hardware Version", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Software Version", payLoad[ 2 ], additionalData2 );
                         appendDiagnosticsLine( "Model Number", payLoad[ 3 ], additionalData3 );
                     }
                     break;

            case  4: // - - Page 4: Previous Heartbeat Time - - - - - - - - - - - - -
                     result                         = HEART_RATE_METER;
                     rollOver                       = 65536;  // 256^2
                     additionalData2                = byte2UInt( payLoad[ 1 ] );                 // Manufacturer Specific Data;
                     additionalData3                = byte2UInt( payLoad[ 3 ], payLoad[ 2 ] );   // Previous Heart Beat Event Time
                     additionalData1                = getDeltaInt( rollOverHappened, sensorID, rollOver, previousHeartRateTable, additionalData3 );
                     additionalDoubleData1          = heartBeatTimeTable[ sensorID ] + ( ( double ) additionalData1 ) / 1024.0;
                     additionalData3                = 0;
                     heartBeatTimeTable[ sensorID ] = additionalDoubleData1;
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Manufacturer Specific Info", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Previous Heart Beat Event Time", payLoad[ 3 ], payLoad[ 2 ], additionalData2 );
                         *auxBuffer = 0;
                         if ( rollOverHappened )
                         {
                             sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                         }
                         appendDiagnosticsLine( "Delta Previous Heart Beat Event Time", additionalData2, auxBuffer );
                     }
                     break;

            default: commonPage = true;
                     result     = HEART_RATE_METER;
                     break;
        }
    }

    if ( result == HEART_RATE_METER )
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
            createHRMResultString
            (
                dataPage,
                heartRate,
                deltaHeartBeatEventTime,
                deltaHeartBeatCount,
                totalHeartBeatEventTime,
                totalHeartBeatCount,
                additionalData1,
                additionalData2,
                additionalData3,
                additionalDoubleData1
            );
        }
        appendOutputFooter( getVersion() );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processHRMSensorSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
// Depending on the value of <data_page> & 127:
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antHRMProcessing::processHRMSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    amSplitString words;
    unsigned int  deltaHeartBeatEventTime = 0;
    unsigned int  deltaHeartBeatCount     = 0;
    unsigned int  heartRate               = 0;
    unsigned int  dataPage                = 0;
    unsigned int  counter                 = 0;
    unsigned int  startCounter            = 0;
    unsigned int  additionalData1         = 0;
    unsigned int  additionalData2         = 0;
    unsigned int  additionalData3         = 0;
    unsigned int  totalHeartBeatCount     = 0;
    unsigned int  nbWords                 = words.split( inputBuffer );
    double        totalHeartBeatEventTime = 0;
    double        additionalDoubleData1   = 0;
    amString      timeStampBuffer;
    amString      curVersion              = getVersion();
    amString      sensorID;
    amString      timeStampString;
    amString      semiCookedString;
    bool          commonPage              = false;
    bool          outputPageNo            = true;

    if ( nbWords > 6 )
    {
        sensorID         = words[ counter++ ];                        // 0
        timeStampBuffer  = words[ counter++ ];                        // 1
        semiCookedString = words[ counter++ ];                        // 2
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }
        if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isHeartRateSensor( sensorID ) )
        {
            startCounter            = counter;
            heartRate               = words[ counter++ ].toUInt();    // 3
            deltaHeartBeatEventTime = words[ counter++ ].toUInt();    // 4
            deltaHeartBeatCount     = words[ counter++ ].toUInt();    // 5
            dataPage                = words[ counter++ ].toUInt();    // 6
            if ( words[ counter ] == C_UNSUPPORTED_DATA_PAGE )
            {
                result = UNKNOWN_DEVICE;
            }
            else
            {
                totalHeartBeatEventTime     = totalTimeTable[ sensorID ] + ( ( double ) deltaHeartBeatEventTime ) / 1024.0;
                totalTimeTable[ sensorID ]  = totalHeartBeatEventTime;
                totalHeartBeatCount         = totalCountTable[ sensorID ] + deltaHeartBeatCount;
                totalCountTable[ sensorID ] = totalHeartBeatCount;
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Data Page", dataPage );
                    appendDiagnosticsLine( "Heart Rate", heartRate );
                    appendDiagnosticsLine( "Delta Heart Beat Event Time", deltaHeartBeatEventTime );
                    appendDiagnosticsLine( "Delta Heart Beat Count", deltaHeartBeatCount );
                }

                int dataPageMod128 = dataPage & 0x0F;
                switch ( dataPageMod128 )
                {
                    case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                             result = HEART_RATE_METER;
                             break;

                    case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                             if ( nbWords > 7 )
                             {
                                 result          = HEART_RATE_METER;
                                 additionalData1 = words[ counter++ ].toUInt();   // deltaOperatingTime
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Delta Cumulative Operating Time", additionalData1 );
                                 }
                             }
                             break;

                    case  2: // - - Page 2: Manufacturer Information - - - - - - - - - - - -
                             if ( nbWords > 8 )
                             {
                                 result          = HEART_RATE_METER;
                                 additionalData1 = words[ counter++ ].toUInt();   // manufacturerID
                                 additionalData2 = words[ counter++ ].toUInt();   // serialNumber
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Manufacturer ID", additionalData1 );
                                     appendDiagnosticsLine( "Serial Number", additionalData2 );
                                 }
                             }
                             break;

                    case  3: // - - Page 3: Product Information  - - - - - - - - - - - - - -
                             if ( nbWords > 9 )
                             {
                                 result          = HEART_RATE_METER;
                                 additionalData1 = words[ counter++ ].toUInt();   // hwVersion
                                 additionalData2 = words[ counter++ ].toUInt();   // swVersion
                                 additionalData3 = words[ counter++ ].toUInt();   // modelNumber
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Model Number", additionalData1 );
                                     appendDiagnosticsLine( "Software Version", additionalData2 );
                                     appendDiagnosticsLine( "Hardware Version", additionalData3 );
                                 }
                             }
                             break;

                    case  4: // - - Page 4: Previous Heartbeat Time - - - - - - - - - - - - -
                             if ( nbWords > 8 )
                             {
                                 result                         = HEART_RATE_METER;
                                 additionalData1                = words[ counter++ ].toUInt(); // deltaPrevHeartBeatEventTime;
                                 additionalData2                = words[ counter++ ].toUInt(); // manufacturerSpecificData;
                                 additionalDoubleData1          = heartBeatTimeTable[ sensorID ] + ( ( double ) additionalData1 ) / 1024.0;
                                 heartBeatTimeTable[ sensorID ] = additionalDoubleData1;
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Manufacturer Specific Info", additionalData1 );
                                     appendDiagnosticsLine( "Delta Previous Heart Beat Event Time", additionalData2 );
                                 }
                             }
                             break;

                   default: counter    = startCounter;
                            commonPage = true;
                            result     = HEART_RATE_METER;
                            break;
                }
            }
        }
    }

    if ( result == HEART_RATE_METER )
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
            createHRMResultString
            (
                dataPage,
                heartRate,
                deltaHeartBeatEventTime,
                deltaHeartBeatCount,
                totalHeartBeatEventTime,
                totalHeartBeatCount,
                additionalData1,
                additionalData2,
                additionalData3,
                additionalDoubleData1
            );
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
// the result string into the outBuffer.
//
// Parameters:
//    int             deviceType        IN   Device type (SPCAD, SPEED, CADENCE, HRM, AERO, POWER).
//    const amString &deviceID          IN   Device ID (number).
//    const amString &timeStampBuffer   IN   Time stamp.
//    BYTE            payLoad[]         IN   Array of bytes with the data to be converted.
//
// Return amDeviceType SPEED_SENSOR, CADENCE_SENSOR, POWER_METER, AERO_SENSOR, or HEART_RATE_METER
//             if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
//---------------------------------------------------------------------------------------------------
amDeviceType antHRMProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_HRM_TYPE )
    {
        result = processHRMSensor( deviceIDNo, timeStampBuffer, payLoad );
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

amDeviceType antHRMProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        if ( isHeartRateSensor( inputBuffer ) )
        {
            result = processHRMSensorSemiCooked( inputBuffer );
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

void antHRMProcessing::readDeviceFileLine
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
        else if ( deviceType == C_HEART_RATE_DEVICE_ID )
        {
            amString deviceName = words[ 1 ];
            if ( isHeartRateSensor( deviceName ) )
            {
                appendHRMSensor( deviceName );
            }
        }
    }
}

void antHRMProcessing::createHRMResultString
(
    unsigned int dataPage,
    unsigned int heartRate,
    unsigned int deltaHeartBeatEventTime,
    unsigned int deltaHeartBeatCount,
    double       totalHeartBeatEventTime,
    unsigned int totalHeartBeatCount,
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3,
    double       additionalDoubleData1
)
{
    int dataPageMod128 = dataPage & 0x0F;
    if ( outputAsJSON )
    {
        appendJSONItem( "heart rate", heartRate );
        if ( semiCookedOut )
        {
            appendJSONItem( "delta heart beat event time", deltaHeartBeatEventTime );
            appendJSONItem( "delta heart beat count",      deltaHeartBeatCount );
        }
        else
        {
            appendJSONItem( "total heart beat event time", totalHeartBeatEventTime, getValuePrecision() );
            appendJSONItem( "total heart beat count",      totalHeartBeatCount );
        }
        appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        if ( dataPageMod128 == 1 )
        {
            if ( semiCookedOut )
            {
                appendJSONItem( "delta operating time", additionalData1 );
            }
            else
            {
                appendJSONItem( "operating time", 2 * additionalData1 );
            }
        }
        else if ( dataPageMod128 == 2 )
        {
            appendJSONItem( C_MANUFACTURER_JSON,  additionalData1 );
            appendJSONItem( C_SERIAL_NUMBER_JSON, additionalData2 );
        }
        else if ( dataPageMod128 == 3 )
        {
            appendJSONItem( C_MANUFACTURER_JSON,      additionalData1 );
            appendJSONItem( C_HARDWARE_REVISION_JSON, additionalData2 );
            appendJSONItem( C_MODEL_NUMBER_JSON,      additionalData3 );
        }
        else if ( dataPageMod128 == 4 )
        {
            if ( semiCookedOut )
            {
                appendJSONItem( "delta previous valid heart beat event time", additionalData1 );
            }
            else
            {
                appendJSONItem( "previous valid heart beat event time", additionalDoubleData1, getValuePrecision() );
            }
            appendJSONItem( "manufacturer specific data", additionalData2 );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( heartRate );
            appendOutput( deltaHeartBeatEventTime );
            appendOutput( deltaHeartBeatCount );
            appendOutput( dataPage );

            if ( ( dataPageMod128 == 1 ) || ( dataPageMod128 == 2 ) || ( dataPageMod128 == 3 ) || ( dataPageMod128 == 4 ) )
            {
                appendOutput( additionalData1 ); //  deltaOperatingTime (1) / manufacturerID (2) / hwVersion (3) // deltaPrevHeartBeatEventTime (4)
                if ( ( dataPageMod128 == 2 ) || ( dataPageMod128 == 3 ) || ( dataPageMod128 == 4 ) )
                {
                    appendOutput( additionalData2 ); //  serialNumber (2) / swVersion (3) / manufacturerSpecificData (4)
                    if ( dataPageMod128 == 3 )
                    {
                        appendOutput( additionalData3 ); //  modelNumber (3)
                    }
                }
            }
        }
        else
        {
            appendOutput( heartRate );
            appendOutput( totalHeartBeatCount );
            appendOutput( totalHeartBeatEventTime, 3 );
            appendOutput( dataPage );
            if ( dataPageMod128 == 1 )
            {
                appendOutput( 2 * additionalData1 );                         // operatingTimeSeconds
            }
            else if ( ( dataPageMod128 == 2 ) || ( dataPageMod128 == 3 ) )
            {
                appendOutput( additionalData1 );                             //  manufacturerID (2) / hwVersion (3)
                appendOutput( additionalData2 );                             //  serialNumber (2)   / swVersion (3)
                if ( dataPageMod128 == 3 )
                {
                    appendOutput( additionalData3 );                         //  modelNumber (3)
                }
            }
            else if ( dataPageMod128 == 4 )
            {
                appendOutput( additionalDoubleData1, getTimePrecision() );   //  prevHeartBeatEventTime
                appendOutput( additionalData2 );                             //  manufacturerSpecificData
            }
        }
    }
}

