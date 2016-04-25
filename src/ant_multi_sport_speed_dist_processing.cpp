// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_multi_sport_speed_dist_processing.h"

const double C_SSU_2_DEG = 1.34110450744628906250E-6;


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// MULTI SPORT SPEED and DISTANCE ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antMultiSportProcessing::antMultiSportProcessing
(
    void
) : antProcessing()
{
    currentDeviceType = "MULTI";
    reset();
}

bool antMultiSportProcessing::isMultiSportSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_MSSDM_DEVICE_HEAD );
    return result;
}

bool antMultiSportProcessing::appendMultiSportSensor
(
    const amString &sensorID
)
{
    bool result = isMultiSportSensor( sensorID );
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
// MSSDPD: Multi-Sport Speed and Distance Sensor
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antMultiSportProcessing::processMultiSportSpeedAndDistanceSensor
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result                = OTHER_DEVICE;
    amString     sensorID              = amString( C_MSSDM_DEVICE_HEAD ) + deviceIDNo;
    unsigned int dataPage              = 0;
    unsigned int auxInt1               = 0;
    unsigned int auxInt2               = 0;
    unsigned int additionalData1       = 0;
    unsigned int additionalData2       = 0;
    unsigned int additionalData3       = 0;
    double       additionalDoubleData1 = 0;
    double       additionalDoubleData2 = 0;
    unsigned int rollOver              = 0;
    bool         rollOverHappened      = false;
    bool         commonPage            = false;
    bool         outputPageNo          = true;

    if ( isRegisteredDevice( sensorID ) )
    {
        dataPage = hex2Int( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        switch ( dataPage & 0x0F )
        {
            case  1: result          = MULTI_SPORT_SD_SENSOR;

                     auxInt1         = hex2Int( payLoad[ 3 ], payLoad[ 2 ] );    // Time Stamp
                     rollOver        = 65536;  // 256^2
                     additionalData1 = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, auxInt1 );
                     if ( !semiCookedOut )
                     {
                         additionalDoubleData1      = totalTimeTable[ sensorID ] + ( double ) ( additionalData1 ) / 1024.0;
                         totalTimeTable[ sensorID ] = additionalDoubleData1;
                     }
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Time Stamp", payLoad[ 3 ], payLoad[ 2 ], auxInt1 );
                         appendDiagnosticsLine( "Delta Time Stamp", additionalData1 );
                         if ( !semiCookedOut )
                         {
                             appendDiagnosticsLine( "Total Time Stamp", additionalDoubleData1 );
                         }
                     }

                     auxInt2         = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );    // Distance
                     rollOver        = 65536;  // 256^2
                     additionalData2 = getDeltaInt( rollOverHappened, sensorID, rollOver, eventDistTable, auxInt2 );
                     if ( !semiCookedOut )
                     {
                         additionalDoubleData2      = totalDistTable[ sensorID ] + ( double ) ( additionalData2 ) / 10.0;
                         totalTimeTable[ sensorID ] = additionalDoubleData2;
                     }
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Distance Stamp", payLoad[ 5 ], payLoad[ 4 ], auxInt2 );
                         appendDiagnosticsLine( "Delta Distance Stamp", additionalData2 );
                         if ( !semiCookedOut )
                         {
                             appendDiagnosticsLine( "Total Distance Stamp", additionalDoubleData2 );
                         }
                     }

                     additionalData3 = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );    // Instantaneous Speed
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Instantaneous Speed", payLoad[ 7 ], payLoad[ 6 ], additionalData3 );
                     }
                     break;

            case  2: result          = MULTI_SPORT_SD_SENSOR;
                     auxInt1         = hex2Int( payLoad[ 4 ], payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ] );          // Latitude
                     auxInt2         = hex2Int( payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ] );          // Longitude
                     additionalData1 = ( auxInt1 << 4 ) >> 4;
                     additionalData2 = auxInt2 >> 4;
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Latitude", payLoad[ 4 ], payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ],
                                                additionalData1, " (Byte 1: first 4 bits)" );
                         appendDiagnosticsLine( "Longitude", payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ],
                                                additionalData2, " (Byte 4: last 4 bits)" );
                     }
                     break;

            case  3: result          = MULTI_SPORT_SD_SENSOR;
                     auxInt1         = hex2Int( payLoad[ 4 ] );
                     additionalData1 = ( auxInt1 << 4 ) >> 4;                      // Fix Type
                     auxInt2         = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );
                     additionalData2 = auxInt2 >> 4;                               // Heading
                     additionalData3 = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );          // Elevation
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Fix Type",  payLoad[ 4 ],               additionalData1, " (bits 7-4)" );
                         appendDiagnosticsLine( "Heading",   payLoad[ 5 ], payLoad[ 4 ], additionalData2, " Second Byte: (bits 3-0)" );
                         appendDiagnosticsLine( "Elevation", payLoad[ 7 ], payLoad[ 6 ], additionalData3 );
                     }
                     break;

            case 48: result          = MULTI_SPORT_SD_SENSOR;
                     additionalData1 = hex2Int( payLoad[ 5 ] );                        // Mode
                     additionalData2 = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );          // Scale Factor
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Mode",         payLoad[ 5 ],               additionalData1 );
                         appendDiagnosticsLine( "Scale Factor", payLoad[ 7 ], payLoad[ 6 ], additionalData2 );
                     }
                     break;

            default: commonPage = true;
                     result     = MULTI_SPORT_SD_SENSOR;
                     break;
        }
    }

    if ( result == MULTI_SPORT_SD_SENSOR )
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
            createMSSDMResultString ( dataPage, additionalData1, additionalData2, additionalData3, additionalDoubleData1, additionalDoubleData2 );
        }
        appendOutputFooter( b2tVersion );
    }

    return result;
}

amDeviceType antMultiSportProcessing::processMultiSportSpeedAndDistanceSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amString      sensorID;
        amString      semiCookedString;
        amString      timeStampBuffer;
        amString      curVersion            = b2tVersion;
        amSplitString words;
        unsigned int  nbWords               = words.split( inputBuffer );
        unsigned int  counter               = 0;
        unsigned int  startCounter          = 0;
        unsigned int  dataPage              = 0;
        unsigned int  additionalData1       = 0;
        unsigned int  additionalData2       = 0;
        unsigned int  additionalData3       = 0;
        double        additionalDoubleData1 = 0;
        double        additionalDoubleData2 = 0;
        bool          commonPage            = false;
        bool          outputPageNo          = true;

        if ( nbWords > 5 )
        {
            sensorID         = words[ counter++ ];                                                  //  0 - sensor ID
            timeStampBuffer  = words[ counter++ ];                                                  //  1 - time stamp
            semiCookedString = words[ counter++ ];                                                  //  2 - semi-cooked ID
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isMultiSportSensor( sensorID ) )
            {
                startCounter = counter;
                dataPage     = words[ counter++ ].toUInt();                                   //  3 - data page
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Data Page", dataPage );
                }

                switch ( dataPage & 0x0F )
                {
                    case  1: if ( nbWords > 6 )
                             {
                                 result         = MULTI_SPORT_SD_SENSOR;
                                 additionalData1            = words[ counter++ ].toUInt();                        //  4 - deltaTimeEvent
                                 additionalDoubleData1      = totalTimeTable[ sensorID ] + ( double ) additionalData1 / 1024.0;
                                 totalTimeTable[ sensorID ] = additionalDoubleData1;                              // Total Event Time

                                 additionalData2 = words[ counter++ ].toUInt();                                   //  5 - deltaDistEvent
                                 additionalDoubleData2      = totalDistTable[ sensorID ] + ( double ) additionalData2 / 1024.0;
                                 totalDistTable[ sensorID ] = additionalDoubleData2;                 // Total Event Dist

                                 additionalData3 = words[ counter++ ].toUInt();                                   //  6 - event speed
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Delta Event Time", additionalData1 );
                                     appendDiagnosticsLine( "Event Time", additionalDoubleData1 );
                                     appendDiagnosticsLine( "Delta Event Dist", additionalData2 );
                                     appendDiagnosticsLine( "Event Dist", additionalDoubleData2 );
                                     appendDiagnosticsLine( "Event Speed", additionalData3 );
                                 }
                             }
                             break;
                    case  2: if ( nbWords > 5 )
                             {
                                 result          = MULTI_SPORT_SD_SENSOR;
                                 additionalData1 = words[ counter++ ].toUInt();                                   //  4 - latitude
                                 additionalData2 = words[ counter++ ].toUInt();                                   //  5 - longitude
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "latitudeSSU",  additionalData1 );
                                     appendDiagnosticsLine( "longitudeSSU", additionalData2 );
                                 }
                             }
                             break;
                    case  3: if ( nbWords > 6 )
                             {
                                 result          = MULTI_SPORT_SD_SENSOR;
                                 additionalData1 = words[ counter++ ].toUInt();                                   //  4 - fix type
                                 additionalData2 = words[ counter++ ].toUInt();                                   //  5 - heading
                                 additionalData3 = words[ counter++ ].toUInt();                                   //  6 - elevation
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Fix Type",        additionalData1 );
                                     appendDiagnosticsLine( "Heading (int)",   additionalData2 );
                                     appendDiagnosticsLine( "Elevation (int)", additionalData3 );
                                 }
                             }
                             break;
                    case 48: if ( nbWords > 5 )
                             {
                                 result         = MULTI_SPORT_SD_SENSOR;
                                 additionalData1 = words[ counter++ ].toUInt();                                   //  4 - mode
                                 additionalData2 = words[ counter++ ].toUInt();                                   //  5 - scale factoe
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Mode",               additionalData1 );
                                     appendDiagnosticsLine( "Scale Factor (int)", additionalData2 );
                                 }
                             }
                             break;
                    default: commonPage = true;
                             counter    = startCounter;
                             result     = MULTI_SPORT_SD_SENSOR;
                }
            }
        }

        if ( result == MULTI_SPORT_SD_SENSOR )
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
                createMSSDMResultString( dataPage, additionalData1, additionalData2, additionalData3, additionalDoubleData1, additionalDoubleData2 );
            }
            appendOutputFooter( curVersion );
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
amDeviceType antMultiSportProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_MSSDM_TYPE )
    {
        result = processMultiSportSpeedAndDistanceSensor( deviceIDNo, timeStampBuffer, payLoad );
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

amDeviceType antMultiSportProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        if ( isMultiSportSensor( inputBuffer ) )
        {
            result = processMultiSportSpeedAndDistanceSensorSemiCooked( inputBuffer );
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
bool antMultiSportProcessing::evaluateDeviceLine
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
        if ( ( deviceType == C_MULTI_SPORT_DEVICE_ID ) && isMultiSportSensor( deviceName ) )
        {
            result = appendMultiSportSensor( deviceName );
        }
    }
    return result;
}

int antMultiSportProcessing::readDeviceFileStream
(
    std::ifstream &deviceFileStream
)
{
    char line[ C_BUFFER_SIZE ];
    amSplitString words;

    amString     deviceType = "";
    amString     deviceName = "";
    unsigned int nbWords    = 0;

    while ( true )
    {
        deviceFileStream.getline( line, C_BUFFER_SIZE, '\n' );
        if ( deviceFileStream.fail() || deviceFileStream.eof() )
        {
            break;
        }
        const char *lPtr = line;
        while ( IS_WHITE_CHAR( *lPtr ) )
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
                    errorMessage += "ERROR while opening devices ID include file \"";
                    errorMessage += includeFileName;
                    errorMessage += "\".\n";
                    errorCode     = E_READ_FILE_NOT_OPEN;
                }
                else
                {
                    errorCode = readDeviceFileStream( devicesIncludeFileStream );
                    devicesIncludeFileStream.close();
                }
            }
            else if ( ( deviceType == C_MULTI_SPORT_DEVICE_ID ) && isMultiSportSensor( deviceName ) )
            {
                evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
}

void antMultiSportProcessing::reset
(
    void
)
{
    antProcessing::reset();
    totalDistTable.clear();
    eventDistTable.clear();
}

void antMultiSportProcessing::createMSSDMResultString
(
    unsigned int dataPage,
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3,
    double       additionalDoubleData1,
    double       additionalDoubleData2
)
{
    char auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };

    if ( outputAsJSON )
    {
        appendJSONItem( "data page", dataPage );
    }
    else
    {
        appendOutput( dataPage );
    }
    if ( semiCookedOut )
    {
        if ( ( dataPage == 1 ) || ( dataPage == 3 ) )
        {
            if ( outputAsJSON )
            {
                if ( dataPage == 1 )
                {
                    appendJSONItem( "delta event time",     additionalData1 );
                    appendJSONItem( "delta event distance", additionalData2 );
                    appendJSONItem( "event speed",          additionalData3 );
                }
                else if ( dataPage == 3 )
                {
                    appendJSONItem( "fix type",  additionalData1 );
                    appendJSONItem( "heading",   additionalData2 );
                    appendJSONItem( "elevation", additionalData3 );
                }
            }
            else
            {
                appendOutput( additionalData1 );
                appendOutput( additionalData2 );
                appendOutput( additionalData3 );
            }
        }
        if ( ( dataPage == 2 ) || ( dataPage == 48 ) )
        {
            if ( outputAsJSON )
            {
                if ( dataPage == 2 )
                {
                    appendJSONItem( "latitude",  additionalData1 );
                    appendJSONItem( "longitude", additionalData2 );
                }
                else if ( dataPage == 48 )
                {
                    appendJSONItem( "mode",         additionalData1 );
                    appendJSONItem( "scale factor", additionalData2 );
                }
            }
            else
            {
                appendOutput( additionalData1 );
                appendOutput( additionalData2 );
            }
        }
    }
    else
    {
        if ( dataPage == 1 )
        {
            double speed = ( double ) additionalData3 / 1000.0;
            if ( outputAsJSON )
            {
                appendJSONItem( "time",     additionalDoubleData1, 3 );
                appendJSONItem( "distance", additionalDoubleData2, 1 );
                appendJSONItem( "speed",    speed,                 3 );
            }
            else
            {
                appendOutput( "T" );
                appendOutput( additionalDoubleData1, 3, "s" );
                appendOutput( "D" );
                appendOutput( additionalDoubleData2, 1, "m" );
                appendOutput( "V" );
                appendOutput( speed, 3, "km/h" );
            }
        }
        else if ( dataPage == 2 )
        {
            int    latitudeSigned  = NEGATE_BINARY_INT( additionalData1, 28 );
            int    longitudeSigned = NEGATE_BINARY_INT( additionalData2, 28 );
            double latitude  = ( double ) latitudeSigned * C_SSU_2_DEG;
            double longitude = ( double ) longitudeSigned * C_SSU_2_DEG;
            if ( outputAsJSON )
            {
                appendJSONItem( "latitude",  latitude, getValuePrecision() );
                appendJSONItem( "longitude", longitude, getValuePrecision() );
            }
            else
            {
                appendOutput( "LAT" );
                appendOutput( latitude, 3, "deg" );
                appendOutput( "LON" );
                appendOutput( longitude, 3, "deg" );
            }
        }
        else if ( dataPage == 3 )
        {
            double heading   = convertTemp10( additionalData2 );
            double elevation = ( double ) additionalData3 / 5.0 - 500.0;

            switch ( additionalData1 )
            {
                case  0: strcpy( auxBuffer, outputAsJSON ? "no fix" : "NO_FIX" );
                         break;
                case  1: strcpy( auxBuffer, outputAsJSON ? "searching" : "SEARCHING" );
                         break;
                case  2: strcpy( auxBuffer, outputAsJSON ? "propagating" : "PROPAGATING" );
                         break;
                case  3: strcpy( auxBuffer, outputAsJSON ? "last known position" : "LAST_KNOWN_POS" );
                         break;
                case  4: strcpy( auxBuffer, outputAsJSON ? "2d" : "2D" );
                         break;
                case  5: strcpy( auxBuffer, outputAsJSON ? "2d waas" : "2D_WAAS" );
                         break;
                case  6: strcpy( auxBuffer, outputAsJSON ? "2d differential" : "2D_DIFFERENTIAL" );
                         break;
                case  7: strcpy( auxBuffer, outputAsJSON ? "3d" : "3D" );
                         break;
                case  8: strcpy( auxBuffer, outputAsJSON ? "3d waas" : "3D_WAAS" );
                         break;
                case  9: strcpy( auxBuffer, outputAsJSON ? "3d differential" : "3D_DIFFERENTIAL" );
                         break;
                case 15: strcpy( auxBuffer, outputAsJSON ? C_INVALID_JSON : C_INVALID );
                         break;
                default: strcpy( auxBuffer, outputAsJSON ? C_UNUSED_JSON : C_UNUSED );
                         break;
            }
            if ( outputAsJSON )
            {
                appendJSONItem( "fix type",  auxBuffer );
                appendJSONItem( "heading",   heading,   2 );
                appendJSONItem( "elevation", elevation, 1 );
            }
            else
            {
                appendOutput( "FIX_TYPE" );
                appendOutput( auxBuffer );
                appendOutput( "HEADING" );
                appendOutput( heading, 2 );
                appendOutput( "ELEV" );
                appendOutput( elevation, 1 );
            }
        }
        else if ( dataPage == 48 )
        {
            double scaleFactor = additionalData2 / 10000.0;
            if ( outputAsJSON )
            {
                if ( additionalData1 == 0xFF )
                {
                    appendJSONItem( "mode", C_INVALID_JSON );
                }
                else
                {
                    appendJSONItem( "mode", additionalData1 );
                }

                if ( additionalData2 == 0xFFFF )
                {
                    appendJSONItem( "calibration request", true );
                }
                else
                {
                    appendJSONItem( "scale factor", scaleFactor, 5 );
                }
            }
            else
            {
                appendOutput( "MODE" );
                if ( additionalData1 == 0xFF )
                {
                    appendOutput( C_INVALID );
                }
                else
                {
                    appendOutput( additionalData1 );
                }

                if ( additionalData2 == 0xFFFF )
                {
                    appendOutput( "CALIBRATION_REQUEST" );
                }
                else
                {
                    appendOutput( "SCALE_FACTOR" );
                    appendOutput( scaleFactor, 5 );
                }
            }
        }
    }
}

double antMultiSportProcessing::convertTemp10
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

