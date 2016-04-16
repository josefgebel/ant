// -------------------------------------------------------------------------------------------------------------------------
// System C++ libraries
#include <iostream>
#include <fstream>
#include <iomanip>

// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "b2t_utils.h"
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
    setMaxZeroTime( C_MAX_ZERO_TIME_HRM );
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
    const std::string &deviceID
)
{
    bool result = startsWith( deviceID, C_HEART_RATE_DEVICE_ID );
    return result;
}

bool antHRMProcessing::appendHRMSensor
(
    const std::string &sensorID
)
{
    bool result = isHeartRateSensor( sensorID );
    if ( result )
    {
        if ( !isRegisteredSensor( sensorID ) )
        {
            heartRateSensorTable.insert( std::pair<std::string, bool>( sensorID, true ) );
        }
    }
    return result;
}

bool antHRMProcessing::isRegisteredSensor
(
    const std::string &deviceID
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
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
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
    std::string  sensorID                          = std::string( C_HRM_DEVICE_HEAD) + deviceIDNo;


    if ( isRegisteredDevice( sensorID ) )
    {
        if ( ( eventTimeTable.count    ( sensorID ) == 0 ) ||
             ( eventCountTable.count   ( sensorID ) == 0 ) ||
             ( operatingTimeTable.count( sensorID ) == 0 ) )
        {
            eventTimeTable.insert( std::pair<std::string, double>( sensorID, 0.0 ) );
            eventCountTable.insert( std::pair<std::string, double>( sensorID, 0.0 ) );
            operatingTimeTable.insert( std::pair<std::string, unsigned int>( sensorID, 0 ) );
        }

        dataPage = hex( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        heartRate = hex( payLoad[ 7 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Heart Rate", payLoad[ 7 ], heartRate );
        }

        heartBeatEventTime         = hex( payLoad[ 5 ], payLoad[ 4 ] );
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

        heartBeatCount              = hex( payLoad[ 6 ] );
        rollOver                    = 256;  // 256 = 1 Byte
        deltaHeartBeatCount         = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, heartBeatCount );
        totalHeartBeatCount         = totalCountTable[ sensorID ] + deltaHeartBeatCount;
        totalCountTable[ sensorID ] = totalHeartBeatEventTime;
        if ( diagnostics )
        {
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Heart Beat Count", deltaHeartBeatCount, auxBuffer );
        }

        switch ( dataPage & 0x0F )
        {
            case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                     result = HEART_RATE_METER;
                     break;

            case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                     result          = HEART_RATE_METER;
                     additionalData2 = hex( payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ] ); // Operating Time
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
                     additionalData1 = hex( payLoad[ 1 ] );                // Manufacturer ID
                     additionalData2 = hex( payLoad[ 3 ], payLoad[ 2 ] );  // Serial Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Manufacturer ID", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Serial Number", payLoad[ 3 ], payLoad[ 2 ], additionalData2 );
                     }
                     break;

            case  3: // - - Page 3: Product Information  - - - - - - - - - - - - - -
                     result          = HEART_RATE_METER;
                     additionalData1 = hex( payLoad[ 1 ] );   // H/W Version
                     additionalData2 = hex( payLoad[ 2 ] );   // S/W Version
                     additionalData3 = hex( payLoad[ 3 ] );   // Model Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Hardware Version", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Software Version", payLoad[ 2 ], additionalData2 );
                         appendDiagnosticsLine( "Model Number", payLoad[ 3 ], additionalData3 );
                     }
                     break;

            case  4: // - - Page 4: Previous Heartbeat Time - - - - - - - - - - - - -
                     result                          = HEART_RATE_METER;
                     additionalData2                 = hex( payLoad[ 1 ] );                 // Manufacturer Specific Data;
                     additionalData3                 = hex( payLoad[ 3 ], payLoad[ 2 ] );   // Previous Heart Beat Event Time
                     rollOver                        = 65536;  // 256^2
                     additionalData1                 = getDeltaInt( rollOverHappened, sensorID, rollOver, previousHeartRateTable, additionalData3 );
                     additionalDoubleData1           = heartBeatTimeTable[ sensorID ] + ( ( double ) additionalData1 ) / 1024.0;
                     heartBeatTimeTable[ sensorID ]  = additionalDoubleData1;
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
        appendOutputFooter( b2tVersion );
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
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
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
        std::string   timeStampBuffer;
        std::string   curVersion              = b2tVersion;
        std::string   sensorID;
        std::string   timeStampString;
        std::string   semiCookedString;
        bool          commonPage              = false;
        bool          outputPageNo            = true;

        if ( nbWords > 6 )
        {
            sensorID         = words[ counter++ ];                  // 0
            timeStampBuffer  = words[ counter++ ];                  // 1
            semiCookedString = words[ counter++ ];                  // 2
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isHeartRateSensor( sensorID ) )
            {
                startCounter                = counter;
                heartRate                   = ( unsigned int ) strToInt( words[ counter++ ] );    // 3
                deltaHeartBeatEventTime     = ( unsigned int ) strToInt( words[ counter++ ] );    // 4
                deltaHeartBeatCount         = ( unsigned int ) strToInt( words[ counter++ ] );    // 5
                dataPage                    = ( unsigned int ) strToInt( words[ counter++ ] );    // 6
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

                switch ( dataPage & 0x0F )
                {
                    case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                             result = HEART_RATE_METER;
                             break;

                    case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                             if ( nbWords > 7 )
                             {
                                 result          = HEART_RATE_METER;
                                 additionalData1 = ( unsigned int ) strToInt(  words[ counter++ ] );   // deltaOperatingTime
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
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );   // manufacturerID
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );   // serialNumber
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
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );   // hwVersion
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );   // swVersion
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );   // modelNumber
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
                                 result                     = HEART_RATE_METER;
                                 additionalData2            = ( unsigned int ) strToInt( words[ counter++ ] ); // manufacturerSpecificData;
                                 additionalData1            = ( unsigned int ) strToInt( words[ counter++ ] ); // deltaPrevHeartBeatEventTime;
                                 additionalDoubleData1      = heartBeatTimeTable[ sensorID ] + ( ( double ) additionalData1 ) / 1024.0;
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

        if ( result == HEART_RATE_METER )
        {
            if ( nbWords > counter )
            {
                curVersion = words[ counter ];
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Version", curVersion );
                }
            }
            createOutputHeader( sensorID, timeStampBuffer );
            if ( commonPage )
            {
                commonPage = processCommonPagesSemiCooked( words, startCounter, dataPage, outputPageNo );
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

        if ( result == OTHER_DEVICE )
        {
            resetOutBuffer();
            if ( outputUnknown )
            {
                outBuffer = inputBuffer;
            }
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
//    int                deviceType        IN   Device type (SPCAD, SPEED, CADENCE, HRM, AERO, POWER).
//    const std::string &deviceID          IN   Device ID (number).
//    const std::string &timeStampBuffer   IN   Time stamp.
//    unsigned char      payLoad[]         IN   Array of bytes with the data to be converted.
//
// Return amDeviceType SPEED_SENSOR, CADENCE_SENSOR, POWER_METER, AERO_SENSOR, or HEART_RATE_METER
//             if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
//---------------------------------------------------------------------------------------------------
amDeviceType antHRMProcessing::processSensor
(
    int                deviceType,
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
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
            int deviceIDNoAsInt = strToInt( deviceIDNo );
            createUnknownDeviceTypeString( deviceType, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

amDeviceType antHRMProcessing::processSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
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
                outBuffer = inputBuffer;
            }
        }
    }

    return result;
}

// ----------------------------------------------------------------------------
// Read a line from the deviceIDs file
// If the line contains a heart rate monitor device definition
//     Return true.
// Else:
//     Return false.
// ----------------------------------------------------------------------------
bool antHRMProcessing::evaluateDeviceLine
(
    const amSplitString &words
)
{
    bool         result  = false;
    unsigned int nbWords = words.size();
    if ( nbWords > 2 )
    {
        std::string deviceType = words[ 0 ];
        std::string deviceName = words[ 1 ];
        if ( ( deviceType == C_HEART_RATE_DEVICE_ID ) && isHeartRateSensor( deviceName ) )
        {
            result = appendHRMSensor( deviceName );
        }
    }
    return result;
}

int antHRMProcessing::readDeviceFileStream
(
    std::ifstream &deviceFileStream
)
{
    char line[ C_BUFFER_SIZE ];
    int  errorCode = 0;
    amSplitString words;

    std::string  deviceType = "";
    std::string  deviceName = "";
    unsigned int nbWords    = 0;

    while ( true )
    {
        deviceFileStream.getline( line, C_BUFFER_SIZE, '\n' );
        if ( deviceFileStream.fail() || deviceFileStream.eof() )
        {
            break;
        }
        const char *lPtr = line;
        while ( isWhiteChar( *lPtr ) )
        {
            ++lPtr;
        }
        if ( ( *lPtr == 0 ) || ( *lPtr == C_COMMENT_SYMBOL ) )
        {
            continue;
        }

        nbWords = words.split( line, C_COMMENT_SYMBOL_AS_STRING );
        if ( nbWords > 1 )
        {
            deviceType = words[ 0 ];
            deviceName = words[ 1 ];
            if ( deviceType == C_INCLUDE_FILE )
            {
                const char *includeFileName = deviceName.c_str();
                std::ifstream devicesIncludeFileStream( includeFileName );
                if ( devicesIncludeFileStream.fail() )
                {
                    strcat( errorMessage,"ERROR while opening devices ID include file \"" );
                    strcat( errorMessage,includeFileName );
                    strcat( errorMessage,"\".\n" );
                    errorCode = E_READ_FILE_NOT_OPEN;
                }
                else
                {
                    errorCode = readDeviceFileStream( devicesIncludeFileStream );
                    devicesIncludeFileStream.close();
                }
            }
            else if ( ( deviceType == C_HEART_RATE_DEVICE_ID ) && isHeartRateSensor( deviceName ) )
            {
                evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
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
        if ( ( dataPage & 0x0F ) == 1 )
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
        else if ( ( dataPage & 0x0F ) == 2 )
        {
            appendJSONItem( C_MANUFACTURER_JSON,  additionalData1 );
            appendJSONItem( C_SERIAL_NUMBER_JSON, additionalData2 );
        }
        else if ( ( dataPage & 0x0F ) == 3 )
        {
            appendJSONItem( C_MANUFACTURER_JSON,      additionalData1 );
            appendJSONItem( C_HARDWARE_REVISION_JSON, additionalData2 );
            appendJSONItem( C_MODEL_NUMBER_JSON,      additionalData3 );
        }
        else if ( ( dataPage & 0x0F ) == 4 )
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

            if ( ( ( dataPage & 0x0F ) == 1 ) || ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) || ( ( dataPage & 0x0F ) == 4 ) )
            {
                appendOutput( additionalData1 ); //  deltaOperatingTime (1) / manufacturerID (2) / hwVersion (3) // deltaPrevHeartBeatEventTime (4)
                if ( ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) || ( ( dataPage & 0x0F ) == 4 ) )
                {
                    appendOutput( additionalData2 ); //  serialNumber (2) / swVersion (3) / manufacturerSpecificData (4)
                    if ( ( dataPage & 0x0F ) == 3 )
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
            if ( ( dataPage & 0x0F ) == 1 )
            {
                appendOutput( 2 * additionalData1 );                         // operatingTimeSeconds
            }
            else if ( ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) )
            {
                appendOutput( additionalData1 );                             //  manufacturerID (2) / hwVersion (3)
                appendOutput( additionalData2 );                             //  serialNumber (2)   / swVersion (3)
                if ( ( dataPage & 0x0F ) == 3 )
                {
                    appendOutput( additionalData3 );                         //  modelNumber (3)
                }
            }
            else if ( ( dataPage & 0x0F ) == 4 )
            {
                appendOutput( additionalDoubleData1, getTimePrecision() );   //  prevHeartBeatEventTime
                appendOutput( additionalData2 );                             //  manufacturerSpecificData
            }
        }
    }
}

