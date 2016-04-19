// -------------------------------------------------------------------------------------------------------------------------

// System C++ libraries
#include <iostream>
#include <fstream>
#include <iomanip>

// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "b2t_utils.h"
#include "ant_environment_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// ENVIRONMENT ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antEnvironmentProcessing::antEnvironmentProcessing
(
    void
) : antProcessing()
{
}

bool antEnvironmentProcessing::isEnvironmentSensor
(
    const std::string &deviceID
)
{
    bool result = startsWith( deviceID, C_ENV_DEVICE_HEAD );
    return result;
}

bool antEnvironmentProcessing::appendEnvironmentSensor
(
    const std::string &sensorID
)
{
    bool result = isEnvironmentSensor( sensorID );
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
// ENVRIONMENT
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antEnvironmentProcessing::processEnvironmentSensor
(
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
    unsigned int dataPage                          = 0;
    unsigned int auxInt0                           = 0;
    unsigned int auxInt1                           = 0;
    unsigned int auxInt2                           = 0;
    unsigned int additionalData1                   = 0;
    unsigned int additionalData2                   = 0;
    unsigned int additionalData3                   = 0;
    unsigned int additionalData4                   = 0;
    amDeviceType result                            = OTHER_DEVICE;
    std::string  sensorID                          = std::string( C_ENV_DEVICE_HEAD ) + deviceIDNo;
    bool         commonPage                        = false;
    bool         outputPageNo                      = true;

    if ( isRegisteredDevice( sensorID ) )
    {
        dataPage = hex( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        switch ( dataPage & 0x0F )
        {
            case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                     //     Bytes 1, 2, and 3: 0xFF.
                     result          = ENVIRONMENT_SENSOR;
                     auxInt0         = hex( payLoad[ 3 ] );                                              // Transmission Info
                     additionalData1 = ( auxInt0 << 4 ) & 3;                                             // Local Time
                     additionalData2 = ( auxInt0 << 2 ) & 3;                                             // UTC Time
                     additionalData3 = auxInt0 & 3;                                                      // Transmission Rate
                     additionalData4 = hex( payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ] );    // Supported Pages
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Transmission Info", payLoad[ 3 ], auxInt0 );
                         appendDiagnosticsLine( "Local Time",        additionalData1 );
                         appendDiagnosticsLine( "UTC Time",          additionalData2 );
                         appendDiagnosticsLine( "Transmission Rate", additionalData3 );
                         appendDiagnosticsLine( "Supported Pages", payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ], additionalData4 );
                     }
                     break;
            case  1: // - - Page 1: Temperature - - - - - - - - - - - - - - - - -
                     //     Byte 3 and Bits 4-7 of Byte 4: Low  Temp * 10 (Byte 3 LSB)
                     //     Bits 0-3 of Byte 4 and Byte 5: High Temp * 10 (Byte 5 MSB)
                     //     Bytes 6 & 7                  : Current Temp * 100 (byte 6 LSB, byte 7 MSB)
                     result          = ENVIRONMENT_SENSOR;
                     additionalData4 = hex( payLoad[ 2 ] );                                              // Event Count
                     auxInt0         = hex( payLoad[ 4 ] );
                     auxInt1         = auxInt0 >> 4;
                     auxInt2         = auxInt0 & 0x0F;
                     additionalData2 = ( auxInt1 * 256 ) + hex( payLoad[ 3 ] );                          // Low Temperature 24h
                     additionalData3 = ( hex( payLoad[ 5 ] ) << 4 ) + auxInt2;                           // High Temperature 24h
                     additionalData1  = hex( payLoad[ 7 ], payLoad[ 6 ] );                               // Current Temperature
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Current Temperature * 100", payLoad[ 7 ], payLoad[ 6 ], additionalData1 );
                         strcpy( auxBuffer, " (Second Byte: Upper 4 Bits)" );
                         appendDiagnosticsLine( "Low Temperature 24h * 10", payLoad[ 3 ], payLoad[ 4 ], additionalData2, auxBuffer );
                         strcpy( auxBuffer, " (First Byte: Lower 4 Bits)" );
                         appendDiagnosticsLine( "High Temperature 24h * 10", payLoad[ 4 ], payLoad[ 5 ], additionalData3, auxBuffer );
                         appendDiagnosticsLine( "Event Count", payLoad[ 2 ], additionalData4 );
                     }
                     break;

            default: commonPage = true;
                     result     = ENVIRONMENT_SENSOR;
                     break;
        }
    }

    if ( result == ENVIRONMENT_SENSOR )
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
            createENVResultString( dataPage, additionalData1, additionalData2, additionalData3, additionalData4 );
        }
        appendOutputFooter( b2tVersion );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = strToInt( deviceIDNo );
            createUnknownDeviceTypeString( C_ENV_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processEnvironmentSensorSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
// Depending on the value of <data_page> & 127:
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antEnvironmentProcessing::processEnvironmentSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        unsigned int  dataPage        = 0;
        unsigned int  counter         = 0;
        unsigned int  startCounter    = 0;
        unsigned int  additionalData1 = 0;
        unsigned int  additionalData2 = 0;
        unsigned int  additionalData3 = 0;
        unsigned int  additionalData4 = 0;
        std::string   curVersion      = b2tVersion;
        std::string   timeStampBuffer;
        std::string   sensorID;
        std::string   timeStampString;
        std::string   semiCookedString;
        std::string   dataPageString;
        bool          commonPage      = false;
        bool          outputPageNo    = true;
        amSplitString words;
        unsigned int  nbWords         = words.split( inputBuffer );

        // Semi-cooked string has the syntax
        //    "<device_id>\t<time_stamp>\t<semi_cooked_symbol>\t<data_page>\t...."

        if ( nbWords > 3 )
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
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isEnvironmentSensor( sensorID ) )
            {
                dataPage     = ( unsigned int ) strToInt( words[ counter++ ] );    // 3
                startCounter = counter;

                switch ( dataPage & 0x0F )
                {
                    case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                             //
                             if ( nbWords > 7 )
                             {
                                 result          = ENVIRONMENT_SENSOR;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );                      // Local Time
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );                      // UTC Time
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );                      // Transmission Rate
                                 additionalData4 = ( unsigned int ) strToInt( words[ counter++ ] );                      // Supported Pages
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Local Time",        additionalData1 );
                                     appendDiagnosticsLine( "UTC Time",          additionalData2 );
                                     appendDiagnosticsLine( "Transmission Rate", additionalData2 );
                                     appendDiagnosticsLine( "Supported Pages",   additionalData4 );
                                 }
                             }
                             break;
                    case  1: // - - Page 1: Temperature  - - - - - - - - - - - - - - - - - - - - - - - - -
                             //
                             if ( nbWords > 7 )
                             {
                                 result          = ENVIRONMENT_SENSOR;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );                      // Current Temperature
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );                      // Low Temperature 24h
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );                      // High Temperature 24h
                                 additionalData4 = ( unsigned int ) strToInt( words[ counter++ ] );                      // Event Count
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Current Temperature * 100", additionalData1 );
                                     appendDiagnosticsLine( "Low Temperature * 10",      additionalData2, " (last 24h)" );
                                     appendDiagnosticsLine( "High Temperature * 10",     additionalData3, " (last 24h)" );
                                     appendDiagnosticsLine( "Event Count",               additionalData4 );
                                 }
                             }
                             break;
                    default: commonPage = true;
                             result     = ENVIRONMENT_SENSOR;
                             break;
                }
            }
        }

        if ( result == ENVIRONMENT_SENSOR )
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
                commonPage = processCommonPagesSemiCooked( words, startCounter, dataPage, outputPageNo );
                if ( !commonPage )
                {
                    result = OTHER_DEVICE;
                }
            }
            else
            {
                createENVResultString( dataPage, additionalData1, additionalData2, additionalData3, additionalData4 );
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
//    int                deviceType        IN   Device type
//    const std::string &deviceID          IN   Device ID (number).
//    const std::string &timeStampBuffer   IN   Time stamp.
//    unsigned char      payLoad[]         IN   Array of bytes with the data to be converted.
//
// Return amDeviceType SPEED_SENSOR, CADENCE_SENSOR, POWER_METER, AERO_SENSOR, or HEART_RATE_METER
//             if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
//---------------------------------------------------------------------------------------------------
amDeviceType antEnvironmentProcessing::processSensor
(
    int                deviceType,
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_ENV_TYPE )
    {
        result = processEnvironmentSensor( deviceIDNo, timeStampBuffer, payLoad );
    }
    else if ( outputUnknown )
    {
        int deviceIDNoAsInt = strToInt( deviceIDNo );
        createUnknownDeviceTypeString( deviceType, deviceIDNoAsInt, timeStampBuffer, payLoad );
    }
    else
    {
        resetOutBuffer();
    }

    return result;
}

amDeviceType antEnvironmentProcessing::processSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        if ( isEnvironmentSensor( inputBuffer ) )
        {
            result = processEnvironmentSensorSemiCooked( inputBuffer );
        }
        else
        {
            resetOutBuffer();
            if ( getOutputUnknown() )
            {
                outBuffer = inputBuffer;
            }
        }
    }

    return result;
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
bool antEnvironmentProcessing::evaluateDeviceLine
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
        if ( ( deviceType == C_ENV_DEVICE_HEAD ) && isEnvironmentSensor( deviceName ) )
        {
            result = appendEnvironmentSensor( deviceName );
        }
    }
    return result;
}

int antEnvironmentProcessing::readDeviceFileStream
(
    std::ifstream &deviceFileStream
)
{
    char          line[ C_BUFFER_SIZE ];
    int           errorCode = 0;
    unsigned int  nbWords    = 0;
    amSplitString words;
    std::string   deviceType = "";
    std::string   deviceName = "";

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
            else if ( ( deviceType == C_ENV_DEVICE_HEAD ) && isEnvironmentSensor( deviceName ) )
            {
                evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
}

void antEnvironmentProcessing::createENVResultString
(
    unsigned int dataPage,
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3,
    unsigned int additionalData4
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "data page", dataPage );
            if ( ( dataPage & 0x0F ) == 0 )
            {
                appendJSONItem( "local time",        additionalData1 );
                appendJSONItem( "utc time",          additionalData2 );
                appendJSONItem( "transmission rate", additionalData3 );
                appendJSONItem( "supported pages",   additionalData4 );
            }
            else if ( ( dataPage & 0x0F ) == 1 )
            {
                appendJSONItem( "current temperature times 100", additionalData1 );
                appendJSONItem( "low temperature 24h times 10",  additionalData2 );
                appendJSONItem( "high temperature 24h times 10", additionalData3 );
                appendJSONItem( "event count",                   additionalData4 );
            }
        }
        else
        {
            appendOutput( dataPage );
            appendOutput( additionalData1 );
            appendOutput( additionalData2 );
            appendOutput( additionalData3 );
            appendOutput( additionalData4 );
        }
    }
    else
    {
        double currTemp                         = 0;
        double lowTemp24h                       = 0;
        double highTemp24h                      = 0;
        char   auxBuffer[ C_SMALL_BUFFER_SIZE ] = { 0 };
        char   auxBuffer1[ C_TINY_BUFFER_SIZE ] = { 0 };

        if ( ( ( dataPage & 0x0F ) == 0 ) && ( additionalData4 != 0 ) )
        {
            *auxBuffer = 0;
            for ( int page = 0; page < 32; ++page )
            {
                if ( additionalData4 & ( 1 << page ) )
                {
                    if ( *auxBuffer != 0 )
                    {
                        strcat( auxBuffer, ", " );
                    }
                    sprintf( auxBuffer1, "%u", page );
                    strcat( auxBuffer, auxBuffer1 );
                }
            }
        }
        else if ( ( dataPage & 0x0F ) == 1 )
        {
            currTemp    = convertTemp100( additionalData1 );
            lowTemp24h  = convertTemp10 ( additionalData2 );
            highTemp24h = convertTemp10 ( additionalData3 );
        }

        if ( outputAsJSON )
        {
            if ( ( dataPage & 0x0F ) == 0 )
            {
                appendJSONItem( "local time", ( additionalData1 == 0 ) ? "set" : (
                                              ( additionalData1 == 1 ) ? "supported and not set" : (
                                              ( additionalData1 == 2 ) ? "supported and set"     : C_UNUSED_JSON ) ) );

                appendJSONItem( "utc time", ( additionalData2 == 0 ) ? "set" : (
                                            ( additionalData2 == 1 ) ? "supported and not set" : (
                                            ( additionalData2 == 2 ) ? "supported and set"     : C_UNUSED_JSON ) ) );

                appendJSONItem( "transmission rate", ( additionalData3 == 0 ) ? "0.5" : (
                                                     ( additionalData3 == 2 ) ? "4.0" : C_UNUSED ) );

                appendJSONItem( "supported pages", ( additionalData4 == 0 ) ? C_NONE_JSON : auxBuffer );
            }
            else if ( ( dataPage & 0x0F ) == 1 )
            {
                appendJSONItem( "current temperature",  currTemp, 2 );
                appendJSONItem( "low temperature 24h",  lowTemp24h, 1 );
                appendJSONItem( "high temperature 24h", highTemp24h, 1 );
                appendJSONItem( "event count",          additionalData1 );
            }
        }
        else
        {
            if ( ( dataPage & 0x0F ) == 0 )
            {
                appendOutput( "LOCAL_TIME" );
                appendOutput( ( additionalData1 == 0 ) ? C_SET_ID : (
                              ( additionalData1 == 1 ) ? C_SUPPORTED_AND_NOT_SET_ID : (
                              ( additionalData1 == 2 ) ? C_SUPPORTED_AND_SET_ID : C_UNUSED ) ) );

                appendOutput( "UTC_TIME" );
                appendOutput( ( additionalData2 == 0 ) ? C_SET_ID : (
                              ( additionalData2 == 1 ) ? C_SUPPORTED_AND_NOT_SET_ID : (
                              ( additionalData2 == 2 ) ? C_SUPPORTED_AND_SET_ID : C_UNUSED ) ) );

                appendOutput( "TRANSMISSION_HZ" );
                appendOutput( ( additionalData3 == 0 ) ? "0.5" : (
                              ( additionalData3 == 2 ) ? "4.0" : C_UNUSED ) );

                appendOutput( "SUPPORTED_PAGES" );
                appendOutput( ( additionalData4 == 0 ) ? C_NONE_ID : auxBuffer );
            }
            else if ( ( dataPage & 0x0F ) == 1 )
            {
                appendOutput( "CURR_TEMP" );
                appendOutput( currTemp, 2, "C" );
                appendOutput( "LOW_TEMP_24H" );
                appendOutput( lowTemp24h, 1, "C" );
                appendOutput( "HIGH_TEMP_24H" );
                appendOutput( highTemp24h, 1, "C" );
                appendOutput( "EVENT_NO" );
                appendOutput( additionalData4 );
            }
        }
    }
}

double antEnvironmentProcessing::convertTemp10
(
    int temp10
)
{
    double temp = C_DBL_UNDEFINED;
    if ( temp10 != 0x800  )
    {
        if ( temp10 > 0x7FF )
        {
            int aux = temp10 - 0xFFF;
            temp = aux * 0.1;
        }
        else
        {
            temp = temp10 * 0.1;
        }
    }
    return temp;
}

double antEnvironmentProcessing::convertTemp100
(
    int temp100
)
{
    double temp = C_DBL_UNDEFINED;
    if ( temp100 != 0x8000  )
    {
        if ( temp100 & 0x8000 )
        {
            temp = ( temp100 - 0xFFFF ) * 0.01;
        }
        else
        {
            temp = temp100 * 0.01;
        }
    }
    return temp;
}

