// -------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>

// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "b2t_utils.h"
#include "ant_stride_speed_dist_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// SBSDM ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antStrideSpeedDistProcessing::antStrideSpeedDistProcessing
(
    void
) : antProcessing()
{
    reset();
}

bool antStrideSpeedDistProcessing::isStrideSpeedSensor
(
    const std::string &deviceID
)
{
    bool result = startsWith( deviceID, C_SBSDM_DEVICE_HEAD );
    return result;
}

bool antStrideSpeedDistProcessing::appendStrideSpeedDistSensor
(
    const std::string &sensorID
)
{
    bool result = isStrideSpeedSensor( sensorID );
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
// SBSDM (Stride Based Speed and Distance Monitor)
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antStrideSpeedDistProcessing::processStrideBasedSpeedAndDistanceSensor
(
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    amDeviceType result                            = OTHER_DEVICE;
    std::string  sensorID                          = std::string( C_SBSDM_DEVICE_HEAD ) + deviceIDNo;
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
    unsigned int dataPage                          = 0;
    unsigned int auxInt1                           = 0;
    unsigned int auxInt2                           = 0;
    unsigned int auxInt3                           = 0;
    unsigned int additionalData1                   = 0;
    unsigned int additionalData2                   = 0;
    unsigned int additionalData3                   = 0;
    unsigned int additionalData4                   = 0;
    unsigned int additionalData5                   = 0;
    unsigned int additionalData6                   = 0;
    unsigned int additionalData7                   = 0;
    unsigned int additionalData8                   = 0;
    unsigned int rollOver                          = 0;
    bool         rollOverHappened                  = false;
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
            case  1: result = STRIDE_BASED_SD_SENSOR;

                     if ( totalTimeTable.count( sensorID ) > 0 )
                     {
                         totalTimeTable.insert( std::pair<std::string, double>( sensorID, 0 ) );
                         totalDistTable.insert( std::pair<std::string, double>( sensorID, 0 ) );
                         eventTimeTable.insert( std::pair<std::string, unsigned int>( sensorID, 0 ) );
                         eventCountTable.insert( std::pair<std::string, int>( sensorID, 0 ) );
                     }

                     rollOver     = 256;  // 256 (1 byte)
                     auxInt1      = hex( payLoad[ 2 ] );     // Time (integer part)
                     auxInt2      = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, auxInt1 );
                     if ( semiCookedOut )
                     {
                         additionalData1 = auxInt2;
                     }
                     else
                     {
                         additionalData1               = auxInt2 + totalTimeIntTable[ sensorID ];
                         totalTimeIntTable[ sensorID ] = additionalData1;
                     }
                     additionalData2 = hex( payLoad[ 3 ] );  // Time (fractional part)

                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Time (fractional part)", payLoad[ 1 ], additionalData2 );
                         appendDiagnosticsLine( "Time (integer part)", payLoad[ 2 ], auxInt1 );
                         *auxBuffer = 0;
                         if ( rollOverHappened )
                         {
                             sprintf( auxBuffer, "(Rollover [%u] occurred)", rollOver );
                         }
                         appendDiagnosticsLine( "Delta Time", auxInt2, auxBuffer );
                         if ( !semiCookedOut )
                         {
                             appendDiagnosticsLine( "Total Time (int)", additionalData1 );
                         }
                     }

                     auxInt1      = hex( payLoad[ 3 ] );     // Distance (integer part)
                     rollOver     = 256;  // 256 (1 byte)
                     auxInt2      = getDeltaInt( rollOverHappened, sensorID, rollOver, eventDistTable, auxInt1 );
                     if ( semiCookedOut )
                     {
                         additionalData3 = auxInt2;
                     }
                     else
                     {
                         additionalData3               = auxInt2 + totalTimeIntTable[ sensorID ];
                         totalTimeIntTable[ sensorID ] = additionalData3;
                     }

                     auxInt3         = hex( payLoad[ 4 ] );
                     additionalData4 = auxInt3 >> 4;         // Distance (fractional part)

                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Dist (fractional part)", payLoad[ 4 ], additionalData4, " (upper 4 bits)" );
                         appendDiagnosticsLine( "Dist (integer part)", payLoad[ 3 ], auxInt1 );
                         *auxBuffer = 0;
                         if ( rollOverHappened )
                         {
                             sprintf( auxBuffer, "(Rollover [%u] occurred)", rollOver );
                         }
                         appendDiagnosticsLine( "Delta Dist", auxInt2, auxBuffer );
                         if ( !semiCookedOut )
                         {
                             appendDiagnosticsLine( "Total Dist (int)", additionalData3 );
                         }
                     }

                     additionalData5 = ( auxInt3 << 4 ) >> 4;         // Speed (integer part)
                     additionalData6 = hex( payLoad[ 5 ] );           // Speed (fractional part)
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Speed (fractional part)", payLoad[ 4 ], additionalData5, " (lower 4 bits)" );
                         appendDiagnosticsLine( "Speed (integer part)",    payLoad[ 5 ], additionalData6 );
                     }

                     auxInt1      = hex( payLoad[ 6 ] );           // Stride count
                     rollOver     = 256;  // 256 (1 byte)
                     auxInt2      = getDeltaInt( rollOverHappened, sensorID, rollOver, strideCountTable, auxInt1 );
                     if ( semiCookedOut )
                     {
                         additionalData7 = auxInt2;
                     }
                     else
                     {
                         additionalData7                   = auxInt2 + totalStrideCountTable[ sensorID ];
                         totalStrideCountTable[ sensorID ] = additionalData7;
                     }
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Stride Count", payLoad[ 6 ], auxInt1 );
                         *auxBuffer = 0;
                         if ( rollOverHappened )
                         {
                             sprintf( auxBuffer, "(Rollover [%u] occurred)", rollOver );
                         }
                         appendDiagnosticsLine( "Delta Stride Count", auxInt2, auxBuffer );
                         if ( !semiCookedOut )
                         {
                             appendDiagnosticsLine( "Total Stride Count", additionalData7 );
                         }
                     }

                     additionalData8 = hex( payLoad[ 7 ] );           // Latency
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Latency", payLoad[ 7 ], additionalData8 );
                     }

                     break;

            case  2:
            case  3: result = STRIDE_BASED_SD_SENSOR;

                     additionalData3 = hex( payLoad[ 3 ] );     // Cadence (integer part)
                     auxInt3         = hex( payLoad[ 4 ] );
                     additionalData4 = auxInt3 >> 4;            // Cadence (fractional part)

                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Cadence (fractional part)", payLoad[ 4 ], additionalData4, " (upper 4 bits)" );
                         appendDiagnosticsLine( "Cadence (integer part)", payLoad[ 3 ], auxInt1 );
                     }

                     additionalData5 = ( auxInt3 << 4 ) >> 4;         // Speed (integer part)
                     additionalData6 = hex( payLoad[ 5 ] );           // Speed (fractional part)
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Speed (fractional part)", payLoad[ 4 ], additionalData5, " (lower 4 bits)" );
                         appendDiagnosticsLine( "Speed (integer part)",    payLoad[ 5 ], additionalData6 );
                     }

                     if ( dataPage == 3 )
                     {
                         additionalData7 = hex( payLoad[ 6 ] );       // Calories
                         if ( diagnostics )
                         {
                             appendDiagnosticsLine( "Calories", payLoad[ 6 ], additionalData7 );
                         }
                     }

                     additionalData8 = hex( payLoad[ 7 ] );           // Status
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Status", payLoad[ 7 ], additionalData8 );
                     }

                     break;

            case 16: result = STRIDE_BASED_SD_SENSOR;
                     additionalData1 = hex( payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ] );                   // Strides
                     additionalData2 = hex( payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ] );     // Distances
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Strides",  payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ],               additionalData1 );
                         appendDiagnosticsLine( "Distance", payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ], additionalData2 );
                     }

            case 22: result = STRIDE_BASED_SD_SENSOR;
                     additionalData1 = hex( payLoad[ 1 ] );                 // Capabilities
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Capabilities", payLoad[ 1 ], additionalData1 );
                     }

                     break;

            default: commonPage = true;
                     result = STRIDE_BASED_SD_SENSOR;
                     break;
        }
    }

    if ( result == STRIDE_BASED_SD_SENSOR )
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
            createSBSDMResultString
            (
                dataPage,
                additionalData1,
                additionalData2,
                additionalData3,
                additionalData4,
                additionalData5,
                additionalData6,
                additionalData7,
                additionalData8
            );
        }
        appendOutputFooter( b2tVersion );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = strToInt( deviceIDNo );
            createUnknownDeviceTypeString( C_SBSDM_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}


// ---------------------------------------------------------------------------------------------------
//
// processStrideBasedSpeedAndDistanceSensorSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antStrideSpeedDistProcessing::processStrideBasedSpeedAndDistanceSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        std::string   sensorID;
        std::string   semiCookedString;
        std::string   timeStampBuffer;
        std::string   curVersion      = b2tVersion;
        amSplitString words;
        unsigned int  nbWords         = words.split( inputBuffer );
        unsigned int  startCounter    = 0;
        unsigned int  counter         = 0;
        unsigned int  dataPage        = 0;
        unsigned int  auxInt1         = 0;
        unsigned int  auxInt2         = 0;
        unsigned int  auxInt3         = 0;
        unsigned int  additionalData1 = 0;
        unsigned int  additionalData2 = 0;
        unsigned int  additionalData3 = 0;
        unsigned int  additionalData4 = 0;
        unsigned int  additionalData5 = 0;
        unsigned int  additionalData6 = 0;
        unsigned int  additionalData7 = 0;
        unsigned int  additionalData8 = 0;
        bool          commonPage      = false;
        bool          outputPageNo    = true;

        if ( nbWords > 7 )
        {
            result           = BLOOD_PRESSURE_SENSOR;
            sensorID         = words[ counter++ ];                     //  0
            timeStampBuffer  = words[ counter++ ];                     //  1
            semiCookedString = words[ counter++ ];                     //  2
            if ( diagnostics )
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isStrideSpeedSensor( sensorID ) )
            {
                startCounter = counter;
                dataPage    = ( unsigned int ) strToInt( words[ counter++ ] );       //  3
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Data Page", dataPage );
                }
                counter = startCounter;
                switch ( dataPage & 0x0F )
                {
                    case  1: if ( nbWords > 11 )
                             {
                                 result  = STRIDE_BASED_SD_SENSOR;
                                 auxInt1 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - Delta Time (Integer Part)
                                 if ( semiCookedOut )
                                 {
                                     additionalData1 = auxInt1;
                                 }
                                 else
                                 {
                                     additionalData1               = auxInt1 + totalTimeIntTable[ sensorID ];
                                     totalTimeIntTable[ sensorID ] = additionalData1;
                                 }
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );               //  5 - Time (Fractional Part)
                                 auxInt2         = ( unsigned int ) strToInt( words[ counter++ ] );               //  6 - Delta distance (Integer Part)
                                 if ( semiCookedOut )
                                 {
                                     additionalData3 = auxInt2;
                                 }
                                 else
                                 {
                                     additionalData3               = auxInt2 + totalTimeIntTable[ sensorID ];
                                     totalTimeIntTable[ sensorID ] = additionalData3;
                                 }
                                 additionalData4 = ( unsigned int ) strToInt( words[ counter++ ] );               //  7 - Distance (Fractional Part)
                                 additionalData5 = ( unsigned int ) strToInt( words[ counter++ ] );               //  8 - Speed (Integer Part)
                                 additionalData6 = ( unsigned int ) strToInt( words[ counter++ ] );               //  9 - Speed (Fractional Part)
                                 auxInt3         = ( unsigned int ) strToInt( words[ counter++ ] );               // 10 - Delta Stride Count
                                 if ( semiCookedOut )
                                 {
                                     additionalData7 = auxInt3;
                                 }
                                 else
                                 {
                                     additionalData7                   = auxInt3 + totalStrideCountTable[ sensorID ];
                                     totalStrideCountTable[ sensorID ] = additionalData7;
                                 }
                                 additionalData8 = ( unsigned int ) strToInt( words[ counter++ ] );               // 11 - Latency
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "delta time integer part",     auxInt1 );
                                     if ( semiCookedOut )
                                     {
                                         appendDiagnosticsLine( "total time integer part", additionalData1 );
                                     }
                                     appendDiagnosticsLine( "time fractional part",        additionalData2 );
                                     appendDiagnosticsLine( "delta distance integer part", auxInt2 );
                                     if ( semiCookedOut )
                                     {
                                         appendDiagnosticsLine( "total distance integer part", additionalData3 );
                                     }
                                     appendDiagnosticsLine( "distance fractional part",    additionalData4 );
                                     appendDiagnosticsLine( "delta speed integer part",    additionalData5 );
                                     appendDiagnosticsLine( "speed fractional part",       additionalData6 );
                                     appendDiagnosticsLine( "delta stride count",          auxInt3 );
                                     if ( semiCookedOut )
                                     {
                                         appendDiagnosticsLine( "delta stride count",      additionalData7 );
                                     }
                                     appendDiagnosticsLine( "latency",                     additionalData8 );
                                 }
                             }
                             break;
                    case  2:
                    case  3: if ( ( nbWords > 9 ) || ( ( dataPage == 2 ) && ( nbWords > 8 ) ) )
                             {
                                 result          = STRIDE_BASED_SD_SENSOR;
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - Cadence (Integer Part)
                                 additionalData4 = ( unsigned int ) strToInt( words[ counter++ ] );               //  5 - Cadence (Fractional Part)
                                 additionalData5 = ( unsigned int ) strToInt( words[ counter++ ] );               //  6 - Speed (Integer Part)
                                 additionalData6 = ( unsigned int ) strToInt( words[ counter++ ] );               //  7 - Speed (Fractional Part)
                                 if ( dataPage == 3 )
                                 {
                                     additionalData7 = ( unsigned int ) strToInt( words[ counter++ ] );           //  8 - Calories
                                 }
                                 additionalData8 = ( unsigned int ) strToInt( words[ counter++ ] );               // 8/9 - SBSDM Status
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Cadence integer part",     additionalData3 );
                                     appendDiagnosticsLine( "Cadence fractional part",  additionalData4 );
                                     appendDiagnosticsLine( "delta speed integer part", additionalData5 );
                                     appendDiagnosticsLine( "speed fractional part",    additionalData6 );
                                     if ( dataPage == 3 )
                                     {
                                         appendDiagnosticsLine( "Calories",             additionalData7 );
                                     }
                                     appendDiagnosticsLine( "SMD Status",               additionalData8 );
                                 }
                             }
                             break;

                    case 16: if ( nbWords > 5 )
                             {
                                 result          = STRIDE_BASED_SD_SENSOR;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );  // 4 - Strides
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );  //  5 - Distance
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Strides",  additionalData1 );
                                     appendDiagnosticsLine( "Distance", additionalData2 );
                                 }
                             }
                             break;

                    case 22: if ( nbWords > 4 )
                             {
                                 result          = STRIDE_BASED_SD_SENSOR;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - Capabilities
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Capabilities", additionalData1 );
                                 }
                             }
                             break;

                    default: commonPage = true;
                             result     = STRIDE_BASED_SD_SENSOR;
                             break;
                }
            }
        }

        if ( result == STRIDE_BASED_SD_SENSOR )
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
                createSBSDMResultString
                (
                    dataPage,
                    additionalData1,
                    additionalData2,
                    additionalData3,
                    additionalData4,
                    additionalData5,
                    additionalData6,
                    additionalData7,
                    additionalData8
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
amDeviceType antStrideSpeedDistProcessing::processSensor
(
    int                deviceType,
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_SBSDM_TYPE )
    {
        result = processStrideBasedSpeedAndDistanceSensor( deviceIDNo, timeStampBuffer, payLoad );
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

amDeviceType antStrideSpeedDistProcessing::processSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        if ( isStrideSpeedSensor( inputBuffer ) )
        {
            result = processStrideBasedSpeedAndDistanceSensorSemiCooked( inputBuffer );
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
bool antStrideSpeedDistProcessing::evaluateDeviceLine
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
        if ( ( deviceType == C_STRIDE_DEVICE_ID ) && isStrideSpeedSensor( deviceName ) )
        {   
            result = appendStrideSpeedDistSensor( deviceName );
        }   
    }   
    return result;
}

int antStrideSpeedDistProcessing::readDeviceFileStream
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
            else if ( ( deviceType == C_STRIDE_DEVICE_ID ) && isStrideSpeedSensor( deviceName ) )
            {
                evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
}

void antStrideSpeedDistProcessing::reset
(
    void
)
{
    antProcessing::reset();
    totalStrideCountTable.clear();
    totalDistTable.clear();
    eventDistTable.clear();
    strideCountTable.clear();
}

void antStrideSpeedDistProcessing::createSBSDMResultString
(
    unsigned int dataPage,
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3,
    unsigned int additionalData4,
    unsigned int additionalData5,
    unsigned int additionalData6,
    unsigned int additionalData7,
    unsigned int additionalData8
)
{
    if ( outputAsJSON )
    {
        appendJSONItem( "data page", dataPage );
    }

    if ( semiCookedOut )
    {
        if ( ( dataPage >= 1 ) && ( dataPage <= 15 ) )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "delta speed integer part", additionalData5 );
                appendJSONItem( "speed fractional part",    additionalData6 );
            }
            else
            {
                appendOutput( dataPage );
            }
            if ( dataPage == 1 )
            {
                if ( outputAsJSON )
                {
                    appendJSONItem( "delta time integer part",     additionalData1 );
                    appendJSONItem( "time fractional part",        additionalData2 );
                    appendJSONItem( "delta distance integer part", additionalData3 );
                    appendJSONItem( "distance fractional part",    additionalData4 );
                    appendJSONItem( "delta stride count",          additionalData7 );
                    appendJSONItem( "latency",                     additionalData8 );
                }
                else
                {
                    appendOutput( additionalData1 );
                    appendOutput( additionalData2 );
                    appendOutput( additionalData3 );
                    appendOutput( additionalData4 );
                    appendOutput( additionalData5 );
                    appendOutput( additionalData6 );
                    appendOutput( additionalData7 );
                    appendOutput( additionalData8 );
                }
            }
            else
            {
                if ( outputAsJSON )
                {
                    appendJSONItem( "cadence integer part",    additionalData3 );
                    appendJSONItem( "cadence fractional part", additionalData4 );
                    if ( dataPage == 3 )
                    {
                        appendJSONItem( "calories", additionalData7 );
                    }
                    appendJSONItem( "smd status", additionalData8 );
                }
                else
                {
                    appendOutput( additionalData3 );
                    appendOutput( additionalData4 );
                    appendOutput( additionalData5 );
                    appendOutput( additionalData6 );
                    if ( dataPage == 3 )
                    {
                        appendOutput( additionalData7 );
                    }
                    appendOutput( additionalData8 );
                }
            }
        }
        else if ( dataPage == 16 )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "strides since reset",  additionalData1 );
                appendJSONItem( "distance since reset", additionalData2 );
            }
            else
            {
                appendOutput( additionalData1 );
                appendOutput( additionalData2 );
            }
        }
        else if ( dataPage == 22 )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "capabilities",  additionalData1 );
            }
            else
            {
                appendOutput( additionalData1 );
            }
        }
    }
    else
    {
        if ( !outputAsJSON )
        {
            appendOutput( "DATA_PAGE" );
            appendOutput( dataPage );
        }
        if ( ( dataPage >= 1 ) && ( dataPage <= 15 ) )
        {
            double speed = ( double ) additionalData5 + ( ( double ) additionalData6 ) / 256.0;
            if ( outputAsJSON )
            {
                if ( ( additionalData5 == 0 ) && ( additionalData6 == 0 ) )
                {
                    appendJSONItem( "speed", "off" );
                }
                else
                {
                    appendJSONItem( "speed", speed, 8 );
                }
            }
            else
            {
                appendOutput( "SPEED" );
                if ( ( additionalData5 == 0 ) && ( additionalData6 == 0 ) )
                {
                    appendOutput( "OFF" );
                }
                else
                {
                    appendOutput( speed, 8 );
                }
            }

            if ( dataPage == 1 )
            {
                double totalTime     = ( double ) additionalData1 + ( ( double ) additionalData2 ) / 200.0;
                double totalDistance = ( double ) additionalData3 + ( ( double ) additionalData4 ) /  16.0;
                double latency       = ( double ) additionalData8 / 32.0;

                if ( outputAsJSON )
                {
                    if ( ( additionalData1 == 0 ) && ( additionalData2 == 0 ) )
                    {
                        appendJSONItem( "total time", "off" );
                    }
                    else
                    {
                        appendJSONItem( "total time", totalTime, 3 );
                    }

                    if ( ( additionalData3 == 0 ) && ( additionalData4 == 0 ) )
                    {
                        appendJSONItem( "total distance", "off" );
                    }
                    else
                    {
                        appendJSONItem( "total distance", totalDistance, 4 );
                    }

                    if ( additionalData8 == 0 )
                    {
                        appendJSONItem( "latency", "off" );
                    }
                    else
                    {
                        appendJSONItem( "latency", latency, 6 );
                    }
                }
                else
                {
                    appendOutput( "TOTAL_TIME" );
                    if ( ( additionalData1 == 0 ) && ( additionalData2 == 0 ) )
                    {
                        appendOutput( "OFF" );
                    }
                    else
                    {
                        appendOutput( totalTime, 3 );
                    }

                    appendOutput( "TOTAL_DIST" );
                    if ( ( additionalData3 == 0 ) && ( additionalData4 == 0 ) )
                    {
                        appendOutput( "OFF" );
                    }
                    else
                    {
                        appendOutput( totalDistance, 4 );
                    }

                    appendOutput( "LATENCY" );
                    if ( additionalData8 == 0 )
                    {
                        appendOutput( "OFF" );
                    }
                    else
                    {
                        appendOutput( latency, 6 );
                    }

                    appendOutput( "TOTAL_STRIDE_COUNT" );
                    appendOutput( additionalData7 );
                }
            }
            else
            {
                double cadence = ( double ) additionalData3 + ( ( double ) additionalData4 ) / 16.0;

                if ( outputAsJSON )
                {
                    if ( ( additionalData3 == 0 ) && ( additionalData4 == 0 ) )
                    {
                        appendJSONItem( "cadence", "off" );
                    }
                    else
                    {
                        appendJSONItem( "cadence", cadence, 3 );
                    }
                }
                else
                {
                    appendOutput( "CADENCE" );
                    if ( ( additionalData3 == 0 ) && ( additionalData4 == 0 ) )
                    {
                        appendOutput( "OFF" );
                    }
                    else
                    {
                        appendOutput( cadence, 4 );
                    }
                }

                if ( dataPage == 3 )
                {
                    if ( outputAsJSON )
                    {
                        if ( additionalData7 == 0 )
                        {
                            appendJSONItem( "calories", "off" );
                        }
                        else
                        {
                            appendJSONItem( "calories", additionalData7 );
                        }
                    }
                    else
                    {
                        appendOutput( "CALORIES" );
                        if ( ( additionalData1 == 0 ) && ( additionalData2 == 0 ) )
                        {
                            appendOutput( "OFF" );
                        }
                        else
                        {
                            appendOutput( additionalData7 );
                        }
                    }
                }

                if ( outputAsJSON )
                {
                    appendJSONItem( "smd location", ( ( additionalData8 & 0x03 ) == 0 ) ? "laces"   : (
                                                    ( ( additionalData8 & 0x03 ) == 1 ) ? "midsole" : (
                                                    ( ( additionalData8 & 0x03 ) == 2 ) ? "other"   : "ankle" ) ) );
                }
                else
                {
                    appendOutput( "DEVICE_LOCATION" );
                    appendOutput( ( ( additionalData8 & 0x03 ) == 0 ) ? "LACES"   : (
                                  ( ( additionalData8 & 0x03 ) == 1 ) ? "MIDSOLE" : (
                                  ( ( additionalData8 & 0x03 ) == 2 ) ? "OTHER"   : "ANKLE" ) ) );
                }

                if ( outputAsJSON )
                {
                    appendJSONItem( "battery status", ( ( additionalData8 & 0x0C ) == 0 ) ? "new"  : (
                                                      ( ( additionalData8 & 0x0C ) == 1 ) ? "good" : (
                                                      ( ( additionalData8 & 0x0C ) == 2 ) ? "ok"   : "low" ) ) );
                }
                else
                {
                    appendOutput( "BATTERY_STATUS" );
                    appendOutput( ( ( additionalData8 & 0x0C ) == 0 ) ? "NEW"  : (
                                  ( ( additionalData8 & 0x0C ) == 1 ) ? "GOOD" : (
                                  ( ( additionalData8 & 0x0C ) == 2 ) ? "OK"   : "LOW" ) ) );
                }

                switch ( additionalData8 & 0x30 )
                if ( outputAsJSON )
                {
                    appendJSONItem( "smd health", ( ( additionalData8 & 0x30 ) == 0 ) ? "ok"      : (
                                                  ( ( additionalData8 & 0x30 ) == 1 ) ? "error"   : (
                                                  ( ( additionalData8 & 0x30 ) == 2 ) ? "warning" : "reserved" ) ) );
                }
                else
                {
                    appendOutput( "DEVICE_HEALTH" );
                    appendOutput( ( ( additionalData8 & 0x30 ) == 0 ) ? "OK"      : (
                                  ( ( additionalData8 & 0x30 ) == 1 ) ? "ERROR"   : (
                                  ( ( additionalData8 & 0x30 ) == 2 ) ? "WARNING" : "RESERVED" ) ) );
                }

                switch ( additionalData8 & 0xC0 )
                if ( outputAsJSON )
                {
                    appendJSONItem( "use state", ( ( additionalData8 & 0xC0 ) == 0 ) ? "inactive" : (
                                                 ( ( additionalData8 & 0xC0 ) == 1 ) ? "active"   : "reserved" ) );
                }
                else
                {
                    appendOutput( "USE_STATE" );
                    appendOutput( ( ( additionalData8 & 0xC0 ) == 0 ) ? "INACTIVE" : (
                                  ( ( additionalData8 & 0xC0 ) == 1 ) ? "ACTIVE"   : "RESERVED" ) );
                }
            }
        }
        else if ( dataPage == 16 )
        {
            double totalDistance = ( double ) additionalData2 / 256.0;
            if ( outputAsJSON )
            {
                appendJSONItem( "strides since reset",  additionalData1 );
                appendJSONItem( "distance since reset", totalDistance, 8 );
            }
            else
            {
                appendOutput( additionalData1 );
                appendOutput( totalDistance, 8 );
            }
        }
        else if ( dataPage == 22 )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "time",     ( additionalData1 &  1 ) ? "valid" : "invalid" );
                appendJSONItem( "distance", ( additionalData1 &  2 ) ? "valid" : "invalid" );
                appendJSONItem( "speed",    ( additionalData1 &  4 ) ? "valid" : "invalid" );
                appendJSONItem( "latency",  ( additionalData1 &  8 ) ? "valid" : "invalid" );
                appendJSONItem( "cadence",  ( additionalData1 & 16 ) ? "valid" : "invalid" );
                appendJSONItem( "calories", ( additionalData1 & 32 ) ? "valid" : "invalid" );
            }
            else
            {
                appendOutput( "TIME" );
                appendOutput( ( additionalData1 &  1 ) ? "VALID" : "INVALID" );
                appendOutput( "DIST" );
                appendOutput( ( additionalData1 &  2 ) ? "VALID" : "INVALID" );
                appendOutput( "SPEED" );
                appendOutput( ( additionalData1 &  4 ) ? "VALID" : "INVALID" );
                appendOutput( "LATENCY" );
                appendOutput( ( additionalData1 &  8 ) ? "VALID" : "INVALID" );
                appendOutput( "CADENCE" );
                appendOutput( ( additionalData1 & 16 ) ? "VALID" : "INVALID" );
                appendOutput( "CALORIES" );
                appendOutput( ( additionalData1 & 32 ) ? "VALID" : "INVALID" );
            }
        }
    }
}

