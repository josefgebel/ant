// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_audio_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// AUDIO CONTROL ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antAudioProcessing::antAudioProcessing
(
    void
) : antProcessing()
{
    currentDeviceType = "AUDIO";
    reset();
}

void antAudioProcessing::reset
(
    void
)
{
    antProcessing::reset();
}

bool antAudioProcessing::isAudioSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_AUDIO_DEVICE_HEAD );
    return result;
}

bool antAudioProcessing::appendAudioSensor
(
    const amString &sensorID
)
{
    bool result = isAudioSensor( sensorID );
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
// AUDIO
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antAudioProcessing::processAudioControl
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result          = OTHER_DEVICE;
    amString     sensorID        = amString( C_AUDIO_DEVICE_HEAD ) + deviceIDNo;
    unsigned int dataPage        = 0;
    unsigned int additionalData1 = 0;
    unsigned int additionalData2 = 0;
    unsigned int additionalData3 = 0;
    unsigned int additionalData4 = 0;
    bool         commonPage      = false;
    bool         outputPageNo    = true;

    if ( isRegisteredDevice( sensorID ) )
    {
        dataPage = hex2Int( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        switch ( dataPage & 0x0F )
        {
            case  1: result          = AUDIO_CONTROL;
                     additionalData1 = hex2Int( payLoad[ 1 ] );                   // Volume
                     additionalData2 = hex2Int( payLoad[ 3 ], payLoad[ 2 ] );     // Total Track Time
                     additionalData3 = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );     // Current Track Time
                     additionalData4 = hex2Int( payLoad[ 7 ] );                   // State
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Volume",             payLoad[ 1 ],               additionalData1 );
                         appendDiagnosticsLine( "Total Track Time",   payLoad[ 3 ], payLoad[ 2 ], additionalData2 );
                         appendDiagnosticsLine( "Current Track Time", payLoad[ 5 ], payLoad[ 4 ], additionalData3 );
                         appendDiagnosticsLine( "State",              payLoad[ 7 ],               additionalData4 );
                     }
                     break;

            case 16: result          = AUDIO_CONTROL;
                     additionalData1 = hex2Int( payLoad[ 2 ], payLoad[ 1 ] );     // Serial Number
                     additionalData2 = hex2Int( payLoad[ 7 ] );                   // Command Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Serial Number",  payLoad[ 2 ], payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Command Number", payLoad[ 7 ],               additionalData2 );
                     }
                     break;

            default: result     = AUDIO_CONTROL;
                     commonPage = true;
                     break;
        }
    }

    if ( result == AUDIO_CONTROL )
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
            result = createAudioControlString( dataPage, additionalData1, additionalData2, additionalData3, additionalData4 );
        }
        appendOutputFooter( b2tVersion );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = deviceIDNo.toInt();
            createUnknownDeviceTypeString( C_AUDIO_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processAudioControlSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the resultBuffer.
// The output string has the form
// Depending on the value of <data_page> & 127:
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antAudioProcessing::processAudioControlSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType  result          = OTHER_DEVICE;
    amString      sensorID;
    amString      semiCookedString;
    amString      timeStampBuffer;
    amString      curVersion         = b2tVersion;
    amSplitString words;
    unsigned int  nbWords         = words.split( inputBuffer );
    unsigned int  counter         = 0;
    unsigned int  dataPage        = 0;
    unsigned int  additionalData1 = 0;
    unsigned int  additionalData2 = 0;
    unsigned int  additionalData3 = 0;
    unsigned int  additionalData4 = 0;
    unsigned int  startCounter    = 0;
    bool          commonPage      = false;
    bool          outputPageNo    = true;

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
        if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isAudioSensor( sensorID ) )
        {
            startCounter = counter;
            dataPage    = ( unsigned int ) words[ counter++ ].toInt();       //  3
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Data Page", dataPage );
            }
            counter = startCounter;
            switch ( dataPage & 0x0F )
            {
                case  1: if ( nbWords > 6 )
                         {
                             result          = AUDIO_CONTROL;
                             additionalData1 = words[ counter++ ].toUInt();       //  4 - Volume
                             additionalData2 = words[ counter++ ].toUInt();       //  5 - Total Track Time
                             additionalData3 = words[ counter++ ].toUInt();       //  6 - Current Track Time
                             additionalData4 = words[ counter++ ].toUInt();       //  7 - State
                             if ( diagnostics )
                             {
                                 appendDiagnosticsLine( "Volume",             additionalData1 );
                                 appendDiagnosticsLine( "Total Track Time",   additionalData2 );
                                 appendDiagnosticsLine( "Current Track Time", additionalData3 );
                                 appendDiagnosticsLine( "State",              additionalData4 );
                            }
                         }
                         break;

                case 16: result          = AUDIO_CONTROL;
                         additionalData1 = words[ counter++ ].toUInt();           // 4 - Serial Number
                         additionalData2 = words[ counter++ ].toUInt();           // 5 - Command Number
                         if ( diagnostics )
                         {
                             appendDiagnosticsLine( "Serial Number",  additionalData1 );
                             appendDiagnosticsLine( "Command Number", additionalData2 );
                         }
                         break;

                default: commonPage = true;
                         result     = AUDIO_CONTROL;
                         break;
            }
        }
    }

    if ( result == AUDIO_CONTROL )
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
            createAudioControlString( dataPage, additionalData1, additionalData2, additionalData3, additionalData4 );
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
amDeviceType antAudioProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_AUDIO_TYPE )
    {
        result = processAudioControl( deviceIDNo, timeStampBuffer, payLoad );
    }
    else if ( outputUnknown )
    {
        int deviceIDNoAsInt = deviceIDNo.toInt();
        createUnknownDeviceTypeString( deviceType, deviceIDNoAsInt, timeStampBuffer, payLoad );
    }
    else
    {
        resetOutBuffer();
    }

    return result;
}

amDeviceType antAudioProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( !inputBuffer.empty() )
    {
        if ( isAudioSensor( inputBuffer ) )
        {
            result = processAudioControlSemiCooked( inputBuffer );
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
bool antAudioProcessing::evaluateDeviceLine
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
        if ( ( deviceType == C_AUDIO_DEVICE_ID ) && isAudioSensor( deviceName ) )
        {
            result = appendAudioSensor( deviceName );
        }
    }
    return result;
}

int antAudioProcessing::readDeviceFileStream
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
            else if ( ( deviceType == C_AUDIO_DEVICE_ID ) && isAudioSensor( deviceName ) )
            {
                evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
}

amDeviceType antAudioProcessing::createAudioControlString
(
    unsigned int dataPage,
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3,
    unsigned int additionalData4
)
{
    amDeviceType result = AUDIO_CONTROL;

    if ( outputAsJSON )
    {
        appendJSONItem( "data page", dataPage );
    }

    if ( semiCookedOut )
    {
        if ( dataPage == 1 )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "volume",             additionalData1 );
                appendJSONItem( "total track time",   additionalData2 );
                appendJSONItem( "current track time", additionalData3 );
                appendJSONItem( "status",             additionalData4 );
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
        else if ( dataPage == 16 )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "serial number",  additionalData1 );
                appendJSONItem( "command number", additionalData2 );
            }
            else
            {
                appendOutput( dataPage );
                appendOutput( additionalData1 );
                appendOutput( additionalData2 );
            }
        }
    }
    else
    {
        if ( dataPage == 1 )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "volume",             additionalData1 );
                appendJSONItem( "total track time",   additionalData2 );
                appendJSONItem( "current track time", additionalData3 );

                int audioDeviceState = additionalData3 >> 4;
                switch ( audioDeviceState )
                {
                    case  0: appendJSONItem( "status", "off" );
                             break;
                    case  1: appendJSONItem( "status", "play" );
                             break;
                    case  2: appendJSONItem( "status", "paused" );
                             break;
                    case  3: appendJSONItem( "status", "stopped" );
                             break;
                    case  4: appendJSONItem( "status", "busy" );
                             break;
                    case 15: appendJSONItem( "status", C_UNKNOWN_JSON );
                             break;
                    default: appendJSONItem( "status", "reserved" );
                             break;
                }

                if ( audioDeviceState != 15 )
                {
                    int repeatState = additionalData3 & 0x0C;
                    switch ( repeatState )
                    {
                        case  0: appendJSONItem( "repeat", "off" );
                                 break;
                        case  1: appendJSONItem( "repeat", "current track" );
                                 break;
                        case  2: appendJSONItem( "repeat", "all" );
                                 break;
                        default: appendJSONItem( "repeat", "reserved" );
                                 break;
                    }

                    int shuffleState = additionalData3 & 0x03;
                    switch ( shuffleState )
                    {
                        case  0: appendJSONItem( "shuffle", "off" );
                                 break;
                        case  1: appendJSONItem( "shuffle", "track level" );
                                 break;
                        case  2: appendJSONItem( "shuffle", "album level" );
                                 break;
                        default: appendJSONItem( "shuffle", "reserved" );
                                 break;
                    }
                }
            }
            else
            {
                appendOutput( "VOLUME" );
                if ( additionalData1 == 0xFF )
                {
                    appendOutput( C_UNKNOWN );
                }
                else
                {
                    appendOutput( additionalData1, "\%" );
                }

                appendOutput( "TOTAL_TRACK_TIME" );
                if ( additionalData2 == 0xFFFF )
                {
                    appendOutput( C_UNKNOWN );
                }
                else
                {
                    appendOutput( additionalData2, "s" );
                }

                appendOutput( "CURR_TRACK_TIME" );
                if ( additionalData3 == 0xFFFF )
                {
                    appendOutput( C_UNKNOWN );
                }
                else
                {
                    appendOutput( additionalData3 );
                }

                int audioDeviceState = additionalData3 >> 4;
                appendOutput( "STATE" );
                switch ( audioDeviceState )
                {
                    case  0: appendOutput( "OFF" );
                             break;
                    case  1: appendOutput( "PLAY" );
                             break;
                    case  2: appendOutput( "PAUSED" );
                             break;
                    case  3: appendOutput( "STOPPED" );
                             break;
                    case  4: appendOutput( "BUSY" );
                             break;
                    case 15: appendOutput( C_UNKNOWN );
                             break;
                    default: appendOutput( "RESERVED" );
                             break;
                }

                if ( audioDeviceState != 15 )
                {
                    int repeatState = additionalData3 & 0x0C;
                    appendOutput( "REPEAT" );
                    switch ( repeatState )
                    {
                        case  0: appendOutput( "OFF" );
                                 break;
                        case  1: appendOutput( "CURRENT_TRACK" );
                                 break;
                        case  2: appendOutput( "ALL" );
                                 break;
                        default: appendOutput( "RESERVED" );
                                 break;
                    }

                    int shuffleState = additionalData3 & 0x03;
                    appendOutput( "SHUFFLE" );
                    switch ( shuffleState )
                    {
                        case  0: appendOutput( "OFF" );
                                 break;
                        case  1: appendOutput( "TRACK" );
                                 break;
                        case  2: appendOutput( "ALBUM" );
                                 break;
                        default: appendOutput( "RESERVED" );
                                 break;
                    }
                }
            }
        }
        else if ( dataPage == 16 )
        {
            if ( outputAsJSON )
            {
                if (  additionalData1 == 0xFFFF )
                {
                    appendJSONItem( "serial number", C_UNKNOWN_JSON );
                }
                else
                {
                    appendJSONItem( "serial number", additionalData1 );
                }
                appendJSONItem( "command number", additionalData2 );

                switch ( additionalData2 )
                {
                    case  1: appendJSONItem( "command", "off" );
                             break;
                    case  2: appendJSONItem( "command", "pause" );
                             break;
                    case  3: appendJSONItem( "command", "stop" );
                             break;
                    case  4: appendJSONItem( "command", "volume up" );
                             break;
                    case  5: appendJSONItem( "command", "volume down" );
                             break;
                    case  6: appendJSONItem( "command", "mute" );
                             break;
                    case  7: appendJSONItem( "command", "track ahead" );
                             break;
                    case  8: appendJSONItem( "command", "track back" );
                             break;
                    case  9: appendJSONItem( "command", "repeat current track" );
                             break;
                    case 10: appendJSONItem( "command", "repeat all" );
                             break;
                    case 11: appendJSONItem( "command", "repeat off" );
                             break;
                    case 12: appendJSONItem( "command", "shuffle songs" );
                             break;
                    case 13: appendJSONItem( "command", "shuffle albums" );
                             break;
                    case 14: appendJSONItem( "command", "shuffle off" );
                             break;
                    case 15: appendJSONItem( "command", "fast forward through track" );
                             break;
                    case 16: appendJSONItem( "command", "fast rewind through track" );
                             break;
                    default: appendJSONItem( "command", "reserved" );
                             break;
                }
            }
            else
            {
                appendOutput( "SERIAL_NO" );
                if (  additionalData1 == 0xFFFF )
                {
                    appendOutput( C_UNKNOWN );
                }
                else
                {
                    appendOutput( additionalData1 );
                }
                appendOutput( "COMMAND" );
                switch ( additionalData2 )
                {
                    case  1: appendOutput( "OFF" );
                             break;
                    case  2: appendOutput( "PAUSE" );
                             break;
                    case  3: appendOutput( "STOP" );
                             break;
                    case  4: appendOutput( "VOLUME_UP" );
                             break;
                    case  5: appendOutput( "VOLUME_DOWN" );
                             break;
                    case  6: appendOutput( "MUTE" );
                             break;
                    case  7: appendOutput( "TRACK_AHEAD" );
                             break;
                    case  8: appendOutput( "TRACK_BACK" );
                             break;
                    case  9: appendOutput( "REPEAT_CURRENT_TRACK" );
                             break;
                    case 10: appendOutput( "REPEAT_ALL" );
                             break;
                    case 11: appendOutput( "REPEAT_OFF" );
                             break;
                    case 12: appendOutput( "SHUFFLE_SONGS" );
                             break;
                    case 13: appendOutput( "SHUFFLE_ALBUMS" );
                             break;
                    case 14: appendOutput( "SHUFFLE_OFF" );
                             break;
                    case 15: appendOutput( "FFD" );
                             break;
                    case 16: appendOutput( "REW" );
                             break;
                    default: appendOutput( "RESERVED" );
                             break;
                }
            }
        }
    }

    return result;
}

