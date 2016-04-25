// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_all_processing.h"

// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antAllProcessing::antAllProcessing
(
    void
) : antAeroProcessing(),
    antMultiSportProcessing(),
    antPowerProcessing(),
    antStrideSpeedDistProcessing(),
    antAudioProcessing(),
    antWeightProcessing(),
    antBloodPressureProcessing(),
    antEnvironmentProcessing(),
    antSpcadProcessing(),
    antCadenceOnlyProcessing(),
    antHRMProcessing(),
    antSpeedOnlyProcessing()
{
    currentDeviceType = "ALL";
    reset();
}

void antAllProcessing::reset
(
    void
)
{
    antAeroProcessing::reset();
    antMultiSportProcessing::reset();
    antPowerProcessing::reset();
    antStrideSpeedDistProcessing::reset();
    antAudioProcessing::reset();
    antWeightProcessing::reset();
    antBloodPressureProcessing::reset();
    antEnvironmentProcessing::reset();
    antSpcadProcessing::reset();
    antCadenceOnlyProcessing::reset();
    antHRMProcessing::reset();
    antSpeedOnlyProcessing::reset();
}

//---------------------------------------------------------------------------------------------------

int antAllProcessing::readDeviceFileStream
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
            else if ( deviceType == C_SPEED_DEVICE_ID )
            {
                if ( isSpeedOnlySensor( deviceName ) || isSpeedAndCadenceSensor( deviceName ) )
                {
                    antSpeedProcessing::evaluateDeviceLine( words );
                }
                else if ( isCadenceSensor( deviceName ) )
                {
                    antCadenceSpeedProcessing::evaluateDeviceLine( words );
                }
            }
            else if ( deviceType == C_POWER_DEVICE_ID )
            {
                 antPowerProcessing::evaluateDeviceLine( words );
            }
            else if ( deviceType == C_HEART_RATE_DEVICE_ID )
            {
                antHRMProcessing::evaluateDeviceLine( words );
            }
            else if ( deviceType == C_CADENCE_DEVICE_ID )
            {
                antCadenceProcessing::evaluateDeviceLine( words );
            }
            else if ( ( deviceType == C_AERO_DEVICE_ID ) || ( deviceType == C_RHO_ID ) )
            {
                // Aero Sensor or Current Air Density
                antAeroProcessing::evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
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
amDeviceType antAllProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    switch ( deviceType )
    {
        case C_AERO_TYPE  : result = antAeroProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_AUDIO_TYPE : result = antAudioProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_BLDPR_TYPE : result = antBloodPressureProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_CAD_TYPE   : result = antCadenceOnlyProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_ENV_TYPE   : result = antEnvironmentProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_HRM_TYPE   : result = antHRMProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_MSSDM_TYPE : result = antMultiSportProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_POWER_TYPE : result = antPowerProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_SBSDM_TYPE : result = antStrideSpeedDistProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_SPCAD_TYPE : result = antSpcadProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_SPEED_TYPE : result = antSpeedOnlyProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        case C_WEIGHT_TYPE: result = antWeightProcessing::processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
                            break;
        default           : result = OTHER_DEVICE;
                            if ( outputUnknown )
                            {
                                int deviceIDNoAsInt = deviceIDNo.toInt();
                                createUnknownDeviceTypeString( deviceType, deviceIDNoAsInt, timeStampBuffer, payLoad );
                            }
                            else
                            {
                                resetOutBuffer();
                            }
                            break;
    }

    return result;
}

amDeviceType antAllProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( !inputBuffer.empty() )
    {
        switch ( inputBuffer[ 0 ] )
        {
            case 'A': // Aero Sensor  or  Audio Control
                      if ( isAeroSensor( inputBuffer ) )
                      {
                          result = antAeroProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      else if ( isAudioSensor( inputBuffer ) )
                      {
                          result = antAudioProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'B': // Blood Pressure Sensor or Bridge
                      if ( isBloodPressureSensor( inputBuffer ) )
                      {
                          result = antBloodPressureProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'C': // Cadence-Only Sensor
                      if ( isCadenceOnlySensor( inputBuffer ) )
                      {
                          result = antCadenceOnlyProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'E': // Environmental Sensor
                      if ( isEnvironmentSensor( inputBuffer ) )
                      {
                          result = antEnvironmentProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'H': // Heart Rate Sensor
                      if ( isHeartRateSensor( inputBuffer ) )
                      {
                          result = antHRMProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'M': // Multi-Sports Speed and Distance Sensor
                      if ( isMultiSportSensor( inputBuffer ) )
                      {
                          result = antMultiSportProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'P': // Power Meter or Power Meter Related Message
                      if ( isPowerMeterRelated( inputBuffer ) )
                      {
                          result = antPowerProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'S': // Speed-and-Candence Sensor  or  Speed-Only Sensor  or  Stride-Based Speed-and-Distance Sensor
                      if ( isSpeedAndCadenceSensor( inputBuffer ) )
                      {
                          result = antSpcadProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      else if ( isSpeedOnlySensor( inputBuffer ) )
                      {
                          result = antSpeedOnlyProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      else if ( isStrideSpeedSensor( inputBuffer ) )
                      {
                          result = antStrideSpeedDistProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            case 'T': // Previously unrecognized Sensor Type
                      if ( inputBuffer.startsWith( C_UNKNOWN_TYPE_HEAD ) )
                      {
                          result = processUndefinedSensorType( inputBuffer );
                      }
                      break;
            case 'W': // Weight Scale
                      if ( isWeightScaleSensor( inputBuffer ) )
                      {
                          result = antWeightProcessing::processSensorSemiCooked( inputBuffer );
                      }
                      break;
            default : // Unrecognized Sensor Type
                      break;
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

// ------------------------------------------------------------------------------------------------------
//
// processUndefinedSensorType
//
// Description:
//     This function checks for "TYPE" messages, i.e. ANT+ messages that could not be
//     processed by a previous version of 'bridge2txt' and were output as
//         TYPE<type>   <time_stamp>   <pay_load>   <version>
//     where
//         <type>       is the device type from the original ANT+ message
//                      Currently supported types
//                           11: power meter
//                          120: heart rate monitor
//                          121: speed only sensor
//                          122: cadence only sensor
//                          123: speed and cadence sensor
//         <time_stamp> is the time stamp when the original ANT+ message was processed
//         <pay_load>   are the 8 bytes of the pay load from the original ANT+ message
//                      written as text aand separated by a space, e.g. "46 FF FF 01 FF 02 02 01"
//         <version>    is bridge2txt version string when the original ANT+ message was processed
//
// ------------------------------------------------------------------------------------------------------
amDeviceType antAllProcessing::processUndefinedSensorType
(
    const amString &inputBuffer
)
{
    amSplitString words;
    amSplitString sensorParts;
    amDeviceType  result           = OTHER_DEVICE;
    amString      deviceIDNo       = "";
    amString      deviceTypeString = "";
    amString      sensorID         = "";
    amString      timeStampBuffer  = "";
    unsigned int  nbWords          = words.split( inputBuffer );
    unsigned int  nbSensorParts    = 0;
    unsigned int  deviceType       = 0;
    unsigned int  counter          = 0;
    unsigned int  syntaxError      = 0;

    syntaxError = ( nbWords >= 2 + C_ANT_PAYLOAD_LENGTH ) ? 0 : 1;
    if ( syntaxError == 0 )
    {
        // Example: sensorID = "TYPE11_98765"
        //          Extract deviceType = 11
        //          and     deviceIDNo = "98765"
        sensorID = words[ 0 ];
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID", sensorID );
        }
        sensorParts.removeAllSeparators();
        sensorParts.addSeparator( '_' );
        nbSensorParts = sensorParts.split( sensorID );
        syntaxError = ( nbSensorParts == 2 ) ? 0 : 2;
    }

    if ( syntaxError == 0 )
    {
        syntaxError = ( sensorParts[ 0 ].size() > strlen( C_UNKNOWN_TYPE_HEAD ) ) ? 0 : 3;
    }

    if ( syntaxError == 0 )
    {
        deviceTypeString = sensorParts[ 0 ].substr( strlen( C_UNKNOWN_TYPE_HEAD ) );
        deviceType       = deviceTypeString.toUInt();
        syntaxError      = ( deviceType > 0 ) ? 0 : 4;
    }

    if ( syntaxError == 0 )
    {
        deviceIDNo  = sensorParts[ 1 ];
        syntaxError = ( deviceIDNo.toUInt() != 0 ) ? 0 : 5;
    }

    if ( syntaxError == 0 )
    {
        BYTE payLoad[ C_ANT_PAYLOAD_LENGTH ] = { 0 };

        timeStampBuffer = words[ 1 ];
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Timestamp", timeStampBuffer );
        }
        for ( counter = 0; counter < C_ANT_PAYLOAD_LENGTH; ++counter )
        {
            payLoad[ counter ]  = ( HEX_DIGIT_2_INT( words[ 2 + counter ][ 0 ] ) ) << 4;
            payLoad[ counter ] += HEX_DIGIT_2_INT( words[ 2 + counter ][ 1 ] );
            if ( diagnostics )
            {
                appendDiagnosticsLine( "payLoad", counter, payLoad[ counter ] );
            }
        }
        result = processSensor( deviceType, deviceIDNo, timeStampBuffer, payLoad );
    }
    else
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            outBuffer = inputBuffer;
        }
    }

    return result;
}

