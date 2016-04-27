// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_power_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// POWER ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antPowerProcessing::antPowerProcessing
(
    void
) : antProcessing(),
    antCadenceSpeedProcessing()
{
    setMaxZeroTimeB10( C_MAX_ZERO_TIME_POWER_B10 );
    setMaxZeroTimeB11( C_MAX_ZERO_TIME_POWER_B11 );
    setMaxZeroTimeB12( C_MAX_ZERO_TIME_POWER_B12 );
    setMaxZeroTimeB20( C_MAX_ZERO_TIME_POWER_B20 );
    resetSlopeDefault();
    resetOffsetDefault();
    setCurrentDeviceType( "POWER" );
    reset();
}

//--------------------------------------
// Data Page 16  (0x10)
bool antPowerProcessing::isPowerOnlySensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_POWER_ONLY_DEVICE_HEAD );
    return result;
}

//--------------------------------------
// Data Page 17 (0x11)
bool antPowerProcessing::isWheelTorquePowerSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_WT_POWER_DEVICE_HEAD );
    return result;
}

//--------------------------------------
// Data Page 18 (0x12)
bool antPowerProcessing::isCrankTorquePowerSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_CT_POWER_DEVICE_HEAD );
    return result;
}

//--------------------------------------
// Data Page 32 (0x20)
bool antPowerProcessing::isCrankTorqueFrequencyPowerSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_CTF_POWER_DEVICE_HEAD );
    return result;
}

//--------------------------------------
// Data Page 16, 17, 18 or 32 (0x10, 0x11, 0x12, or 0x20)
bool antPowerProcessing::isPowerSensor
(
    const amString &deviceID
)
{
    bool result = isPowerOnlySensor( deviceID ) ||
                  isWheelTorquePowerSensor( deviceID ) ||
                  isCrankTorquePowerSensor( deviceID ) ||
                  isCrankTorqueFrequencyPowerSensor( deviceID );
    return result;
}

//--------------------------------------
// Any Data Page
bool antPowerProcessing::isPowerMeterRelated
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_POWER_DEVICE_HEAD );
    return result;
}

//--------------------------------------
// Data Page 1 (0x01)
bool antPowerProcessing::isPowerMeterCalibration
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_CALIBRATION_HEAD );
    return result;
}

//--------------------------------------
// Data Page 2 (0x02)
bool antPowerProcessing::isPowerMeterGetSetParameters
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_GET_SET_PARAM_HEAD );
    return result;
}

//--------------------------------------
// Data Page 3 (0x03)
bool antPowerProcessing::isPowerMeterMeasurementOutput
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_MEASUREMENT_HEAD );
    return result;
}

//--------------------------------------
// Data Page 19 (0x13)
bool antPowerProcessing::isPowerMeterPedalSmoothness
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_PEDAL_SMOOTH_HEAD );
    return result;
}

//--------------------------------------
// Data Page 70 (0x46)
bool antPowerProcessing::isPowerMeterRequest
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_REQUEST_HEAD );
    return result;
}

//--------------------------------------
// Data Page 80 (0x50)
bool antPowerProcessing::isPowerMeterManufacturerInfo
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_MFR_INFO_HEAD );
    return result;
}

//--------------------------------------
// Data Page 81 (0x51)
bool antPowerProcessing::isPowerMeterProductInfo
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_PROD_INFO_HEAD );
    return result;
}

//--------------------------------------
// Data Page 82 (0x52)
bool antPowerProcessing::isPowerMeterStatusMessage
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_PM_STATUS_MSG_HEAD );
    return result;
}

bool antPowerProcessing::appendPowerSensor
(
    const amString &sensorID
)
{
    bool result = isPowerSensor( sensorID );
    if ( result )
    {
        if ( !isRegisteredDevice( sensorID ) )
        {
            registerDevice( sensorID );
        }
    }
    return result;
}

bool antPowerProcessing::appendPowerSensor
(
    const amString &sensorID,
    unsigned int    offset,
    unsigned int    slope_Nm_10Hz
)
{
    bool result = isCrankTorqueFrequencyPowerSensor( sensorID );
    if ( result )
    {
        if ( slope_Nm_10Hz_Table.count( sensorID ) == 0 )
        {
            slope_Nm_10Hz_Table.insert ( std::pair<amString, unsigned int>( sensorID, 0 ) );
        }
        slope_Nm_10Hz_Table[ sensorID ] = slope_Nm_10Hz;

        if ( offsetTable.count( sensorID ) == 0 )
        {
            offsetTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
        }
        offsetTable[ sensorID ] = offset;

        if ( !isRegisteredDevice( sensorID ) )
        {
            registerDevice( sensorID );
        }
    }
    return result;
}

double antPowerProcessing::getPower
(
    const amString &sensorID
)
{
    if ( powerTable.count( sensorID ) == 0 )
    {
        powerTable.insert( std::pair<amString, double>( sensorID, 0 ) );
    }
    double power = powerTable[ sensorID ];
    return power;
}

void antPowerProcessing::setPower
(
    const amString &sensorID,
    double          value
)
{
    if ( powerTable.count( sensorID ) == 0 )
    {
        powerTable.insert( std::pair<amString, double>( sensorID, 0 ) );
    }
    powerTable[ sensorID ] = value;
}

double antPowerProcessing::getTorque
(
    const amString &sensorID
)
{
    if ( torqueTable.count( sensorID ) == 0 )
    {
        torqueTable.insert( std::pair<amString, double>( sensorID, 0 ) );
    }
    double torque = torqueTable[ sensorID ];
    return torque;
}

void antPowerProcessing::setTorque
(
    const amString &sensorID,
    double          value
)
{
    if ( torqueTable.count( sensorID ) == 0 )
    {
        torqueTable.insert( std::pair<amString, double>( sensorID, 0 ) );
    }
    torqueTable[ sensorID ] = value;
}

unsigned int antPowerProcessing::getOffset
(
    const amString &sensorID
)
{
    if ( offsetTable.count( sensorID ) == 0 )
    {
        offsetTable.insert( std::pair<amString, unsigned int>( sensorID, offsetDefault ) );
    }
    unsigned int offset = offsetTable[ sensorID ];
    return offset;
}

unsigned int antPowerProcessing::getSlope_Nm_10Hz
(
    const amString &sensorID
)
{
    if ( slope_Nm_10Hz_Table.count( sensorID ) == 0 )
    {
        slope_Nm_10Hz_Table.insert( std::pair<amString, unsigned int>( sensorID, slopeDefault ) );
    }
    unsigned int slope_Nm_10Hz = slope_Nm_10Hz_Table[ sensorID ];
    return slope_Nm_10Hz;
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
// Return amDeviceType POWER_METER if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
//---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_POWER_TYPE )
    {
        result = processPowerMeter( deviceIDNo, timeStampBuffer, payLoad );
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

amDeviceType antPowerProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        // Cadence-Only Sensor
        if ( isPowerMeterRelated( inputBuffer ) )
        {
            result = processPowerMeterSemiCooked( inputBuffer );
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

void antPowerProcessing::readDeviceFileLine
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
        else
        {
            amString curErrorMessage;
            amString deviceName = words[ 1 ];

            if ( isPowerSensor( deviceName ) )
            {
                if ( deviceType == C_POWER_DEVICE_ID )
                {
                    if ( isCrankTorqueFrequencyPowerSensor( deviceName ) )
                    {
                        unsigned int curOffset      = offsetDefault;
                        unsigned int curSlope10NmHz = slopeDefault;
                        if ( nbWords > 2 )
                        {
                            curOffset = words[ 2 ].toUInt();
                            if ( ( curOffset < C_MIN_ZERO_OFFSET ) || ( curOffset > C_MAX_ZERO_OFFSET ) )
                            {
                                appendErrorMessage( "WARNING: Value for power meter zero offset (" );
                                appendErrorMessage( curOffset );
                                appendErrorMessage( " Hz) is outside of recommended range [" );
                                appendErrorMessage( C_MIN_ZERO_OFFSET );
                                appendErrorMessage( ", " );
                                appendErrorMessage( C_MAX_ZERO_OFFSET );
                                appendErrorMessage( "].\n" );
                                curOffset = offsetDefault;
                                errorCode = E_BAD_PARAMETER_VALUE;
                                appendErrorMessage( "             The value has been set to its default (" );
                                appendErrorMessage( curOffset, 4 );
                                appendErrorMessage( " Hz).\n" );
                            }
                            if ( nbWords > 3 )
                            {
                                double curSlope = words[ 3 ].toDouble();
                                if ( curSlope >= 0 )
                                {
                                    if ( ( curSlope < C_MIN_PM_SLOPE ) || ( curSlope > C_MAX_PM_SLOPE ) )
                                    {
                                        if ( errorCode )
                                        {
                                            appendErrorMessage( "         " );
                                        }
                                        else
                                        {
                                            appendErrorMessage( "WARNING: " );
                                        }
                                        appendErrorMessage( "Value for power meter slope (" );
                                        appendErrorMessage( curSlope, 1 );
                                        appendErrorMessage( " Nm/Hz) is outside of recommended range [" );
                                        appendErrorMessage( C_MIN_PM_SLOPE, 1 );
                                        appendErrorMessage( ", " );
                                        appendErrorMessage( C_MAX_PM_SLOPE, 1 );
                                        appendErrorMessage( "].\n" );
                                        appendErrorMessage( "             The value has been set to its default (factory slope).\n" );
                                        errorCode = E_BAD_PARAMETER_VALUE;
                                    }
                                    else
                                    {
                                       curSlope *= 10;
                                       curSlope10NmHz = ( unsigned int ) round( curSlope );
                                    }
                                }
                            }
                            if ( errorCode )
                            {
                                appendErrorMessage( "         Line in file: \"" );
                                appendErrorMessage( line );
                                appendErrorMessage( "\".\n" );
                            }
                        }
                        appendPowerSensor( deviceName, curOffset, curSlope10NmHz );
                    }
                    else if ( isWheelTorquePowerSensor( deviceName ) )
                    {
                        appendPowerSensor( deviceName );
                    }
                    else
                    {
                        appendPowerSensor( deviceName );
                    }
                }
                else if ( deviceType == C_SPEED_DEVICE_ID )
                {
                    if ( isWheelTorquePowerSensor( deviceName ) )
                    {
                        errorCode = antSpeedProcessing::readDeviceFileLine1( line, curErrorMessage );
                    }
                    else
                    {
                        errorCode = antCadenceSpeedProcessing::readDeviceFileLine1( line, curErrorMessage );
                    }
                }
                else if ( deviceType == C_CADENCE_DEVICE_ID )
                {
                    errorCode = antCadenceProcessing::readDeviceFileLine1( line, curErrorMessage );
                }
                if ( errorCode )
                {
                    appendErrorMessage( curErrorMessage );
                }
            }
        }
    }
}

void antPowerProcessing::reset
(
    void
)
{
    antProcessing::reset();
    antCadenceSpeedProcessing::reset();
    resetSlopeDefault();
    resetOffsetDefault();

    //.clear();
    offsetTable.clear();
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// POWER
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
#ifdef __SINGLE_LEG__
const unsigned int C_MAX_DELTA_EVENT_COUNT = 10;

bool antPowerProcessing::isLeftCrankEvent
(
    const amString &sensorID,
    unsigned int    newValue,
    unsigned int    rollOver
)
{
    bool         result           = false;
    unsigned int prevLeftValue    = eventCountTable[ sensorID ];
    unsigned int prevRightValue   = otherEventCountTable[ sensorID ];
    bool         rollOverHappened = ( newValue < prevLeftValue );
    unsigned int leftDeltaValue   = rollOverHappened ? ( rollOver - prevLeftValue + newValue ) : ( newValue - prevLeftValue );
    rollOverHappened              = ( newValue < prevRightValue );
    unsigned int rightDeltaValue  = rollOverHappened ? ( rollOver - prevRightValue + newValue ) : ( newValue - prevRightValue );

    if ( ( prevLeftValue == 0 ) && ( prevRightValue == 0 ) )
    {
        result = true;
    }
    else if ( prevLeftValue == 0 )
    {
        if ( rightDeltaValue < C_MAX_DELTA_EVENT_COUNT )
        {
            result = false;
        }
        else
        {
            result = true;
        }
    }
    else if ( prevRightValue == 0 )
    {
        if ( leftDeltaValue < C_MAX_DELTA_EVENT_COUNT )
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = ( leftDeltaValue < rightDeltaValue );
    }

    return result;
}
#else // __SINGLE_LEG__
#define isLeftCrankEvent( A, B, C ) true
#endif // __SINGLE_LEG__

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB01: Power Meter Calibration Request
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
unsigned int antPowerProcessing::splitFormat137_B01
(
    const amString &inputBuffer,
    amSplitString  &outWords
)
{
    unsigned int nbWords = 0;
    if ( !inputBuffer.empty() )
    {
        amSplitString inWords;
        bool          success    = false;
        unsigned int  counter    = 0;
        unsigned int  subCounter = 0;
        amString      calibrationID;

        nbWords = inWords.split( inputBuffer );

        if ( nbWords > 4 )
        {
            unsigned int   nbSubWords = 0;
            amSplitString subWords;

            subWords.removeAllSeparators();
            subWords.addSeparator( '=' );

            outWords.push_back( inWords[ counter++ ] );   // Device ID
            outWords.push_back( inWords[ counter++ ] );   // Time Stamp
            outWords.push_back( inWords[ counter++ ] );   // Semi-cooked Symbol
            if ( inWords[ counter ].contains( "=" ) )
            {
                calibrationID = inWords[ counter++ ];        // Calibration ID
                if ( nbWords > counter )
                {
                    nbSubWords = subWords.split( inWords[ counter++ ] );
                }
            }
            else
            {
                nbSubWords = subWords.split( inWords[ counter++ ] );
                calibrationID = subWords[ subCounter++ ];   // Calibration ID
            }
            outWords.push_back( calibrationID );

            if ( calibrationID == "16" )
            {
                if ( nbSubWords > 1 )
                {
                    if ( ( subWords[ subCounter ] == "1" ) || ( subWords[ subCounter ] == "2" ) ||
                         ( subWords[ subCounter ] == "3" ) || ( subWords[ subCounter ] == "172" ) )
                    {
                        // Slope / Offset / SerialNo / Acknowledgement
                        outWords.push_back( subWords[ subCounter++ ] );
                        outWords.push_back( "1" );
                        outWords.push_back( subWords[ subCounter++ ] );
                        success = true;
                    }
                }
            }
            else if ( calibrationID == "18" )
            {
                if ( nbSubWords > 1 )
                {
                    outWords.push_back( subWords[ subCounter++ ] );
                    outWords.push_back( subWords[ subCounter++ ] );
                    success = true;
                }
            }
            else if ( ( calibrationID == "170" ) || ( calibrationID == "186" ) || ( calibrationID == "187" )
                                                 || ( calibrationID == "188" ) || ( calibrationID == "189" ) )
            {
                success = true;
            }
            else if ( calibrationID == "171" )
            {
                if ( nbSubWords > 0 )
                {
                    outWords.push_back( subWords[ subCounter++ ] );
                    success = true;
                }
            }
            else if ( ( calibrationID == "172" ) || ( calibrationID == "175" ) )
            {
                if ( nbSubWords > 1 )
                {
                    outWords.push_back( subWords[ subCounter++ ] );
                    outWords.push_back( subWords[ subCounter++ ] );
                    success = true;
                }
            }
        }

        if ( success )
        {
            if ( nbWords > counter )
            {
                outWords.push_back( inWords[ counter++ ] );   // Version
            }
        }
        else
        {
            outWords.clear();
        }
        nbWords = outWords.size();
    }

    return nbWords;
}

bool antPowerProcessing::createB01ResultString
(
    const amSplitString &words
)
{
    amString     sensorID;
    amString     timeStampBuffer;
    amString     semiCookedString;
    amString     curVersion      = getVersion();
    bool         result          = false;
    unsigned int nbWords         = words.size();
    unsigned int counter         = 0;
    unsigned int calibrationID   = 0;
    unsigned int additionalData1 = 0;
    unsigned int additionalData2 = 0;

    if ( nbWords > 3 )
    {
        sensorID         = words[ counter++ ];                                             // 0
        timeStampBuffer  = words[ counter++ ];                                             // 1
        semiCookedString = words[ counter++ ];                                             // 2
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }
        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterCalibration( sensorID ) )
        {
            calibrationID = words[ counter++ ].toUInt();                          // 3
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Calibration ID", calibrationID );
            }
            if ( calibrationID == C_CTF_CALIBRATION_MESSAGE_ID )                                // 16 = 0x10: Crank Torque Frequency (CTF) Power Sensor defined message
            {
                // Take care of old format first...
                if ( nbWords > 5 )
                {
                    result = true;
                    if ( nbWords > 6 )
                    {
                        if ( words[ counter ].isUnsignedInteger() )
                        {
                            additionalData1 = words[ counter++ ].toUInt();
                            additionalData2 = words[ counter++ ].toUInt();
                        }
                        else
                        {
                            additionalData1 = words[ counter++ ].toUInt();
                            additionalData2 = words[ counter++ ].toUInt();
                        }
                    }
                    else
                    {
                        additionalData1 = words[ counter++ ].toUInt();
                        additionalData2 = words[ counter++ ].toUInt();
                    }

                    if ( diagnostics )
                    {
                        appendDiagnosticsLine( "CTF Defined ID", additionalData1 );
                        appendDiagnosticsLine( "Message Value", additionalData2 );
                    }
                }
            }
            else if ( calibrationID == C_AUTO_ZERO_SUPPORT_MESSAGE_ID )                         //  18 = 0x12
            {
                // Old format == New format
                if ( nbWords > 5 )
                {
                    result          = true;
                    additionalData1 = words[ counter++ ].toUInt();                     // 4
                    additionalData2 = words[ counter++ ].toUInt();                     // 5
                    if ( diagnostics )
                    {
                        appendDiagnosticsLine( "Auto Zero Enable", additionalData1 );
                        appendDiagnosticsLine( "Auto Zero Status", additionalData2 );
                    }
                }
            }
            else if ( calibrationID == C_CALIBRATION_REQUEST_MANUAL_ZERO_ID )                   // 170 = 0xAA
            {
                result = true;
            }
            else if ( calibrationID == C_CALIBRATION_REQUEST_AUTO_ZERO_ID )                     // 171 = 0xAB
            {
                if ( nbWords > 4 )
                {
                    result          = true;
                    additionalData1 = words[ counter++ ].toUInt();                     // 4
                    if ( diagnostics )
                    {
                        appendDiagnosticsLine( "Auto Zero Status", additionalData1 );
                    }
                }
            }
            else if ( ( calibrationID == C_CALIBRATION_RESPONSE_MANUAL_ZERO_SUCCESS_ID ) ||            // 172 = 0xAC
                      ( calibrationID == C_CALIBRATION_RESPONSE_MANUAL_ZERO_FAIL_ID ) )                // 175 = 0xAF
            {
                if ( nbWords > 5 )
                {
                    result          = true;
                    additionalData1 = words[ counter++ ].toUInt();                     // 4
                    additionalData2 = words[ counter++ ].toUInt();                     // 5
                    if ( diagnostics )
                    {
                        appendDiagnosticsLine( "Auto Zero Status", additionalData1 );
                        appendDiagnosticsLine( "Message Value", additionalData2 );
                    }
                }
            }
            else if ( ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_REQUEST_ID ) ||             // 186 = 0xBA
                      ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_RESPONSE_ID ) ||            // 187 = 0xBB
                      ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_ID ) ||              // 188 = 0xBC
                      ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_RESPONSE_ID ) )      // 189 = 0xBD
            {
                result = true;
            }
        }
    }

    if ( result )
    {
        if ( words.size() > counter )
        {
            curVersion = words.back();
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB01ResultString( calibrationID, additionalData1, additionalData2 );
        appendOutputFooter( curVersion );
    }
    return result;
}

amDeviceType antPowerProcessing::processPowerMeterB01
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result          = OTHER_DEVICE;
    amString     sensorID        = amString( C_PM_CALIBRATION_HEAD ) + deviceIDNo;
    unsigned int auxInt          = 0;
    unsigned int calibrationID   = 0;
    unsigned int additionalData1 = 0;
    unsigned int additionalData2 = 0;

    calibrationID = hex2Int( payLoad[ 1 ] );
    if ( diagnostics )
    {
        appendDiagnosticsLine( "\nCalibration ID", payLoad[ 1 ], calibrationID );
    }

    if ( calibrationID == C_CTF_CALIBRATION_MESSAGE_ID )                                   //  16 = 0x10
    {
        // Crank Torque Frequency (CTF) Power Sensor defined message
        result          = POWER_METER;
        additionalData1 = hex2Int( payLoad[ 2 ] );   // CTF Defined ID
        if ( diagnostics )
        {
            appendDiagnosticsLine( "CTF Defined ID", payLoad[ 2 ], additionalData1 );
        }

        if ( ( additionalData1 == 1 ) || ( additionalData1 == 2 ) || ( additionalData1 == 3 ) )
        {
            additionalData2 = hex2Int( payLoad[ 6 ], payLoad[ 7 ] );    // Message Value
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Message Value", payLoad[ 7 ], payLoad[ 6 ], additionalData2 );
            }
        }
        else if ( additionalData1 == 172 )
        {
            additionalData2 = hex2Int( payLoad[ 3 ] );   // CTF Acknowledge Message
            if ( diagnostics )
            {
                appendDiagnosticsLine( "CTF Acknowledge Message", payLoad[ 3 ], additionalData2 );
            }
        }
    }
    else if ( calibrationID == C_AUTO_ZERO_SUPPORT_MESSAGE_ID )                            //  18 = 0x12
    {
        result          = POWER_METER;
        auxInt          = hex2Int( payLoad[ 2 ] );
        additionalData1 = auxInt & 1;     // Auto Zero Enable
        additionalData2 = auxInt & 2;     // Auto Zero Status
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Auto Zero Enable", additionalData1 );
            appendDiagnosticsLine( "Auto Zero Status", additionalData2 );
        }
    }
    else if ( ( calibrationID == C_CALIBRATION_REQUEST_MANUAL_ZERO_ID ) ||                   // 170 = 0xAA
              ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_REQUEST_ID ) ||              // 186 = 0xBA
              ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_RESPONSE_ID ) ||             // 187 = 0xBB
              ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_ID ) ||               // 188 = 0xBC
              ( calibrationID == C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_RESPONSE_ID ) )       // 189 = 0xBD
    {
        result = POWER_METER;
    }
    else if ( calibrationID == C_CALIBRATION_REQUEST_AUTO_ZERO_ID )                        // 171 = 0xAB
    {
        result          = POWER_METER;
        additionalData1 = hex2Int( payLoad[ 2 ] );    // Auto Zero Status
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Auto Zero Status", payLoad[ 2 ], additionalData1 );
        }
    }
    else if ( ( calibrationID == C_CALIBRATION_RESPONSE_MANUAL_ZERO_SUCCESS_ID ) ||         // 172 = 0xAC
              ( calibrationID == C_CALIBRATION_RESPONSE_MANUAL_ZERO_FAIL_ID ) )             // 175 = 0xAF
    {
        result          = POWER_METER;
        additionalData1 = hex2Int( payLoad[ 2 ] );                  // Auto Zero Status
        additionalData2 = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );    // Message Value
        if ( diagnostics )
        {
            char auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
            appendDiagnosticsLine( "Auto Zero Status", payLoad[ 2 ], additionalData1 );
            int messageValue = NEGATE_BINARY_INT( additionalData2, 16 );
            sprintf( auxBuffer, "(signed: %d)", messageValue );
            appendDiagnosticsLine( "Message Value", payLoad[ 7 ], payLoad[ 6 ], additionalData2, auxBuffer );
        }
    }

    if ( result == POWER_METER )
    {
        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB01ResultString( calibrationID, additionalData1, additionalData2 );
        appendOutputFooter( getVersion() );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB01SemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outtBuffer.
// The output string has the form
//     "PWRB01_<device_ID>
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB01SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        bool          resultCreate = false;
        unsigned int  nbWords      = 0;
        amSplitString words;
        if ( isSemiCookedFormat137( inputBuffer ) )
        {
            nbWords = splitFormat137_B01( inputBuffer, words );
        }
        else
        {
            nbWords = words.split( inputBuffer );
        }

        resultCreate = createB01ResultString( words );

        if ( resultCreate )
        {
            result = POWER_METER;
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

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB02: Power Sensor Get/Set Parameters
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB02
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outtBuffer.
// The output string has the form
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB02
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result          = OTHER_DEVICE;
    amString     sensorID        = amString( C_PM_GET_SET_PARAM_HEAD ) + deviceIDNo;
    unsigned int subPageNumber   = 0;
    unsigned int additionalData1 = 0;
    unsigned int additionalData2 = 0;
    unsigned int additionalData3 = 0;

    // ---------------------------------------------------------------
    // Payload
    //   0   Command ID (0x02)
    //   1   Subpage Number
    //   2-7 Subpage Defined Data

    subPageNumber = hex2Int( payLoad[ 1 ] );
    if ( diagnostics )
    {
        appendDiagnosticsLine( "Sub Page Number", payLoad[ 2 ], subPageNumber );
    }

    if ( subPageNumber == 1 )
    {
        // Crank Parameters
        result          = POWER_METER;
        additionalData1 = hex2Int( payLoad[ 4 ] );
        additionalData2 = hex2Int( payLoad[ 5 ] );
        additionalData3 = hex2Int( payLoad[ 6 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Crank Length (int)", payLoad[ 4 ], additionalData1 );
            appendDiagnosticsLine( "Sensor Status", payLoad[ 5 ], additionalData2 );
            appendDiagnosticsLine( "Sensor Capabilities", payLoad[ 6 ], additionalData3 );
        }
    }

    if ( result == POWER_METER )
    {
        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB02ResultString( subPageNumber, additionalData1, additionalData2, additionalData3 );
        appendOutputFooter( getVersion() );
    }

    return result;
}

amDeviceType antPowerProcessing::processPowerMeterB02SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords      = words.split( inputBuffer );
        bool          resultCreate = false;

        if ( nbWords > 3 )
        {
            resultCreate = createB02ResultString( words );
        }

        if ( resultCreate )
        {
            result = POWER_METER;
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

bool antPowerProcessing::createB02ResultString
(
    const amSplitString &words
)
{
    bool         result          = false;
    bool         oldFormat       = false;
    unsigned int nbWords         = words.size();
    unsigned int counter         = 0;
    unsigned int subPageNumber   = 0;
    unsigned int additionalData1 = 0;
    unsigned int additionalData2 = 0;
    unsigned int additionalData3 = 0;
    amString     curVersion      = getVersion();
    amString     sensorID;
    amString     timeStampBuffer;
    amString     semiCookedString;

    if ( nbWords > 3 )
    {
        sensorID         = words[ counter++ ];                                  // 0
        timeStampBuffer  = words[ counter++ ];                                  // 1
        semiCookedString = words[ counter++ ];                                  // 2
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }
        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterGetSetParameters( sensorID ) )
        {
            subPageNumber = words[ counter++ ].toUInt();                         // 3
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Sub Page Number", subPageNumber );
            }

            if ( nbWords > 8 )
            {
                // Old format:
                //   "<sensorID> <timeStampBuffer> <semiCookedString> <INT> <INT> <crankLengthInt> <sensorStatus> <sensorCapabilities> <INT> <version>"
                // where <INT> is an unused byte from the original ANT+ payload.
                // The current fromat only outputs bytes that are used.
                oldFormat  = true;
                counter   += 2;
            }

            if ( subPageNumber == 1 )
            {
                // Crank Parameters
                if ( nbWords > 6 )
                {
                    result          = true;
                    additionalData1 = words[ counter++ ].toUInt();            // 4 Crank Length (int)
                    additionalData2 = words[ counter++ ].toUInt();            // 5 Sensor Status
                    additionalData3 = words[ counter++ ].toUInt();            // 6 Sensor Capabilities
                    if ( diagnostics )
                    {
                        appendDiagnosticsLine( "Crank Length (int)", additionalData1 );
                        appendDiagnosticsLine( "Sensor Status", additionalData2 );
                        appendDiagnosticsLine( "Sensor Capabilities", additionalData3 );
                    }
                    if ( oldFormat )
                    {
                        // Old format (see comment above)
                        counter++;
                    }
                }
            }
        }
    }

    if ( result )
    {
        if ( words.size() > counter )
        {
            curVersion = words[ counter ];
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB02ResultString( subPageNumber, additionalData1, additionalData2, additionalData3 );
        appendOutputFooter( curVersion );
    }

    return result;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB03: Power Only Sensor - Measurement Output
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antPowerProcessing::processPowerMeterB03
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    unsigned int nbDataTypes      = 0;
    unsigned int dataType         = 0;
    unsigned int timeStampInt     = 0;
    unsigned int rollOver         = 0;
    unsigned int scaleFactor      = 0;
    unsigned int value            = 0;
    unsigned int deltaEventTime   = 0;
    bool         rollOverHappened = false;
    double       timeStampDbl     = 0;
    amDeviceType result           = OTHER_DEVICE;
    amString     sensorID         = amString( C_PM_MEASUREMENT_HEAD ) + deviceIDNo;

    if ( totalTimeTable.count( sensorID ) == 0 )
    {
        totalTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
        eventTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
    }

    // ---------------------------------------------------------------
    // Payload
    //   0   Command ID (0x03)
    //   1   Bits 0-3: Number of Data Types Currently Being Transmitted.
    //       Bits 4-7: Reserved (currently set to 0000)
    //   2   Data Type
    //   3   Scale Factor               Range: -127 to 127
    //   4   Time Stamp LSB          \     Unit:
    //   5   Time Stamp MSB          /    1/2048s
    //   6   Measurement Value LSB   \      Range:
    //   7   Measurement Value MSB   / -32768 to 32767

    nbDataTypes                 = hex2Int( payLoad[ 1 ] ) & 0x07;
    dataType                    = hex2Int( payLoad[ 2 ] );
    scaleFactor                 = hex2Int( payLoad[ 3 ] );
    timeStampInt                = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );
    rollOver                    = ( 1 << 16 );  // 2^16 = 256^2
    deltaEventTime              = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, timeStampInt );
    timeStampDbl                = totalTimeTable[ sensorID ];
    timeStampDbl               += ( double ) deltaEventTime / 2048.0;
    totalTimeTable[ sensorID ]  = timeStampDbl;
    value                       = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );
    if ( diagnostics )
    {
        char auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
        appendDiagnosticsLine( "Number of Data Types", payLoad[ 1 ], nbDataTypes, " (Lower 4 Bits)" );
        appendDiagnosticsLine( "Data Type", payLoad[ 2 ], dataType );
        int scaleFactorSigned = NEGATE_BINARY_INT( scaleFactor, 8 );
        sprintf( auxBuffer, "(signed %d)", scaleFactorSigned );
        appendDiagnosticsLine( "Scale Factor", payLoad[ 3 ], scaleFactor, auxBuffer );
        appendDiagnosticsLine( "Time Stamp (int)", payLoad[ 5 ], payLoad[ 4 ], timeStampInt );
        int valueSigned = NEGATE_BINARY_INT( value, 16 );
        sprintf( auxBuffer, "(signed %d)", valueSigned );
        appendDiagnosticsLine( "Value", payLoad[ 5 ], payLoad[ 4 ], value, auxBuffer );
    }

    // - - - - - - - - - - - - - - - -
    // Create the Result String
    createOutputHeader( sensorID, timeStampBuffer );
    createPWRB03ResultString( nbDataTypes, dataType, scaleFactor, deltaEventTime, value, timeStampDbl );
    appendOutputFooter( getVersion() );

    return result;
}

amDeviceType antPowerProcessing::processPowerMeterB03SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords      = words.split( inputBuffer );
        bool          resultCreate = false;

        if ( nbWords )
        {
            resultCreate = createB03ResultString( words );
        }

        if ( resultCreate )
        {
            result = POWER_METER;
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

bool antPowerProcessing::createB03ResultString
(
    const amSplitString &words
)
{
    bool         result               = false;
    unsigned int nbWords              = words.size();
    unsigned int counter              = 0;
    unsigned int additionalData1      = 0;
    unsigned int additionalData2      = 0;
    unsigned int additionalData3      = 0;
    unsigned int additionalData4      = 0;
    unsigned int additionalData5      = 0;
    double       additionalDoubleData = 0;
    amString     curVersion           = getVersion();
    amString     sensorID;
    amString     timeStampBuffer;
    amString     semiCookedString;

    if ( nbWords > 7 )
    {
        sensorID         = words[ counter++ ];                                  // 0
        timeStampBuffer  = words[ counter++ ];                                  // 1
        semiCookedString = words[ counter++ ];                                  // 2
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }
        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterMeasurementOutput( sensorID ) )
        {
            if ( totalTimeTable.count( sensorID ) == 0 )
            {
                totalTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
                eventTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
            }
            result                      = true;
            additionalData1             = words[ counter++ ].toUInt();          // 3 Number of Data Types
            additionalData2             = words[ counter++ ].toUInt();          // 4 Data Type
            additionalData3             = words[ counter++ ].toUInt();          // 5 Scale Factor
            additionalData4             = words[ counter++ ].toUInt();          // 6 Delta Event Time
            additionalData5             = words[ counter++ ].toUInt();          // 7 Value,
            additionalDoubleData        = totalTimeTable[ sensorID ];
            additionalDoubleData       += ( double ) additionalData4 / 2048.0;
            totalTimeTable[ sensorID ]  = additionalDoubleData;
        }
    }

    if ( result )
    {
        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB03ResultString
        (
            additionalData1,
            additionalData2,
            additionalData3,
            additionalData4,
            additionalData5,
            additionalDoubleData
        );
        if ( words.size() > counter )
        {
            curVersion = words[ counter ];
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        appendOutputFooter( curVersion );
    }

    return result;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB10: Power Only Power Meter
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antPowerProcessing::processPowerMeterB10
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result                = OTHER_DEVICE;
    amString     sensorID              = amString( C_POWER_ONLY_DEVICE_HEAD ) + deviceIDNo;
    unsigned int eventCount            = 0;
    unsigned int pedalPower            = 0;
    unsigned int instantaneousCadence  = 0;
    unsigned int accumulatedPower      = 0;
    unsigned int instantaneousPower    = 0;
    unsigned int deltaEventCount       = 0;
    unsigned int totalEventCount       = 0;
    unsigned int deltaAccumulatedPower = 0;
    unsigned int rollOver              = 0;
    bool         rollOverHappened      = false;
    bool         left                  = false;

    if ( powerTable.count( sensorID ) == 0 )
    {
       appendPowerSensor( sensorID );
    }

    if ( isRegisteredDevice( sensorID ) )
    {
        result = POWER_METER;

        // - - - - - - - - - - - - - - - -
        // Event Count
        eventCount       = hex2Int( payLoad[ 1 ] );
        accumulatedPower = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );
        rollOver         = 256;
        if ( isLeftCrankEvent( sensorID, eventCount, rollOver ) )
        {
            left                        = true;
            deltaEventCount             = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, eventCount );
            totalEventCount             = totalCountTable[ sensorID ] + deltaEventCount;
            totalCountTable[ sensorID ] = totalEventCount;

            // - - - - - - - - - - - - - - - -
            // Accumulated Power
            rollOver              = 65536;  // 256^2
            deltaAccumulatedPower = getDeltaInt( rollOverHappened, sensorID, rollOver, accumulatedPowerTable, accumulatedPower );
        }
        else
        {
            deltaEventCount                  = getDeltaInt( rollOverHappened, sensorID, rollOver, otherEventCountTable, eventCount );
            totalEventCount                  = otherTotalCountTable[ sensorID ] + deltaEventCount;
            otherTotalCountTable[ sensorID ] = totalEventCount;

            // - - - - - - - - - - - - - - - -
            // Accumulated Power
            rollOver              = 65536;  // 256^2
            deltaAccumulatedPower = getDeltaInt( rollOverHappened, sensorID, rollOver, otherAccumulatedPowerTable, accumulatedPower );
        }

        if ( diagnostics )
        {
            char auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
            appendDiagnosticsLine( "Event Count", payLoad[ 1 ], eventCount );
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Event Count", deltaEventCount, auxBuffer );
            appendDiagnosticsLine( "Accumulated Power", payLoad[ 5 ], payLoad[ 4 ], accumulatedPower );
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Accumulated Power", deltaAccumulatedPower, auxBuffer );
        }


        // - - - - - - - - - - - - - - - -
        // Pedal Power
        pedalPower = hex2Int( payLoad[ 2 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Pedal Power", payLoad[ 2 ], pedalPower );
        }


        // - - - - - - - - - - - - - - - -
        // Instantaneous Cadence
        instantaneousCadence = hex2Int( payLoad[ 3 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Instantaneous Cadence", payLoad[ 3 ], instantaneousCadence );
        }


        // - - - - - - - - - - - - - - - -
        // Instantaneous Power
        instantaneousPower = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Instantaneous Power", payLoad[ 7 ], payLoad[ 6 ], instantaneousPower );
        }
    }

    if ( result == POWER_METER )
    {
        double       power              = getPower( sensorID );
        double       speed              = 0;
        double       wheelCircumference = 0;
        double       gearRatio          = getNbMagnets( sensorID );
        unsigned int cadence            = getCadence( sensorID );
        unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );

        createOutputHeader( sensorID, timeStampBuffer );
        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            speed              = getSpeed( sensorID );
            wheelCircumference = getWheelCircumference( sensorID );
            gearRatio          = getNbMagnets( sensorID );
        }

        createPWRB10ResultString
        (
            power,
            cadence,
            zeroTimeCount,
            eventCount,
            deltaEventCount,
            deltaAccumulatedPower,
            instantaneousPower,
            instantaneousCadence,
            pedalPower,
            isUsedAsSpeedSensor( sensorID ),
            speed,
            wheelCircumference,
            gearRatio
        );
        appendOutputFooter( getVersion() );

        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            setSpeed( sensorID, speed );
        }
        setPower( sensorID,   power );
        setCadence( sensorID, cadence );
        setZeroTimeCount( sensorID, zeroTimeCount );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB10SemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outtBuffer.
// The output string has the form
//     "PWRB10_<device_ID> <event_count> <accumulated_power> <instantaneous_power> <pedal_power> <instantaneous_cadence>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB10SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amString      sensorID;
        amString      timeStampBuffer;
        amString      semiCookedString;
        amString      curVersion            = getVersion();
        amSplitString words;
        unsigned int  nbWords               = words.split( inputBuffer );
        unsigned int  eventCount            = 0;
        unsigned int  deltaEventCount       = 0;
        unsigned int  deltaAccumulatedPower = 0;
        unsigned int  instantaneousPower    = 0;
        unsigned int  instantaneousCadence  = 0;
        unsigned int  pedalPower            = 0;
        unsigned int  counter               = 0;

        if ( nbWords > 7 )
        {
            sensorID         = words[ counter++ ];                             // 0
            timeStampBuffer  = words[ counter++ ];                             // 1
            semiCookedString = words[ counter++ ];                             // 2
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerOnlySensor( sensorID ) )
            {
                result                = POWER_METER;
                deltaEventCount       = words[ counter++ ].toUInt();            // 3
                deltaAccumulatedPower = words[ counter++ ].toUInt();            // 4
                instantaneousPower    = words[ counter++ ].toUInt();            // 5
                instantaneousCadence  = words[ counter++ ].toUInt();            // 6
                pedalPower            = words[ counter++ ].toUInt();            // 7
                if ( ( nbWords > 8 ) && words[ counter ].isUnsignedInteger() )
                {
                    eventCount        = words[ counter++ ].toUInt();            // 8
                }
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Delta Event Count", deltaEventCount );
                    appendDiagnosticsLine( "Delta Accumulated Power", deltaAccumulatedPower );
                    appendDiagnosticsLine( "Instantaneous Power", instantaneousPower );
                    appendDiagnosticsLine( "Instantaneous Cadence", instantaneousCadence );
                    appendDiagnosticsLine( "Pedal Power", pedalPower );
                    appendDiagnosticsLine( "Event Count", eventCount );
                }

                if ( powerTable.count( sensorID ) == 0 )
                {
                   appendPowerSensor( sensorID );
                }
            }
        }

        if ( result == POWER_METER )
        {
            double       power              = getPower( sensorID );
            unsigned int cadence            = getCadence( sensorID );
            unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );
            double       speed              = 0;
            double       wheelCircumference = 0;
            double       gearRatio          = 0;

            if ( nbWords > counter )
            {
                curVersion = words[ counter++ ];
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Version", curVersion );
                }
            }

            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                speed              = getSpeed( sensorID );
                wheelCircumference = getWheelCircumference( sensorID );
                gearRatio          = getNbMagnets( sensorID );
            }

            createOutputHeader( sensorID, timeStampBuffer );
            createPWRB10ResultString
            (
                power,
                cadence,
                zeroTimeCount,
                eventCount,
                deltaEventCount,
                deltaAccumulatedPower,
                instantaneousPower,
                instantaneousCadence,
                pedalPower,
                isUsedAsSpeedSensor( sensorID ),
                speed,
                wheelCircumference,
                gearRatio
            );
            appendOutputFooter( curVersion );

            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                setSpeed( sensorID, speed );
            }
            setPower  ( sensorID, power );
            setCadence( sensorID, cadence );
            setZeroTimeCount( sensorID, zeroTimeCount );
        }

        if ( result == OTHER_DEVICE )
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

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB11: Wheel Torque Power Meter (Hub Power Meter)
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB11
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
// If semiCookedOut == true
//     "PWRB11_<device_ID> <event_count> <accum_wheel_period> <accum_torque> <inst_cadence> <wheel_ticks>"
// Otherwise,
//     "PWRB11_<device_ID> <sub_second_timer> <power> <inst_cadence>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB11
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result                 = OTHER_DEVICE;
    amString     sensorID               = amString( C_WT_POWER_DEVICE_HEAD ) + deviceIDNo;
    unsigned int eventCount             = 0;
    unsigned int wheelTicks             = 0;
    unsigned int instantaneousCadence   = 0;
    unsigned int accumWheelPeriod       = 0;
    unsigned int accumulatedTorque      = 0;
    unsigned int deltaEventCount        = 0;
    unsigned int deltaWheelPeriod       = 0;
    unsigned int deltaAccumulatedTorque = 0;
    unsigned int rollOver               = 0;
    bool         rollOverHappened       = false;
    char         auxBuffer[ C_SMALL_BUFFER_SIZE ] = { 0 };

    if ( powerTable.count( sensorID ) == 0 )
    {
       appendPowerSensor( sensorID );
    }

    if ( isRegisteredDevice( sensorID ) )
    {
        result = POWER_METER;

        // - - - - - - - - - - - - - - - -
        // Event Count
        eventCount      = hex2Int( payLoad[ 1 ] );
        rollOver        = 256;
        deltaEventCount = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, eventCount );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Event Count", payLoad[ 1 ], eventCount );
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Event Count", deltaEventCount, auxBuffer );
        }
        if ( eventCountTable[ sensorID ] == eventCount )
        {
            sameEventCountTable[ sensorID ]++;
        }
        else
        {
            sameEventCountTable[ sensorID ] = 0;
        }


        // - - - - - - - - - - - - - - - -
        // Wheel Ticks
        wheelTicks = hex2Int( payLoad[ 2 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Wheel Ticks", payLoad[ 2 ], wheelTicks );
        }

        // - - - - - - - - - - - - - - - -
        // Instantaneous Cadence
        instantaneousCadence = hex2Int( payLoad[ 3 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Instantaneous Cadence", payLoad[ 3 ], instantaneousCadence );
        }

        // - - - - - - - - - - - - - - - -
        // Accumulated Power
        accumWheelPeriod = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );
        rollOver         = 65536;  // 256^2
        deltaWheelPeriod = getDeltaInt( rollOverHappened, sensorID, rollOver, crankOrWheelPeriodTable, accumWheelPeriod );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Accumulated Wheel Period", payLoad[ 5 ], payLoad[ 4 ], accumWheelPeriod );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Accumulated Wheel Period", deltaWheelPeriod, auxBuffer );
        }


        // - - - - - - - - - - - - - - - -
        // Accumulated Torque
        accumulatedTorque      = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );
        rollOver               = 65536;  // 256^2
        deltaAccumulatedTorque = getDeltaInt( rollOverHappened, sensorID, rollOver, accumulatedTorqueTable, accumulatedTorque );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Accumulated Torque", payLoad[ 7 ], payLoad[ 6 ], accumulatedTorque );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Accumulated Torque", deltaAccumulatedTorque, auxBuffer );
        }
    }

    if ( result == POWER_METER )
    {
        double       power              = getPower( sensorID );
        unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );
        unsigned int cadence            = getCadence( sensorID );
        double       torque             = getTorque( sensorID );
        double       speed              = 0;
        double       wheelCircumference = 0;
        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            speed                       = getSpeed( sensorID );
            wheelCircumference          = getWheelCircumference( sensorID );
        }

        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB11ResultString
        (
            power,
            cadence,
            torque,
            zeroTimeCount,
            deltaEventCount,
            deltaWheelPeriod,
            deltaAccumulatedTorque,
            instantaneousCadence,
            wheelTicks,
            isUsedAsSpeedSensor( sensorID ),
            speed,
            wheelCircumference
        );
        appendOutputFooter( getVersion() );

        setPower  ( sensorID, power );
        setTorque ( sensorID, torque );
        setCadence( sensorID, cadence );
        setZeroTimeCount( sensorID, zeroTimeCount );
        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            setSpeed( sensorID, speed );
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB11SemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
//     "PWRB11_<device_ID> <event_count> <accumulated_wheel_period> <accumulated_torque> <instantaneous_cadence> <wheel_ticks>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB11SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords                = words.split( inputBuffer );
        unsigned int  deltaEventCount        = 0;
        unsigned int  deltaWheelPeriod       = 0;
        unsigned int  deltaAccumulatedTorque = 0;
        unsigned int  instantaneousCadence   = 0;
        unsigned int  wheelTicks             = 0;
        unsigned int  counter                = 0;
        amString      curVersion             = getVersion();
        amString      semiCookedString;
        amString      timeStampBuffer;
        amString      sensorID;

        if ( nbWords > 7 )
        {
            sensorID         = words[ counter++ ];                      // 0
            timeStampBuffer  = words[ counter++ ];                      // 1
            semiCookedString = words[ counter++ ];                      // 2
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isWheelTorquePowerSensor( sensorID ) )
            {
                result                 = POWER_METER;
                deltaEventCount        = words[ counter++ ].toUInt();      // 3
                deltaWheelPeriod       = words[ counter++ ].toUInt();      // 4
                deltaAccumulatedTorque = words[ counter++ ].toUInt();      // 5
                instantaneousCadence   = words[ counter++ ].toUInt();      // 6
                wheelTicks             = words[ counter++ ].toUInt();      // 7
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Delta Event Count", deltaEventCount );
                    appendDiagnosticsLine( "Delta Wheel Period", deltaWheelPeriod );
                    appendDiagnosticsLine( "Delta Accumulated Torque Ticks", deltaAccumulatedTorque );
                    appendDiagnosticsLine( "Instantaneous Cadence", instantaneousCadence );
                    appendDiagnosticsLine( "Wheel Ticks", wheelTicks );
                }

                if ( powerTable.count( sensorID ) == 0 )
                {
                   appendPowerSensor( sensorID );
                }
            }
        }

        if ( result == POWER_METER )
        {
            double       power              = getPower( sensorID );
            unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );
            unsigned int cadence            = getCadence( sensorID );
            double       torque             = getTorque( sensorID );
            double       speed              = 0;
            double       wheelCircumference = 0;
            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                speed                       = getSpeed( sensorID );
                wheelCircumference          = getWheelCircumference( sensorID );
            }

            if ( nbWords > counter )
            {
                curVersion = words[ counter++ ];
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Version", curVersion );
                }
            }

            createOutputHeader( sensorID, timeStampBuffer );
            createPWRB11ResultString
            (
                power,
                cadence,
                torque,
                zeroTimeCount,
                deltaEventCount,
                deltaWheelPeriod,
                deltaAccumulatedTorque,
                instantaneousCadence,
                wheelTicks,
                isUsedAsSpeedSensor( sensorID ),
                speed,
                wheelCircumference
            );
            appendOutputFooter( curVersion );

            setPower  ( sensorID, power );
            setTorque ( sensorID, torque );
            setCadence( sensorID, cadence );
            setZeroTimeCount( sensorID, zeroTimeCount );
            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                setSpeed( sensorID, speed );
            }
        }

        if ( result == OTHER_DEVICE )
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

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB12: Crank Torque Power Meter
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB12
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
// If semiCookedOut == true
//     "PWRB12_<device_ID> <eventCount> <accum_wheel_period> <accum_torque> <inst_cadence>"
// Otherwise,
//     "PWRB12_<device_ID> <sub_second_timer> <power> <inst_cadence>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB12
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result                 = OTHER_DEVICE;
    amString     sensorID               = amString( C_CT_POWER_DEVICE_HEAD ) + deviceIDNo;
    unsigned int eventCount             = 0;
    unsigned int crankTicks             = 0;
    unsigned int instantaneousCadence   = 0;
    unsigned int crankPeriod            = 0;
    unsigned int accumulatedTorque      = 0;
    unsigned int deltaEventCount        = 0;
    unsigned int totalEventCount        = 0;
    unsigned int deltaCrankPeriod       = 0;
    unsigned int deltaAccumulatedTorque = 0;
    unsigned int rollOver               = 0;
    bool         rollOverHappened       = false;
    bool         left                   = false;
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };

    if ( powerTable.count( sensorID ) == 0 )
    {
       appendPowerSensor( sensorID );
    }

    if ( isRegisteredDevice( sensorID ) )
    {
        result = POWER_METER;


        // - - - - - - - - - - - - - - - -
        // Event Count
        eventCount        = hex2Int( payLoad[ 1 ] );
        crankPeriod       = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );
        accumulatedTorque = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );
        rollOver          = 256;
        if ( isLeftCrankEvent( sensorID, eventCount, rollOver ) )
        {
            left                        = true;
            deltaEventCount             = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, eventCount );
            totalEventCount             = totalCountTable[ sensorID ] + deltaEventCount;
            totalCountTable[ sensorID ] = totalEventCount;
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Event Count", deltaEventCount, auxBuffer );
                if ( rollOverHappened )
                {
                    sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                }
                appendDiagnosticsLine( "Delta Event Count", deltaEventCount, auxBuffer );
                appendDiagnosticsLine( "Total Event Count", totalEventCount );
            }

            // - - - - - - - - - - - - - - - -
            // Crank Period
            rollOver         = 65536;  // 256^2
            deltaCrankPeriod = getDeltaInt( rollOverHappened, sensorID, rollOver, crankOrWheelPeriodTable, crankPeriod );
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Accumulated Crank Period", payLoad[ 5 ], payLoad[ 4 ], crankPeriod );
                *auxBuffer = 0;
                if ( rollOverHappened )
                {
                    sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                }
                appendDiagnosticsLine( "Delta Accumulated Crank Period", deltaCrankPeriod, auxBuffer );
            }

            // - - - - - - - - - - - - - - - -
            // Accumulated Torque
            rollOver               = 65536;  // 256^2
            deltaAccumulatedTorque = getDeltaInt( rollOverHappened, sensorID, rollOver, accumulatedTorqueTable, accumulatedTorque );
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Accumulated Torque", payLoad[ 7 ], payLoad[ 6 ], accumulatedTorque );
                *auxBuffer = 0;
                if ( rollOverHappened )
                {
                    sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                }
                appendDiagnosticsLine( "Delta Accumulated Torque", deltaAccumulatedTorque, auxBuffer );
            }
        }
        else
        {
            deltaEventCount                  = getDeltaInt( rollOverHappened, sensorID, rollOver, otherEventCountTable, eventCount );
            totalEventCount                  = otherTotalCountTable[ sensorID ] + deltaEventCount;
            otherTotalCountTable[ sensorID ] = totalEventCount;
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Event Count", deltaEventCount, auxBuffer );
                if ( rollOverHappened )
                {
                    sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                }
                appendDiagnosticsLine( "Delta Event Count (other)", deltaEventCount, auxBuffer );
                appendDiagnosticsLine( "Total Event Count (other)", totalEventCount );
            }

            // - - - - - - - - - - - - - - - -
            // Crank Period
            rollOver         = 65536;  // 256^2
            deltaCrankPeriod = getDeltaInt( rollOverHappened, sensorID, rollOver, otherCrankOrWheelPeriodTable, crankPeriod );
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Accumulated Crank Period", payLoad[ 5 ], payLoad[ 4 ], crankPeriod );
                *auxBuffer = 0;
                if ( rollOverHappened )
                {
                    sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                }
                appendDiagnosticsLine( "Delta Accumulated Crank Period (other)", deltaCrankPeriod, auxBuffer );
            }

            // - - - - - - - - - - - - - - - -
            // Accumulated Torque
            rollOver               = 65536;  // 256^2
            deltaAccumulatedTorque = getDeltaInt( rollOverHappened, sensorID, rollOver, otherAccumulatedTorqueTable, accumulatedTorque );
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Accumulated Torque", payLoad[ 7 ], payLoad[ 6 ], accumulatedTorque );
                *auxBuffer = 0;
                if ( rollOverHappened )
                {
                    sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
                }
                appendDiagnosticsLine( "Delta Accumulated Torque (other)", deltaAccumulatedTorque, auxBuffer );
            }
        }


        // - - - - - - - - - - - - - - - -
        // Crank Ticks
        crankTicks = hex2Int( payLoad[ 2 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Crank Ticks", payLoad[ 2 ], crankTicks );
        }


        // - - - - - - - - - - - - - - - -
        // Instantaneous Cadence
        instantaneousCadence = hex2Int( payLoad[ 3 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Instantaneous Cadence", payLoad[ 3 ], instantaneousCadence );
        }
    }

    if ( result == POWER_METER )
    {
        double       power              = getPower( sensorID );
        unsigned int cadence            = getCadence( sensorID );
        unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );
        double       torque             = getTorque( sensorID );
        double       speed              = 0;
        double       wheelCircumference = 0;
        double       gearRatio          = 0;

        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            speed              = getSpeed( sensorID );
            wheelCircumference = getWheelCircumference( sensorID );
            gearRatio          = getNbMagnets( sensorID );
        }

        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB12ResultString
        (
            power,
            cadence,
            torque,
            zeroTimeCount,
            eventCount,
            deltaEventCount,
            deltaCrankPeriod,
            deltaAccumulatedTorque,
            instantaneousCadence,
            crankTicks,
            isUsedAsSpeedSensor( sensorID ),
            speed,
            wheelCircumference,
            gearRatio
        );
        appendOutputFooter( getVersion() );
        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            setSpeed( sensorID, speed );
        }

        setPower  ( sensorID, power );
        setTorque ( sensorID, torque );
        setCadence( sensorID, cadence );
        setZeroTimeCount( sensorID, zeroTimeCount );
    }

    return result;
}

amDeviceType antPowerProcessing::processPowerMeterB12SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords                = words.split( inputBuffer );
        amString      sensorID;
        amString      curVersion             = getVersion();
        amString      timeStampBuffer;
        amString      semiCookedString;
        unsigned int  counter                = 0;
        unsigned int  eventCount             = 0;
        unsigned int  deltaEventCount        = 0;
        unsigned int  deltaCrankPeriod       = 0;
        unsigned int  deltaAccumulatedTorque = 0;
        unsigned int  instantaneousCadence   = 0;
        unsigned int  crankTicks             = 0;

        if ( nbWords > 7 )
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
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isCrankTorquePowerSensor( sensorID ) )
            {
                // -----------------------------------------
                // Create output string
                result                 = POWER_METER;
                deltaEventCount        = words[ counter++ ].toUInt();        // 3
                deltaCrankPeriod       = words[ counter++ ].toUInt();        // 4
                deltaAccumulatedTorque = words[ counter++ ].toUInt();        // 5
                instantaneousCadence   = words[ counter++ ].toUInt();        // 6
                crankTicks             = words[ counter++ ].toUInt();        // 7
                if ( ( nbWords > 8 ) && words[ counter ].isUnsignedInteger() )
                {
                    eventCount         = words[ counter++ ].toUInt();        // 8
                }
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Delta Event Count", deltaEventCount );
                    appendDiagnosticsLine( "Delta Crank Period", deltaCrankPeriod );
                    appendDiagnosticsLine( "Delta Accumulated Torque", deltaAccumulatedTorque );
                    appendDiagnosticsLine( "Instantaneous Cadence", instantaneousCadence );
                    appendDiagnosticsLine( "Crank Ticks", crankTicks );
                    appendDiagnosticsLine( "Event Count", eventCount );
                }

                if ( powerTable.count( sensorID ) == 0 )
                {
                   appendPowerSensor( sensorID );
                }
            }
        }

        if ( result == POWER_METER )
        {
            double       power              = getPower( sensorID );
            unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );
            unsigned int cadence            = getCadence( sensorID );
            double       torque             = getTorque( sensorID );
            double       speed              = 0;
            double       wheelCircumference = 0;
            double       gearRatio          = 0;

            if ( nbWords > counter )
            {
                curVersion = words[ counter++ ];
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Version", curVersion );
                }
            }

            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                speed              = getSpeed( sensorID );
                wheelCircumference = getWheelCircumference( sensorID );
                gearRatio          = getNbMagnets( sensorID );
            }

            createOutputHeader( sensorID, timeStampBuffer );
            createPWRB12ResultString
            (
                power,
                cadence,
                torque,
                zeroTimeCount,
                eventCount,
                deltaEventCount,
                deltaCrankPeriod,
                deltaAccumulatedTorque,
                instantaneousCadence,
                crankTicks,
                isUsedAsSpeedSensor( sensorID ),
                speed,
                wheelCircumference,
                gearRatio
            );
            appendOutputFooter( curVersion );
            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                setSpeed( sensorID, speed );
            }

            setPower  ( sensorID, power );
            setTorque ( sensorID, torque );
            setCadence( sensorID, cadence );
            setZeroTimeCount( sensorID, zeroTimeCount );
        }

        if ( result == OTHER_DEVICE )
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

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB13: Power Meter Pedal Smoothness and Efficiency
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
bool antPowerProcessing::createB13ResultString
(
    const amSplitString &words
)
{
    bool         result                      = false;
    unsigned int nbWords                     = words.size();
    unsigned int counter                     = 0;
    unsigned int rawLeftTorqueEffectiveness  = 0;
    unsigned int rawRightTorqueEffectiveness = 0;
    unsigned int rawLeftPedalSmoothness      = 0;
    unsigned int rawRightPedalSmoothness     = 0;
    unsigned int deltaEventCount             = 0;
    unsigned int totalEventCount             = 0;
    amString     curVersion                  = getVersion();
    amString     sensorID;
    amString     timeStampBuffer;
    amString     semiCookedString;

    if ( nbWords > 7 )
    {
        sensorID         = words[ counter++ ];                            // 0
        timeStampBuffer  = words[ counter++ ];                            // 1
        semiCookedString = words[ counter++ ];                            // 2
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }
        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterPedalSmoothness( sensorID ) )
        {
            result                      = true;
            rawLeftTorqueEffectiveness  = words[ counter++ ].toUInt();     //  3
            rawRightTorqueEffectiveness = words[ counter++ ].toUInt();     //  4
            rawLeftPedalSmoothness      = words[ counter++ ].toUInt();     //  5
            rawRightPedalSmoothness     = words[ counter++ ].toUInt();     //  6
            if ( words[ counter ].isUnsignedInteger() )
            {
                // Previous Format did not have deltaEventCount as 7th output item (bridge2txt s/w version instead)
                deltaEventCount         = words[ counter++ ].toUInt();     //  7
            }
            totalEventCount             = totalCountTable[ sensorID ] + deltaEventCount;
            totalCountTable[ sensorID ] = totalEventCount;

            if ( diagnostics )
            {
                appendDiagnosticsLine( "Raw Left Torque Effectiveness", rawLeftTorqueEffectiveness );
                appendDiagnosticsLine( "Raw Right Torque Effectiveness", rawRightTorqueEffectiveness );
                appendDiagnosticsLine( "Raw Left Pedal Smoothness", rawLeftPedalSmoothness );
                appendDiagnosticsLine( "Raw Right Pedal Smoothness", rawRightPedalSmoothness );
            }
        }
    }

    if ( result )
    {
        if ( nbWords > counter )
        {
            curVersion = words[ counter++ ];                      //  8
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }

        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB13ResultString
        (
            rawLeftTorqueEffectiveness,
            rawRightTorqueEffectiveness,
            rawLeftPedalSmoothness,
            rawRightPedalSmoothness,
            deltaEventCount,
            totalEventCount
        );
        appendOutputFooter( curVersion );
    }

    return result;
}

amDeviceType antPowerProcessing::processPowerMeterB13
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result                      = POWER_METER;
    amString     sensorID                    = amString( C_PM_PEDAL_SMOOTH_HEAD ) + deviceIDNo;
    unsigned int eventCount                  = 0;
    unsigned int rawLeftTorqueEffectiveness  = 0;
    unsigned int rawRightTorqueEffectiveness = 0;
    unsigned int rawLeftPedalSmoothness      = 0;
    unsigned int rawRightPedalSmoothness     = 0;
    unsigned int deltaEventCount             = 0;
    unsigned int totalEventCount             = 0;
    unsigned int rollOver                    = 0;
    bool         rollOverHappened            = false;
    bool         left                        = false;
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };

    if ( ( eventCountTable.count( sensorID ) == 0 ) || ( totalCountTable.count( sensorID ) == 0 ) )
    {
        appendPowerSensor( sensorID );
    }

    eventCount                  = hex2Int( payLoad[ 1 ] );
    rawLeftTorqueEffectiveness  = hex2Int( payLoad[ 2 ] );
    rawRightTorqueEffectiveness = hex2Int( payLoad[ 3 ] );
    rawLeftPedalSmoothness      = hex2Int( payLoad[ 4 ] );
    rawRightPedalSmoothness     = hex2Int( payLoad[ 5 ] );

    // - - - - - - - - - - - - - - - -
    // Event Count
    rollOver = 256;
    if ( isLeftCrankEvent( sensorID, eventCount, rollOver ) )
    {
        left                        = true;
        deltaEventCount             = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, eventCount );
        totalEventCount             = totalCountTable[ sensorID ] + deltaEventCount;
        totalCountTable[ sensorID ] = totalEventCount;
    }
    else
    {
        deltaEventCount                  = getDeltaInt( rollOverHappened, sensorID, rollOver, otherEventCountTable, eventCount );
        totalEventCount                  = otherTotalCountTable[ sensorID ] + deltaEventCount;
        otherTotalCountTable[ sensorID ] = totalEventCount;
    }

    if ( diagnostics )
    {
        appendDiagnosticsLine( "Event Count", payLoad[ 1 ], eventCount );
        *auxBuffer = 0;
        if ( rollOverHappened )
        {
            sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
        }
        appendDiagnosticsLine( "Delta Event Count", deltaEventCount, auxBuffer );
        appendDiagnosticsLine( "Raw Left Torque Effectiveness", payLoad[ 2 ], rawLeftTorqueEffectiveness );
        appendDiagnosticsLine( "Raw Right Torque Effectiveness", payLoad[ 3 ], rawRightTorqueEffectiveness );
        appendDiagnosticsLine( "Raw Left Pedal Smoothness", payLoad[ 4 ], rawLeftPedalSmoothness );
        appendDiagnosticsLine( "Raw Right Pedal Smoothness", payLoad[ 5 ], rawRightPedalSmoothness );
    }

    if ( result == POWER_METER )
    {
        createOutputHeader( sensorID, timeStampBuffer );
        createPWRB13ResultString
        (
            rawLeftTorqueEffectiveness,
            rawRightTorqueEffectiveness,
            rawLeftPedalSmoothness,
            rawRightPedalSmoothness,
            deltaEventCount,
            totalEventCount
        );
        appendOutputFooter( getVersion() );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB13SemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
//     "PWRB13_<device_ID> <voltage_times_256> <status> <cumulative_operating_time> <resolution>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB13SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords      = words.split( inputBuffer );
        bool          resultCreate = false;

        if ( nbWords > 3 )
        {
            resultCreate = createB13ResultString( words );
        }

        if ( resultCreate )
        {
            result = POWER_METER;
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

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB20: Crank Torque Frequency Power Meter
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antPowerProcessing::processPowerMeterB20
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result               = OTHER_DEVICE;
    amString     sensorID             = amString( C_CTF_POWER_DEVICE_HEAD ) + deviceIDNo;
    unsigned int deltaTimeStamp       = 0;
    unsigned int deltaEventCount      = 0;
    unsigned int deltaTorqueTicks     = 0;
    unsigned int factorySlope_Nm_10Hz = 0;
    unsigned int timeStamp            = 0;
    unsigned int eventCount           = 0;
    unsigned int torqueTicks          = 0;
    unsigned int rollOver             = 0;
    bool         rollOverHappened     = false;
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };

    if ( isRegisteredDevice( sensorID ) )
    {
        result = POWER_METER;

        // - - - - - - - - - - - - - - - -
        // Event Count
        eventCount      = hex2Int( payLoad[ 1 ] );
        rollOver        = 256;
        deltaEventCount = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, eventCount );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Event Count", payLoad[ 1 ], eventCount );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Event Count", deltaEventCount, auxBuffer );
        }
        if ( eventCountTable[ sensorID ] == eventCount )
        {
            sameEventCountTable[ sensorID ]++;
        }
        else
        {
            sameEventCountTable[ sensorID ] = 0;
        }


        // - - - - - - - - - - - - - - - -
        // Factory Slope (Nm/10Hz)
        factorySlope_Nm_10Hz = hex2Int( payLoad[ 2 ], payLoad[ 3 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Factory Slope", payLoad[ 2 ], payLoad[ 3 ], factorySlope_Nm_10Hz, " (Nm/10Hz)" );
        }


        // - - - - - - - - - - - - - - - -
        // Time Stamp
        timeStamp      = hex2Int( payLoad[ 4 ], payLoad[ 5 ] );
        rollOver       = 65536;  // 256^2
        deltaTimeStamp = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, timeStamp );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Time Stamp", payLoad[ 4 ], payLoad[ 5 ], timeStamp );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Time Stamp", deltaTimeStamp, auxBuffer );
        }


        // - - - - - - - - - - - - - - - -
        // Accumulated Torque Ticks
        torqueTicks      = hex2Int( payLoad[ 6 ], payLoad[ 7 ] );
        rollOver         = 65536;  // 256^2
        deltaTorqueTicks = getDeltaInt( rollOverHappened, sensorID, rollOver, accumulatedTorqueTable, torqueTicks );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Torque Ticks", payLoad[ 6 ], payLoad[ 7 ], torqueTicks );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Torque Ticks", deltaTorqueTicks, auxBuffer );
        }
    }

    if ( result == POWER_METER )
    {
        double       power              = getPower( sensorID );
        unsigned int cadence            = getCadence( sensorID );
        double       torque             = getTorque( sensorID );
        unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );
        unsigned int powerMeterOffset   = getOffset( sensorID );
        unsigned int userDefinedSlope   = getSlope_Nm_10Hz( sensorID );
        double       speed              = 0;
        double       wheelCircumference = 0;
        double       gearRatio          = 0;

        createOutputHeader( sensorID, timeStampBuffer );
        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            speed              = getSpeed( sensorID );
            wheelCircumference = getWheelCircumference( sensorID );
            gearRatio          = getNbMagnets( sensorID );
        }

        createPWRB20ResultString
        (
            power,
            cadence,
            torque,
            zeroTimeCount,
            deltaEventCount,
            deltaTimeStamp,
            deltaTorqueTicks,
            factorySlope_Nm_10Hz,
            powerMeterOffset,
            userDefinedSlope,
            isUsedAsSpeedSensor( sensorID ),
            speed,
            wheelCircumference,
            gearRatio
        );

        if ( isUsedAsSpeedSensor( sensorID ) )
        {
            setSpeed( sensorID, speed );
        }

        appendOutputFooter( getVersion() );

        setPower  ( sensorID, power );
        setTorque ( sensorID, torque );
        setCadence( sensorID, cadence );
        setZeroTimeCount( sensorID, zeroTimeCount );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB20SemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
//     "PWRB20_<device_ID> <event_count> <slope> <time_stamp> <torque_ticks>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB20SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords              = words.split( inputBuffer );
        amString      sensorID;
        amString      timeStampBuffer;
        amString      curVersion           = getVersion();
        amString      semiCookedString;
        unsigned int  deltaEventCount      = 0;
        unsigned int  deltaTimeStamp       = 0;
        unsigned int  deltaTorqueTicks     = 0;
        unsigned int  factorySlope_Nm_10Hz = 0;
        unsigned int  counter              = 0;

        if ( nbWords > 6 )
        {
            sensorID         = words[ counter++ ];                          // 0
            timeStampBuffer  = words[ counter++ ];                          // 1
            semiCookedString = words[ counter++ ];                          // 2
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isCrankTorqueFrequencyPowerSensor( sensorID ) )
            {
                result               = POWER_METER;
                deltaEventCount      = words[ counter++ ].toUInt();      // 3
                deltaTimeStamp       = words[ counter++ ].toUInt();      // 4
                deltaTorqueTicks     = words[ counter++ ].toUInt();      // 5
                factorySlope_Nm_10Hz = words[ counter++ ].toUInt();      // 6
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Delta Event Count", deltaEventCount );
                    appendDiagnosticsLine( "Delta Time Stamp", deltaTimeStamp );
                    appendDiagnosticsLine( "Delta Torque Ticks", deltaTorqueTicks );
                    appendDiagnosticsLine( "Factory Slope (Nm/10Hz)", factorySlope_Nm_10Hz );
                }

                if ( powerTable.count( sensorID ) == 0 )
                {
                   appendPowerSensor( sensorID );
                }
            }
        }

        if ( result == POWER_METER )
        {
            double       power              = getPower( sensorID );
            unsigned int cadence            = getCadence( sensorID );
            unsigned int zeroTimeCount      = getZeroTimeCount( sensorID );
            double       torque             = getTorque( sensorID );
            unsigned int powerMeterOffset   = getOffset( sensorID );
            unsigned int userDefinedSlope   = getSlope_Nm_10Hz( sensorID );
            double       speed              = 0;
            double       wheelCircumference = 0;
            double       gearRatio          = 0;

            if ( nbWords > counter )
            {
                curVersion = words[ counter++ ];
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Version", curVersion );
                }
            }

            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                speed              = getSpeed( sensorID );
                wheelCircumference = getWheelCircumference( sensorID );
                gearRatio          = getNbMagnets( sensorID );
            }

            createOutputHeader( sensorID, timeStampBuffer );
            createPWRB20ResultString
            (
                power,
                cadence,
                torque,
                zeroTimeCount,
                deltaEventCount,
                deltaTimeStamp,
                deltaTorqueTicks,
                factorySlope_Nm_10Hz,
                powerMeterOffset,
                userDefinedSlope,
                isUsedAsSpeedSensor( sensorID ),
                speed,
                wheelCircumference,
                gearRatio
            );
            appendOutputFooter( curVersion );

            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                setSpeed( sensorID, speed );
            }

            setPower  ( sensorID, power );
            setTorque ( sensorID, torque );
            setCadence( sensorID, cadence );
            setZeroTimeCount( sensorID, zeroTimeCount );
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


//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB46: Power Meter Request Data Page (Optional Common Page)
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
bool antPowerProcessing::createB46ResultString
(
    const amSplitString &words
)
{
    bool         result            = false;
    unsigned int nbWords           = words.size();
    unsigned int counter           = 0;
    unsigned int descriptor1       = 0;
    unsigned int descriptor2       = 0;
    unsigned int requestedResponse = 0;
    unsigned int requestedPageNo   = 0;
    unsigned int commandType       = 0;
    amString     curVersion        = getVersion();
    amString     sensorID;
    amString     timeStampBuffer;
    amString     semiCookedString;
    bool         outputPageNo      = false;  // The page number is already mentioned in the meassage header (PWBR46)

    if ( nbWords > 7 )
    {
        sensorID         = words[ counter++ ];                            // 0
        timeStampBuffer  = words[ counter++ ];                            // 1
        semiCookedString = words[ counter++ ];                            // 2
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }

        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterRequest( sensorID ) )
        {
            result            = true;
            descriptor1       = words[ counter++ ].toUInt();               //  3
            descriptor2       = words[ counter++ ].toUInt();               //  4
            requestedResponse = words[ counter++ ].toUInt();               //  5
            requestedPageNo   = words[ counter++ ].toUInt();               //  6
            commandType       = words[ counter++ ].toUInt();               //  7
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Descriptor Byte 1", descriptor1 );
                appendDiagnosticsLine( "Descriptor Byte 2", descriptor2 );
                appendDiagnosticsLine( "Requested Response", requestedResponse );
                appendDiagnosticsLine( "Requested Page No", requestedPageNo );
                appendDiagnosticsLine( "Command Type", commandType );
            }
        }
    }

    if ( result )
    {
        createOutputHeader( sensorID, timeStampBuffer );
        createCommonResultStringPage70( sensorID, outputPageNo, descriptor1, descriptor2, requestedResponse, requestedPageNo, commandType );
        if ( nbWords > counter )
        {
            curVersion = words[ counter++ ];                      //  8
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        appendOutputFooter( curVersion );
    }
    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB46
//
// This is the Common Page 0x46 (70) for a Power Meter
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB46
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result            = POWER_METER;
    amString     sensorID          = amString( C_PM_REQUEST_HEAD ) + deviceIDNo;
    unsigned int descriptor1       = hex2Int( payLoad[ 3 ] );
    unsigned int descriptor2       = hex2Int( payLoad[ 4 ] );
    unsigned int requestedResponse = hex2Int( payLoad[ 5 ] );
    unsigned int requestedPageNo   = hex2Int( payLoad[ 6 ] );
    unsigned int commandType       = hex2Int( payLoad[ 7 ] );
    bool         outputPageNo      = false;  // The page number is already mentioned in the meassage header (PWBR46)

    // ---------------------------------------------------------------
    // Payload
    //   0  Command ID (0x46)
    //   1  Reserved
    //   2  Reserved
    //   3  Descriptor byte 1
    //   4  Descriptor byte 2
    //   5  Requested Transmission Response
    //   6  Requested Page Number
    //   7  Command Type

    if ( diagnostics )
    {
        appendDiagnosticsLine( "Descriptor Byte 1", payLoad[ 3 ], descriptor1 );
        appendDiagnosticsLine( "Descriptor Byte 2", payLoad[ 4 ], descriptor2 );
        appendDiagnosticsLine( "Requested Response", payLoad[ 5 ], requestedResponse );
        appendDiagnosticsLine( "Requested Page No", payLoad[ 6 ], requestedPageNo );
        appendDiagnosticsLine( "Command Type", payLoad[ 7 ], commandType );
    }

    if ( result == POWER_METER )
    {
        createOutputHeader( sensorID, timeStampBuffer );
        createCommonResultStringPage70( sensorID, outputPageNo, descriptor1, descriptor2, requestedResponse, requestedPageNo, commandType );
        appendOutputFooter( getVersion() );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB46SemiCooked
//
// This is the Common Page 0x46 (70) for a Power Meter
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB46SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords      = words.split( inputBuffer );
        bool          resultCreate = false;

        if ( nbWords > 3 )
        {
            resultCreate = createB46ResultString( words );
        }

        if ( resultCreate )
        {
            result = POWER_METER;
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

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB50: Power Meter - Manifucturer's Identification (Mandatory Common Page)
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
bool antPowerProcessing::createB50ResultString
(
    const amSplitString &words
)
{
    amString     sensorID;
    amString     semiCookedString;
    amString     timeStampBuffer;
    amString     curVersion       = getVersion();
    bool         result           = false;
    unsigned int nbWords          = words.size();
    unsigned int counter          = 0;
    unsigned int manufacturerID   = 0;
    unsigned int hardwareRevision = 0;
    unsigned int modelNumber      = 0;
    bool         outputPageNo     = false;  // The page number is already mentioned in the meassage header (PWBR50)

    if ( nbWords > 5 )
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
        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterManufacturerInfo( sensorID ) )
        {
            result           = true;
            manufacturerID   = words[ counter++ ].toUInt();    // 3
            hardwareRevision = words[ counter++ ].toUInt();    // 4
            modelNumber      = words[ counter++ ].toUInt();    // 5
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Model Number", modelNumber );
                appendDiagnosticsLine( "Hardware Revision", hardwareRevision );
                appendDiagnosticsLine( "Manufacturer ID", manufacturerID );
            }
            createOutputHeader( sensorID, timeStampBuffer );
            createCommonResultStringPage80( sensorID, outputPageNo, manufacturerID, hardwareRevision, modelNumber );
        }
    }

    if ( result )
    {
        if ( nbWords > counter )
        {
            curVersion = words[ counter++ ];                          //  6
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        appendOutputFooter( curVersion );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB50
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
// If semiCookedOut == true
//     "PWRB50_<device_ID> <manufacturer_ID> <model_number> <hardware_revision>"
// Otherwise,
//     "PWRB50_<device_ID> <timeStampBuffer> <manufacturer_ID> <model_number> <hardware_revision>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB50
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result           = POWER_METER;
    unsigned int hardwareRevision = hex2Int( payLoad[ 3 ] );
    unsigned int manufacturerID   = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );
    unsigned int modelNumber      = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );
    amString     sensorID         = amString( C_PM_MFR_INFO_HEAD ) + deviceIDNo;
    bool         outputPageNo     = false;  // The page number is already mentioned in the meassage header (PWBR50)

    if ( diagnostics )
    {
        appendDiagnosticsLine( "Model Number", payLoad[ 7 ], payLoad[ 6 ], modelNumber );
        appendDiagnosticsLine( "Manufacturer ID", payLoad[ 5 ], payLoad[ 4 ], manufacturerID );
        appendDiagnosticsLine( "Hardware Revision", payLoad[ 3 ], hardwareRevision );
    }

    createOutputHeader( sensorID, timeStampBuffer );
    createCommonResultStringPage80( sensorID, outputPageNo, manufacturerID, hardwareRevision, modelNumber );
    appendOutputFooter( getVersion() );

    return result;
}


amDeviceType antPowerProcessing::processPowerMeterB50SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords      = words.split( inputBuffer );
        bool          resultCreate = false;

        if ( nbWords > 3 )
        {
            resultCreate = createB50ResultString( words );
        }

        if ( resultCreate )
        {
            result = POWER_METER;
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


//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB51: Power Meter - Procduct Information (Mandatory Common Page)
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
bool antPowerProcessing::createB51ResultString
(
    const amSplitString &words
)
{
    amString     sensorID;
    amString     semiCookedString;
    amString     timeStampBuffer;
    amString     curVersion       = getVersion();
    bool         result           = false;
    unsigned int nbWords          = words.size();
    unsigned int counter          = 0;
    unsigned int serialNumber     = 0;
    unsigned int softwareRevision = 0;
    bool         outputPageNo     = false;  // The page number is already mentioned in the meassage header (PWBR51)

    if ( nbWords > 4 )
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
        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterProductInfo( sensorID ) )
        {
            result           = true;
            serialNumber     = words[ counter++ ].toUInt();    // 3
            softwareRevision = words[ counter++ ].toUInt();    // 4
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Serial Number", serialNumber );
                appendDiagnosticsLine( "Software Revision", softwareRevision );
            }
            createOutputHeader( sensorID, timeStampBuffer );
            createCommonResultStringPage81( sensorID, outputPageNo, serialNumber, softwareRevision );
        }
    }

    if ( result )
    {
        if ( nbWords > counter )
        {
            curVersion = words[ counter++ ];                          //  5
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        appendOutputFooter( curVersion );
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB51
//
// Convert the raw ant data into text data and put the result string into the outBuffer.
// The output string has the form
//     "PWRB51_<device_ID> <seconds> payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ], <serialNumber>"
//
// Parameters:
//    const amString &deviceIDNo        IN   Device ID (number).
//    const amString &timeStampBuffer   IN   Time stamp.
//    BYTE            payLoad[]         IN   Array of bytes with the data to be converted.
//
// The output string has the form
// If semiCookedOut == true
//     "PWRB51_<device_ID> <serial_number> <software_revsion>"
// Otherwise,
//     "PWRB51_<device_ID> <timeStampBuffer> <serial_number> <software_revsion>"
//
// Return amDeviceType POWER_METER if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB51
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result           = POWER_METER;
    amString     sensorID         = amString( C_PM_PROD_INFO_HEAD ) + deviceIDNo;
    unsigned int serialNumber     = hex2Int( payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ] );
    unsigned int softwareRevision = hex2Int( payLoad[ 3 ] );
    bool         outputPageNo     = false;  // The page number is already mentioned in the meassage header (PWBR51)

    if ( diagnostics )
    {
        appendDiagnosticsLine( "Serial Number", payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ], serialNumber );
        appendDiagnosticsLine( "Software Revision", payLoad[ 3 ], softwareRevision );
    }

    createOutputHeader( sensorID, timeStampBuffer );
    createCommonResultStringPage81( sensorID, outputPageNo, serialNumber, softwareRevision );
    appendOutputFooter( getVersion() );

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB51SemiCooked
//
// Convert the semi-cooked ant data into fully cooked text data and put the result string into the outBuffer.
// The output string has the form
//     "PWRB51_<device_ID> <timeStampBuffer> <serial_number> <software_revsion>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB51SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords      = words.split( inputBuffer );
        bool          resultCreate = false;

        if ( nbWords > 3 )
        {
            resultCreate = createB51ResultString( words );
        }

        if ( resultCreate )
        {
            result = POWER_METER;
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

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// PWRB52: Power Meter - Battery Status (Optional Common Page)
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
bool antPowerProcessing::createB52ResultString
(
    const amSplitString &words
)
{
    amString     timeStampBuffer;
    amString     sensorID;
    amString     semiCookedString;
    amString     curVersion         = getVersion();
    bool         result             = false;
    unsigned int nbWords            = words.size();
    unsigned int nbBatteries        = 0;
    unsigned int batteryID          = 0;
    unsigned int voltage256         = 0;
    unsigned int status             = 0;
    unsigned int resolution         = 0;
    unsigned int deltaOperatingTime = 0;
    unsigned int counter            = 0;
    bool         commonPage         = false;
    bool         outputPageNo       = false;  // The page number is already mentioned in the meassage header (PWBR52)

    if ( nbWords >= 8 )
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
        if ( ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterStatusMessage( sensorID ) )
        {
            result             = true;
            voltage256         = words[ counter++ ].toUInt();          // 3
            status             = words[ counter++ ].toUInt();          // 4
            deltaOperatingTime = words[ counter++ ].toUInt();          // 5
            resolution         = words[ counter++ ].toUInt();          // 6

            if ( diagnostics )
            {
                appendDiagnosticsLine( "Voltage * 256", voltage256 );
                appendDiagnosticsLine( "Status", status );
                appendDiagnosticsLine( "Delta Operating Time", deltaOperatingTime );
                appendDiagnosticsLine( "Resolution", resolution );
            }
            if ( nbWords >= 10 )
            {
                bool reassign = false;
                nbBatteries   = words[ counter++ ].toUInt();                     // 7
                batteryID     = words[ counter++ ].toUInt();                     // 8
                if ( ( voltage256 == 0 ) && ( status == 0 ) && ( nbBatteries != 0 ) && ( batteryID != 0 ) )
                {
                    reassign = true;
                }
                if ( !reassign && ( voltage256 == 2 ) && ( ( status == 0 ) || ( status == 1 ) ) && ( nbBatteries > 2 ) )
                {
                    reassign = true;
                }
                if ( !reassign && ( voltage256 == 1 ) && ( status == 0 ) && ( nbBatteries > 2 ) )
                {
                    reassign = true;
                }

                if ( reassign )
                {
                    voltage256         = deltaOperatingTime;
                    status             = resolution;
                    deltaOperatingTime = nbBatteries;
                    resolution         = batteryID;
                    nbBatteries        = 0;
                    batteryID          = 0;
                    if ( diagnostics )
                    {
                        appendDiagnosticsLine( "Voltage * 256", voltage256, "\t- REASSIGNED" );
                        appendDiagnosticsLine( "Status", status, "\t- REASSIGNED" );
                        appendDiagnosticsLine( "Delta Operating Time", deltaOperatingTime, "\t- REASSIGNED" );
                        appendDiagnosticsLine( "Resolution", resolution, "\t- REASSIGNED" );
                    }
                }
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Number of Batteries", nbBatteries );
                    appendDiagnosticsLine( "Battery ID", batteryID );
                }
            }
            else
            {
                nbBatteries = 0;
                batteryID   = 0;
            }
        }
    }

    if ( result )
    {
        if ( nbWords > counter )
        {
            curVersion = words[ counter++ ];                      //  9
            if ( diagnostics )
            {
                appendDiagnosticsLine( "Version", curVersion );
            }
        }
        createOutputHeader( sensorID, timeStampBuffer );
        commonPage = createCommonResultStringPage82( sensorID, outputPageNo, voltage256, status, deltaOperatingTime, resolution, nbBatteries, batteryID );
        if ( commonPage )
        {
            result = POWER_METER;
            appendOutputFooter( getVersion() );
        }
        else
        {
            result = OTHER_DEVICE;
        }
    }

    return result;
}

unsigned int antPowerProcessing::splitFormat137_B52
(
    const amString &inputBuffer,
    amSplitString  &outWords
)
{
    unsigned int nbWords = 0;
    if ( !inputBuffer.empty() )
    {
        amSplitString inWords;
        nbWords = inWords.split( inputBuffer );

        if ( nbWords >= 8 )
        {
            // 2 possibilites (Examples):
            //  A  <sensor_id>       <time_stamp>    <S>                  <v_256>  <status>   <oper_time> <resol>      <version>
            //     PWRB52_21557   1452684887.399899   S                     754        1         45208       16      1.34.455-g6555d51
            //  B  PWRB52_54330   1394849709.243736   S     0       0       760        3         1199         2      1.34.484-g23b5005
            //     <sensor_id>       <time_stamp>    <S>  <nb_b>  <b_id>  <v_256>  <status>   <oper_time> <resol>      <version>

            amString     sensorID;
            unsigned int counter            = 0;
            unsigned int operatingTime      = 0;
            unsigned int deltaOperatingTime = 0;
            unsigned int nbBatteries        = 0;
            unsigned int batteryID          = 0;

            sensorID = inWords[ counter++ ];
            outWords.push_back( sensorID );                                // Sensor ID
            outWords.push_back( inWords[ counter++ ] );                    // Time Stamp
            outWords.push_back( inWords[ counter++ ] );                    // Semi-cooked Symbol
            if ( nbWords >= 10 )
            {
                // Case B
                nbBatteries = inWords[ counter++ ].toUInt();               // Nb Batteries
                batteryID   = inWords[ counter++ ].toUInt();               // Battery ID
            }
            outWords.push_back( inWords[ counter++ ] );                    // Voltage256
            outWords.push_back( inWords[ counter++ ] );                    // Status
            operatingTime   = inWords[ counter++ ].toUInt();               // Accumulative Operating Time
            if ( operatingTimeTable.count( sensorID ) == 0 )               //     must be converted to Delta Operating Time
            {
                operatingTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
            }
            deltaOperatingTime = operatingTime - operatingTimeTable[ sensorID ];
            operatingTimeTable[ sensorID ] = operatingTime;
            outWords.push_back( std::to_string( deltaOperatingTime ) );
            outWords.push_back( inWords[ counter++ ] );                    // Resolution (2 or 16)
            outWords.push_back( std::to_string( nbBatteries ) );           // Nb Batteries
            outWords.push_back( std::to_string( batteryID ) );             // Battery ID
            outWords.push_back( inWords[ counter++ ] );                    // Version
        }
        nbWords = outWords.size();
    }

    return nbWords;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB52
//
// Convert the raw ant data into text data and put the result string into the outBuffer.
// The output string has the form
//     "PWRB52_<device_ID> <seconds> <battery_status> <voltage>"
//
// Parameters:
//    const amString &deviceIDNo        IN   Device ID (number).
//    const amString &timeStampBuffer   IN   Time stamp.
//    BYTE            payLoad[]         IN   Array of bytes with the data to be converted.
//
// The output string has the form
// If semiCookedOut == true
//     "PWRB52_<device_ID> <nbBatteries> <batteryID> <voltage_times_256> <battery_status> <operating_time> <resolution>"
// Otherwise,
//     "PWRB52_<device_ID> <timeStampBuffer> <battery_status> <voltage>"
//
// Return amDeviceType POWER_METER if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB52
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    unsigned int deltaOperatingTime = 0;
    unsigned int operatingTime      = 0;
    unsigned int voltage256         = 0;
    unsigned int voltageInt         = 0;
    unsigned int nbBatteries        = 0;
    unsigned int batteryID          = 0;
    unsigned int status             = 0;
    unsigned int resolution         = 0;
    unsigned int rollOver           = 0;
    amDeviceType result             = POWER_METER;
    amString     sensorID           = amString( C_PM_STATUS_MSG_HEAD ) + deviceIDNo;
    bool         rollOverHappened   = false;
    bool         commonPage         = false;
    bool         outputPageNo       = false;  // The page number is already mentioned in the meassage header (PWBR52)
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };

    // Byte 2: Identifies the battery in the system to which this battery status
    //         pertains and specifies how many batteries are available in the system.
    //         Bits 0-3: Number of Batteries
    //         Bits 4-7: Identifier
    //         Set to 0xFF if not used.
    if ( ( totalTimeTable.count( sensorID ) == 0 ) && ( eventTimeTable.count( sensorID ) == 0 ) )
    {
        operatingTimeTable.insert( std::pair<amString, double>( sensorID, 0 ) );
        eventTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
    }

    if ( payLoad[ 2 ] != 0xFF )
    {
        nbBatteries = ( payLoad[ 2 ] & 0x0F );
        batteryID   = ( payLoad[ 2 ] & 0xF0 ) >> 4;
    }
    if ( diagnostics )
    {
        appendDiagnosticsLine( "Number of Batteries", payLoad[ 2 ], nbBatteries, " (Lower 4 bits)" );
        appendDiagnosticsLine( "Battery ID", payLoad[ 2 ], batteryID, " (Upper 4 bits)" );
    }


    voltage256  = hex2Int( payLoad[ 6 ] );
    voltageInt  = hex2Int( payLoad[ 7 ] ) & 0x0F;
    voltage256 += voltageInt * 256;
    if ( diagnostics )
    {
        double voltageDbl  = ( double ) voltage256 / 256.0;
        sprintf( auxBuffer, " (%.3lfV - First Byte Lower 4 bits)", voltageDbl );
        appendDiagnosticsLine( "Battery Voltage * 256", payLoad[ 7 ], payLoad[ 6 ], voltage256, auxBuffer );
    }


    status = ( payLoad[ 7 ] >> 4 ) & 7;
    if ( diagnostics )
    {
        strcpy( auxBuffer, " (bits 4-6)" );
        appendDiagnosticsLine( "Battery Status ID", payLoad[ 7 ], status, auxBuffer );
    }


    resolution        = ( payLoad[ 7 ] & 128 ) ? 16 : 2;
    operatingTime     = hex2Int( payLoad[ 5 ], payLoad[ 4 ], payLoad[ 3 ] );
    rollOver          = 16777216;  // 256^3
    deltaOperatingTime = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, operatingTime );
    if ( diagnostics )
    {
        sprintf( auxBuffer, " (7th bit: [0/1] for [32/2] seconds)" );
        appendDiagnosticsLine( "Resolution", payLoad[ 7 ], resolution, auxBuffer );
        appendDiagnosticsLine( "Cumulative Operating Time", payLoad[ 3 ], payLoad[ 2 ], payLoad[ 1 ], operatingTime );
        *auxBuffer = 0;
        if ( rollOverHappened )
        {
            sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
        }
        appendDiagnosticsLine( "Delta Cumulative Operating Time", deltaOperatingTime, auxBuffer );
    }

    createOutputHeader( sensorID, timeStampBuffer );
    commonPage = createCommonResultStringPage82( sensorID, outputPageNo, voltage256, status, deltaOperatingTime, resolution, nbBatteries, batteryID );
    if ( commonPage )
    {
        result = POWER_METER;
        appendOutputFooter( getVersion() );
    }
    else
    {
        result = OTHER_DEVICE;
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processPowerMeterB52SemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
//     "PWRB52_<device_ID> <voltage_times_256> <status> <cumulative_operating_time> <resolution>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antPowerProcessing::processPowerMeterB52SemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        bool          resultCreate = false;
        unsigned int  nbWords      = 0;
        amSplitString words;

        if ( isSemiCookedFormat137( inputBuffer ) )
        {
            nbWords = splitFormat137_B52( inputBuffer, words );
        }
        else
        {
            nbWords = words.split( inputBuffer );
        }

        if ( nbWords > 3 )
        {
            resultCreate = createB52ResultString( words );
        }

        if ( resultCreate )
        {
            result = POWER_METER;
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

// -------------------------------------------------------------------------------------------------//
// -------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------
//
// processPowerMeter
//
// Determine the subtype of the power meter
//     B01, B02, B03, B10, B11, B12, B13, B20, B46, B50 B51, B52
// call the respective subroutine to convert the raw ant data into text data,
// and put the result string into the outBuffer.
//
// Parameters:
//    const amString &deviceIDNo        IN   Device ID (number).
//    const amString &timeStampBuffer   IN   Time stamp.
//    BYTE            payLoad[]         IN   Array of bytes with the data to be converted.
//
// Return amDeviceType POWER_METER if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
// ---------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------//
// -------------------------------------------------------------------------------------------------//
amDeviceType antPowerProcessing::processPowerMeter
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    unsigned int dataPage           = 0;
    amDeviceType result             = OTHER_DEVICE;
    bool         commonPage         = false;
    bool         outputPageNo       = false;
    amString     sensorID;
    char         auxBuffer[ C_TINY_BUFFER_SIZE ] = { 0 };

    dataPage = hex2Int( payLoad[ 0 ] );
    if ( diagnostics )
    {
        appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
    }

    switch ( dataPage )
    {
        case  1: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x01 = 1: Calibration Request
                 result = processPowerMeterB01( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case  2: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x02 = 2: Power Meter Get/Set Parameters Message (PWRB02)
                 result = processPowerMeterB02( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case  3: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x03 = 3: Power Meter Measurement Output Message (PWRB03)
                 result = processPowerMeterB03( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 16: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x10 = 16: Standard Power-Only Sensor
                 result = processPowerMeterB10( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 17: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x11 = 17: Torque at Wheel Sensor
                 result = processPowerMeterB11( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 18: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x12 = 18: Torque at Crank Sensor
                 result = processPowerMeterB12( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 19: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x13 = 19: Torque at Crank Sensor
                 result = processPowerMeterB13( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 32: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x20 = 32: Crank Torque-Frequency Sensor
                 result = processPowerMeterB20( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 70: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x46 = 70: Power Meter Data Page Request Message (PWRB46)
                 result = processPowerMeterB46( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 80: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x50 = 80: Power Meter Manufacturer's Information Message (PWRB50)
                 result = processPowerMeterB50( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 81: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x51 = 81: Power Meter Product Information Message (PWRB51)
                 result = processPowerMeterB51( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        case 82: // - - - - - - - - - - - - - - - - - - - - - -
                 // 0x52 = 82: Power Meter Battery Voltage Message (PWRB52)
                 result = processPowerMeterB52( deviceIDNo, timeStampBuffer, payLoad );
                 break;
        default: sprintf( auxBuffer, "%2X_", payLoad[ 0 ] );
                 sensorID = amString( C_POWER_DEVICE_HEAD ) + auxBuffer + deviceIDNo;
                 createOutputHeader( sensorID, timeStampBuffer );
                 commonPage = processCommonPages( sensorID, payLoad, outputPageNo );
                 if ( commonPage )
                 {
                     result = POWER_METER;
                     appendOutputFooter( getVersion() );
                 }
                 else
                 {
                     resetOutBuffer();
                     if ( outputUnknown )
                     {
                         int deviceIDNoAsInt = deviceIDNo.toInt();
                         createUnknownDeviceTypeString( C_POWER_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
                     }
                 }
                 break;
    }

    return result;
}

amDeviceType antPowerProcessing::processPowerMeterSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        char          auxBuffer[ C_TINY_BUFFER_SIZE ] = { 0 };
        amSplitString words;
        unsigned int  nbWords            = words.split( inputBuffer );
        unsigned int  dataPage           = 0;
        unsigned int  counter            = 0;
        unsigned int  startCounter       = 0;
        bool          commonPage         = false;
        bool          outputPageNo       = false;
        amString      sensorID;
        amString      timeStampBuffer;
        amString      semiCookedString;

        if ( nbWords > 4 )
        {
            sensorID         = words[ counter++ ];                                      //  0
            timeStampBuffer  = words[ counter++ ];                                      //  1
            semiCookedString = words[ counter++ ];                                      //  2
            if ( diagnostics )
            {
                appendDiagnosticsLine( sensorID );
                appendDiagnosticsLine( timeStampBuffer );
                appendDiagnosticsLine( semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isPowerMeterRelated( sensorID ) )
            {
                startCounter = counter;
                strcpy( auxBuffer, sensorID.c_str() );
                if ( strlen( auxBuffer ) > strlen( C_POWER_DEVICE_HEAD + 2 ) )
                {
                    if ( auxBuffer[ strlen( C_POWER_DEVICE_HEAD ) + 2 ] == '_' )
                    {
                        auxBuffer[ strlen( C_POWER_DEVICE_HEAD ) + 2 ] = 0;
                    }
                    else if ( auxBuffer[ strlen( C_POWER_DEVICE_HEAD ) + 3 ] == '_' )
                    {
                        auxBuffer[ strlen( C_POWER_DEVICE_HEAD ) + 3 ] = 0;
                    }
                    else if ( auxBuffer[ strlen( C_POWER_DEVICE_HEAD ) + 4 ] == '_' )
                    {
                        auxBuffer[ strlen( C_POWER_DEVICE_HEAD ) + 4 ] = 0;
                    }
                    const char *aPtr = auxBuffer + strlen( C_POWER_DEVICE_HEAD );
                    if ( strlen( aPtr ) > 0 )
                    {
                        dataPage = ( *aPtr - '0' );
                        ++aPtr;
                        if ( strlen( aPtr ) > 0 )
                        {
                            dataPage *= 16;
                            dataPage += ( *aPtr - '0' );
                            ++aPtr;
                            if ( strlen( aPtr ) > 0 )
                            {
                                dataPage *= 16;
                                dataPage += ( *aPtr - '0' );
                            }
                        }
                    }
                }
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Page", dataPage );
                }

                switch ( dataPage )
                {
                    case  1: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x01 = 1: Calibration Request
                             result = processPowerMeterB01SemiCooked( inputBuffer );
                             break;
                    case  2: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x02 = 2: Power Meter Get/Set Parameters Message (PWRB02)
                             result = processPowerMeterB02SemiCooked( inputBuffer );
                             break;
                    case  3: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x03 = 3: Power Meter Measurement Output Message (PWRB03)
                             result = processPowerMeterB03SemiCooked( inputBuffer );
                             break;
                    case 16: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x10 = 16: Standard Power-Only Sensor
                             result = processPowerMeterB10SemiCooked( inputBuffer );
                             break;
                    case 17: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x11 = 17: Torque at Wheel Sensor
                             result = processPowerMeterB11SemiCooked( inputBuffer );
                             break;
                    case 18: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x12 = 18: Torque at Crank Sensor
                             result = processPowerMeterB12SemiCooked( inputBuffer );
                             break;
                    case 19: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x13 = 19: Torque at Crank Sensor
                             result = processPowerMeterB13SemiCooked( inputBuffer );
                             break;
                    case 32: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x20 = 32: Crank Torque-Frequency Sensor
                             result = processPowerMeterB20SemiCooked( inputBuffer );
                             break;
                    case 70: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x46 = 70: Power Meter Data Page Request Message (PWRB46)
                             result = processPowerMeterB46SemiCooked( inputBuffer );
                             break;
                    case 80: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x50 = 80: Power Meter Manufacturer's Information Message (PWRB50)
                             result = processPowerMeterB50SemiCooked( inputBuffer );
                             break;
                    case 81: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x51 = 81: Power Meter Product Information Message (PWRB51)
                             result = processPowerMeterB51SemiCooked( inputBuffer );
                             break;
                    case 82: // - - - - - - - - - - - - - - - - - - - - - -
                             // 0x52 = 82: Power Meter Battery Voltage Message (PWRB52)
                             result = processPowerMeterB52SemiCooked( inputBuffer );
                             break;
                    default: createOutputHeader( sensorID, timeStampBuffer );
                             commonPage = processCommonPagesSemiCooked( words, startCounter, dataPage, outputPageNo );
                             if ( commonPage )
                             {
                                 result = POWER_METER;
                                 appendOutputFooter( getVersion() );
                             }
                             else
                             {
                                 resetOutBuffer();
                                 if ( outputUnknown )
                                 {
                                     setOutBuffer( inputBuffer );
                                 }
                             }
                             break;
                 }
             }
        }
    }
    return result;
}

void antPowerProcessing::createPMCalibrationResultString016
(
    unsigned int  ctfDefinedID,
    unsigned int  messageValue
)
{
    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "calibration id", 16 );
            appendJSONItem( "ctf defined id", ctfDefinedID );
            appendJSONItem( "message value", messageValue );
        }
        else
        {
            if ( ctfDefinedID == 172 )
            {
                appendJSONItem( "calibration id", "ctf calibration response" );
                if ( messageValue == 1 )
                {
                    appendJSONItem( "acknowledgement", C_OFFSET_JSON );
                }
                else if ( messageValue == 2 )
                {
                    appendJSONItem( "acknowledgement", C_SLOPE_JSON );
                }
                else if ( messageValue == 3 )
                {
                    appendJSONItem( "acknowledgement", C_SERIAL_NUMBER_JSON );
                }
            }
            else
            {
                appendJSONItem( "calibration id", "ctf calibration request" );
                if ( ctfDefinedID == 1 )
                {
                    appendJSONItem( C_OFFSET_JSON, messageValue );
                }
                else if ( ctfDefinedID == 2 )
                {
                    appendJSONItem( C_SLOPE_10_JSON, messageValue );
                    appendJSONItem( C_SLOPE_JSON,    messageValue / 10.0, 1 );
                }
                else if ( ctfDefinedID == 3 )
                {
                    appendJSONItem( C_SERIAL_NUMBER_JSON, messageValue );
                }
            }
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( "16" );
            appendOutput( ctfDefinedID );
            appendOutput( messageValue );
        }
        else
        {
            if ( ctfDefinedID == 172 )
            {
                appendOutput( C_CTF_CALIBRATION_RESPONSE );
                appendOutput4Way( messageValue, C_UNKNOWN, C_ZERO_OFFSET, C_SLOPE, C_SERIAL_NUMBER );
            }
            else
            {
                appendOutput( C_CTF_CALIBRATION_REQUEST );
                appendOutput4Way( ctfDefinedID, C_UNKNOWN, C_ZERO_OFFSET, C_SLOPE, C_SERIAL_NUMBER );
                appendOutput( messageValue );
            }
        }
    }
}

void antPowerProcessing::createPMCalibrationResultString018
(
    int autoZeroEnable,
    int autoZeroStatus
)
{
    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "calibration id", 18 );
            appendJSONItem( "auto zero enable", autoZeroEnable );
            appendJSONItem( "auto zero status", autoZeroStatus );
        }
        else
        {
            appendJSONItem( "calibration id", "auto zero support" );
            appendJSONItemConditional( "auto zero enable", autoZeroEnable, "supported", "not supported" );
            appendJSONItemConditional( "auto zero status", autoZeroStatus, C_ON_JSON,   C_OFF_JSON );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( "18" );
            appendOutput( autoZeroEnable );
            appendOutput( autoZeroStatus );
        }
        else
        {
            appendOutput( C_AUTO_ZERO_ENABLE );
            appendOutputConditional( autoZeroEnable, C_SUPPORTED_ID, C_NOT_SUPPORTED_ID );
            appendOutput( C_AUTO_ZERO_STATUS );
            appendOutputConditional( autoZeroStatus, C_ON, C_OFF );
        }
    }
}

void antPowerProcessing::createPMCalibrationResultString170
(
    void
)
{
    if ( outputAsJSON )
    {
        appendJSONItemConditional( "calibration id", semiCookedOut, 170, "calibration request" );
    }
    else
    {
        appendOutputConditional( semiCookedOut, 170, C_CALIBRATION_REQUEST_MANUAL_ZERO );
    }
}

void antPowerProcessing::createPMCalibrationResultString171
(
    unsigned int autoZeroStatus
)
{
    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "calibration id", 171 );
            appendJSONItem( "auto zero status", autoZeroStatus );
        }
        else
        {
            appendJSONItem( "calibration request", "auto zero configuration" );
            appendJSONItemConditional( "auto zero", autoZeroStatus == 0, "off", ( ( autoZeroStatus == 1 ) ? "on" : "not supported" ) );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( "171" );
            appendOutput( autoZeroStatus );
        }
        else
        {
            appendOutput( C_CALIBRATION_REQUEST_AUTO_ZERO );
            appendOutput( C_AUTO_ZERO );
            appendOutputConditional( autoZeroStatus == 0, C_OFF, ( ( autoZeroStatus == 1 ) ? C_ON : C_NOT_SUPPORTED_ID ) );
        }
    }
}

void antPowerProcessing::createPMCalibrationResultString172
(
    int autoZeroStatus,
    int messageValue
)
{
    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "calibration id", 172 );
            appendJSONItem( "auto zero status", autoZeroStatus );
        }
        else
        {
            appendJSONItem( "manual zero", "success" );
            appendJSONItemConditional( "auto zero", autoZeroStatus == 0, "off", ( ( autoZeroStatus == 1 ) ? "on" : "not supported" ) );
            appendJSONItem( "value", messageValue );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( "172" );
            appendOutput( autoZeroStatus );
            appendOutput( messageValue );
        }
        else
        {
            appendOutput( C_CALIBRATION_RESPONSE_MANUAL_ZERO_SUCCESS );
            appendOutput( C_AUTO_ZERO );
            appendOutputConditional( autoZeroStatus == 0, C_OFF, ( ( autoZeroStatus == 1 ) ? C_ON : C_NOT_SUPPORTED_ID ) );
            appendOutput( messageValue );
        }
    }
}

void antPowerProcessing::createPMCalibrationResultString175
(
    int autoZeroStatus,
    int messageValue
)
{
    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "calibration id", 175 );
            appendJSONItem( "auto zero status", autoZeroStatus );
        }
        else
        {
            appendJSONItem( "manual zero", "fail" );
            appendJSONItemConditional( "auto zero", autoZeroStatus == 0, "off", ( ( autoZeroStatus == 1 ) ? "on" : "not supported" ) );
            appendJSONItem( "value", messageValue );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( "175" );
            appendOutput( autoZeroStatus );
            appendOutput( messageValue );
        }
        else
        {
            appendOutput( C_CALIBRATION_RESPONSE_MANUAL_ZERO_FAILED );
            appendOutput( C_AUTO_ZERO );
            appendOutputConditional( autoZeroStatus == 0, C_OFF, ( ( autoZeroStatus == 1 ) ? C_ON : C_NOT_SUPPORTED_ID ) );
            appendOutput( messageValue );
        }
    }
}

void antPowerProcessing::createPMCalibrationResultString186
(
    void
)
{
    if ( outputAsJSON )
    {
        appendJSONItemConditional( "calibration id", semiCookedOut, 186, "custom calibration parameter request" );
    }
    else
    {
        appendOutputConditional( semiCookedOut, 186, C_CUSTOM_CALIBRATION_PARAMETER_REQUEST );
    }
}

void antPowerProcessing::createPMCalibrationResultString187
(
    void
)
{
    if ( outputAsJSON )
    {
        appendJSONItemConditional( "calibration id", semiCookedOut, 187, "custom calibration parameter response" );
    }
    else
    {
        appendOutputConditional( semiCookedOut, 187, C_CUSTOM_CALIBRATION_PARAMETER_RESPONSE );
    }
}

void antPowerProcessing::createPMCalibrationResultString188
(
    void
)
{
    if ( outputAsJSON )
    {
        appendJSONItemConditional( "calibration id", semiCookedOut, 188, "custom calibration parameter update" );
    }
    else
    {
        appendOutputConditional( semiCookedOut, 188, C_CUSTOM_CALIBRATION_PARAMETER_UPDATE );
    }
}

void antPowerProcessing::createPMCalibrationResultString189
(
    void
)
{
    if ( outputAsJSON )
    {
        appendJSONItemConditional( "calibration id", semiCookedOut, 189, "custom calibration parameter update response" );
    }
    else
    {
        appendOutputConditional( semiCookedOut, 189, C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_RESPONSE );
    }
}

void antPowerProcessing::createPWRB01ResultString
(
    unsigned int calibrationID,
    unsigned int additionalData1,
    unsigned int additionalData2
)
{
    switch ( calibrationID )
    {
        case C_CTF_CALIBRATION_MESSAGE_ID:                      // 16 = 0x10: Crank Torque Frequency (CTF) Power Sensor (PWRB20) defined message
             createPMCalibrationResultString016( additionalData1, additionalData2 );
             break;
        case C_AUTO_ZERO_SUPPORT_MESSAGE_ID:                    // 18 = 0x12: Auto Zero Support
             createPMCalibrationResultString018( additionalData1, additionalData2 );
             break;
        case C_CALIBRATION_REQUEST_MANUAL_ZERO_ID:              // 170 = 0xAA: Calibration Request - Manual Zero
             createPMCalibrationResultString170();
             break;
        case C_CALIBRATION_REQUEST_AUTO_ZERO_ID:                // 171 = 0xAB: Calibration Request - Auto Zero Configuration
             createPMCalibrationResultString171( additionalData1 );
             break;
        case C_CALIBRATION_RESPONSE_MANUAL_ZERO_SUCCESS_ID:     // 172 = 0xAC: Calibration Response - Auto Zero Configuration Success
             createPMCalibrationResultString172( additionalData1, additionalData2 );
             break;
        case C_CALIBRATION_RESPONSE_MANUAL_ZERO_FAIL_ID:        // 175 = 0xAF: Calibration Response - Auto Zero Configuration Fail
             createPMCalibrationResultString175( additionalData1, additionalData2 );
             break;
        case C_CUSTOM_CALIBRATION_PARAMETER_REQUEST_ID:         // 186 = 0xBA: Custom Calibration Parameter Request
             createPMCalibrationResultString186();
             break;
        case C_CUSTOM_CALIBRATION_PARAMETER_RESPONSE_ID:        // 187 = 0xBB: Custom Calibration Parameter Response
             createPMCalibrationResultString187();
             break;
        case C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_ID:          // 188 = 0xBC: Custom Calibration Parameter Update
             createPMCalibrationResultString188();
             break;
        case C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_RESPONSE_ID: // 188 = 0xBC: Custom Calibration Parameter Update Response
             createPMCalibrationResultString189();
             break;
        default:
             break;
    }
}

void antPowerProcessing::createPWRB02ResultString
(
    unsigned int subPageNumber,
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3
)
{
    if ( outputAsJSON )
    {
        appendJSONItem( "subpage number", subPageNumber );
    }
    if ( subPageNumber == 1 )
    {
        if ( semiCookedOut )
        {
            if ( outputAsJSON )
            {
                appendJSONItem( "crank length (raw)",  additionalData1 );
                appendJSONItem( "sensor status",       additionalData2 );
                appendJSONItem( "sensor capabilities", additionalData3 );
            }
            else
            {
                appendOutput( "1" );
                appendOutput( additionalData1 );
                appendOutput( additionalData2 );
                appendOutput( additionalData3 );
            }
        }
        else
        {
            double crankLength             = ( additionalData1 == 255 ) ? 0 : ( 110.0 + ( double ) additionalData1 / 2.0 );
            int    crankLengthStatus       = ( additionalData2 & 3 );
            int    sensorSWMismatchStatus  = ( ( additionalData2 >> 2 ) & 3 );
            int    sensorAvailabiltyStatus = ( ( additionalData2 >> 4 ) & 3 );
            int    customCalibrationStatus = ( ( additionalData2 >> 6 ) & 3 );
            bool   autoCrankLength         = ( ( additionalData3 & 1 ) != 0 );

            if ( outputAsJSON )
            {
                if ( additionalData1 == 255 )
                {
                    appendJSONItem( "crank length", C_UNDEFINED_JSON );
                }
                else
                {
                    appendJSONItem( "crank length", crankLength, 1 );   // <xyz>.0  or  <xyz>.5
                }

                appendJSONItem4Way( "crank length set",   crankLengthStatus,       C_UNDEFINED_JSON, "default length",     "manually set",      "automatically set" );
                appendJSONItem4Way( "sw mismatch",        sensorSWMismatchStatus,  C_UNDEFINED_JSON, "right sensor older", "left sensor older", C_NONE_JSON );
                appendJSONItem4Way( "sensor availabilty", sensorAvailabiltyStatus, C_UNDEFINED_JSON, "left present",       "right present",     "both present" );
                appendJSONItem4Way( "custom calibration", customCalibrationStatus, C_UNDEFINED_JSON, "not required",       "required",          C_UNKNOWN_JSON );
                appendJSONItemConditional( "determine crank length", autoCrankLength, "automatically", "manually" );
            }
            else
            {
                appendOutput( C_CRANK_LENGTH );
                if ( additionalData1 == 255 )
                {
                    appendOutput( C_UNDEFINED );
                }
                else
                {
                    appendOutput( crankLength, 1 );
                }

                appendOutput( C_CRANK_LENGTH_USED );
                appendOutput4Way( crankLengthStatus, C_INVALID, "DEFAULT_LENGTH", "MANUALLY_SET", "AUTOMATICALLY_SET" );

                appendOutput( C_SW_MISMATCH );
                appendOutput4Way( sensorSWMismatchStatus, C_UNDEFINED, "RIGHT_SENSOR_OLDER", "LEFT_SENSOR_OLDER", C_NONE_ID );

                appendOutput( C_SENSOR_AVAILABILTY );
                appendOutput4Way( sensorAvailabiltyStatus, C_UNDEFINED, "RIGHT_PRESENT", "LEFT_PRESENT", "BOTH_PRESENT" );

                appendOutput( C_CUSTOM_CALIBRATION );
                appendOutput4Way( customCalibrationStatus, C_UNDEFINED, "NOT_REQUIRED", "REQUIRED", C_UNKNOWN );

                appendOutput( "DETERMINE_CRANK_LENGTH" );
                appendOutputConditional( autoCrankLength, "AUTOMATICALLY", "MANUALLY" );
            }
        }
    }
}

void antPowerProcessing::createPWRB03ResultString
(
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3,
    unsigned int additionalData4,
    unsigned int additionalData5,
    double       additionalDoubleData
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "number of data types", additionalData1 );
            appendJSONItem( "data type",            additionalData3 );
            appendJSONItem( "scale factor",         additionalData3 );
            appendJSONItem( "delta time stamp",     additionalData4 );
            appendJSONItem( "value",                additionalData5 );
        }
        else
        {
            appendOutput( additionalData1 );
            appendOutput( additionalData2 );
            appendOutput( additionalData3 );
            appendOutput( additionalData4 );
            appendOutput( additionalData5 );
        }
    }
    else
    {
        int    scaleFactor = NEGATE_BINARY_INT( additionalData3,  8 );
        int    value       = NEGATE_BINARY_INT( additionalData5, 16 );
        double finalValue  = scaleValue( value, scaleFactor );
        if ( outputAsJSON )
        {
            appendJSONItem( "number of data types", additionalData1 );
            appendJSONItem( "time value",           additionalDoubleData, getValuePrecision() );
        }
        else
        {
            appendOutput( "NB_DATA_TYPES" );
            appendOutput( additionalData1 );
            appendOutput( "TIME_VALUE" );
            appendOutput( additionalDoubleData, getValuePrecision() );
        }

        switch ( additionalData2 )
        {
            case  0: // Countdown (remaining process, in %)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "countdown percentage", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "COUNTDOWN_PCT" );
                         appendOutput( finalValue, 2, "\%" );
                     }
                     break;
            case  1: // Countdown (time of remaining process, in s)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "countdown seconds", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "COUNTDOWN_SEC" );
                         appendOutput( finalValue, 2, "s" );
                     }
                     break;
            case  8: // Torque (whole sensor, in Nm)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "total torque", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "TOTAL_TORQUE" );
                         appendOutput( finalValue, 2, "Nm" );
                     }
                     break;
            case  9: // Left Torque (in Nm)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "left torque", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "L_TORQUE" );
                         appendOutput( finalValue, 2, "Nm" );
                     }
                     break;
            case 10: // Right Torque (in Nm)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "right torque", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "R_TORQUE" );
                         appendOutput( finalValue, 2, "Nm" );
                     }
                     break;
            case 16: // Force (whole sensor, in N)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "total force", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "TOTAL_FORCE" );
                         appendOutput( finalValue, 2, "N" );
                     }
                     break;
            case 17: // Left Force (in N)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "left force", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "L_FORCE" );
                         appendOutput( finalValue, 2, "N" );
                     }
                     break;
            case 18: // Right Force (in N)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "right force", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "R_FORCE" );
                         appendOutput( finalValue, 2, "N" );
                     }
                     break;
            case 24: // Zero Offset
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "zero offset", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "ZERO_OFFSET" );
                         appendOutput( finalValue, 2 );
                     }
                     break;
            case 25: // Temperature (in degree C)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "temperature", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "TEMP" );
                         appendOutput( finalValue, 2, "C" );
                     }
                     break;
            case 26: // Voltage (in V)
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "voltage", finalValue, 2 );
                     }
                     else
                     {
                         appendOutput( "VOLT" );
                         appendOutput( finalValue, 2, "V" );
                     }
                     break;
            default: // Reserved for future use
                     if ( outputAsJSON )
                     {
                         appendJSONItem( "data type", C_UNDEFINED_JSON );
                     }
                     else
                     {
                         appendOutput( "UNUSED" );
                         appendOutput( "0" );
                     }
                     break;
        }
    }
}

void antPowerProcessing::createPWRB10ResultString
(
    double       &power,
    unsigned int &cadence,
    unsigned int &zeroTimeCount,
    unsigned int  eventCount,
    unsigned int  deltaEventCount,
    unsigned int  deltaAccumulatedPower,
    unsigned int  instantaneousPower,
    unsigned int  instantaneousCadence,
    unsigned int  pedalPower,
    bool          isMakeshiftSpeedSensor,
    double       &speed,
    double        wheelCircumference,
    double        gearRatio
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "event count",             eventCount );
            appendJSONItem( "delta event count",       deltaEventCount );
            appendJSONItem( "delta accumulated power", deltaAccumulatedPower );
            appendJSONItem( "instantaneous power",     instantaneousPower );
            appendJSONItemConditional( "instantaneous cadence", instantaneousCadence != 255, instantaneousCadence, C_UNDEFINED_JSON );
            appendJSONItem( "pedal power",             pedalPower );
        }
        else
        {
            appendOutput( eventCount );
            appendOutput( deltaEventCount );
            appendOutput( deltaAccumulatedPower );
            appendOutput( instantaneousPower );
            appendOutput( instantaneousCadence );
            appendOutput( pedalPower );
        }
    }
    else
    {
        int  pedalPowerContribution = 0;
        bool rightPedal             = false;

        cadence = instantaneousCadence;
        if ( deltaEventCount == 0 )
        {
            ++zeroTimeCount;
            if ( zeroTimeCount > maxZeroTimeB10 )
            {
                speed = 0;
                power = 0;
            }
        }
        else
        {
            zeroTimeCount = 0;
            power = ( double ) deltaAccumulatedPower / ( double ) deltaEventCount;
            if ( isMakeshiftSpeedSensor && ( instantaneousCadence != 255 ) )
            {
                speed = ( ( wheelCircumference > 0 ) && ( gearRatio > 0 ) ) ? wheelCircumference * cadence * gearRatio / 60.0 : 0;
            }
        }

        // pedalPower: Bit 0 - 6: Pedal Power value (in %)
        //             Bit 7 = 0: Unknown Pedal Power Contribution
        //                     1: Right Pedal Power Contribution
        if ( pedalPower == 0xFF )
        {
            pedalPowerContribution = -1;
        }
        else if ( pedalPower & 0x80 )
        {
            pedalPowerContribution = ( int ) ( pedalPower & 0x7F );
            rightPedal             = true;
        }
        else
        {
            pedalPowerContribution = ( int ) pedalPower;
            rightPedal             = false;
        }

        if ( outputAsJSON )
        {
            appendJSONItem( "event count", eventCount );
            appendJSONItem( "power",       power,              getValuePrecision() );
            appendJSONItemConditional( "cadence", instantaneousCadence != 255, cadence, C_UNDEFINED_JSON );
            if ( isMakeshiftSpeedSensor )
            {
                appendJSONItem( "speed",               speed,              getValuePrecision() );
                appendJSONItem( "wheel circumference", wheelCircumference, getValuePrecision() );
                appendJSONItem( "gear ratio",          gearRatio,          getValuePrecision() );
            }
            appendJSONItemConditional( "pedal contribution percentage", pedalPowerContribution >= 0, pedalPowerContribution, C_UNKNOWN_JSON );
            appendJSONItemConditional( "contributing pedal", ( pedalPowerContribution >= 0 ) && rightPedal, "right", C_UNKNOWN_JSON );
        }
        else
        {
            appendOutput( power, getValuePrecision() );
            appendOutputConditional( instantaneousCadence != 255, cadence, C_UNDEFINED );
            appendOutputConditional( pedalPowerContribution >= 0, pedalPowerContribution, "NO_PEDAL_POWER_CONTRIBUTION" );
            appendOutputConditional( rightPedal, C_RIGHT_PEDAL, C_UNKNOWN_PEDAL );
            appendOutput( eventCount );
            if ( isMakeshiftSpeedSensor )
            {
                appendOutput( speed, getValuePrecision() );
                appendOutput( wheelCircumference, getValuePrecision() );
                appendOutput( gearRatio, getValuePrecision() );
            }
        }
    }
}

void antPowerProcessing::createPWRB11ResultString
(
    double       &power,
    unsigned int &cadence,
    double       &torque,
    unsigned int &zeroTimeCount,
    unsigned int  deltaEventCount,
    unsigned int  deltaWheelPeriod,
    unsigned int  deltaAccumulatedTorque,
    unsigned int  instantaneousCadence,
    unsigned int  wheelTicks,
    bool          isSpeedSensor,
    double       &speed,
    double        wheelCircumference
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "delta event count",        deltaEventCount );
            appendJSONItem( "delta wheel period",       deltaWheelPeriod );
            appendJSONItem( "delta accumulated torque", deltaAccumulatedTorque );
            appendJSONItem( "instantaneous cadence",    instantaneousCadence );
            appendJSONItem( "wheel ticks",              wheelTicks );
        }
        else
        {
            appendOutput( deltaEventCount );
            appendOutput( deltaWheelPeriod );
            appendOutput( deltaAccumulatedTorque );
            appendOutput( instantaneousCadence );
            appendOutput( wheelTicks );
        }
    }
    else
    {
        if ( instantaneousCadence != 255 )
        {
            // 255 means invalid
            cadence = instantaneousCadence;
        }

        if ( deltaEventCount == 0 )
        {
            ++zeroTimeCount;
            if ( zeroTimeCount > maxZeroTimeB11 )
            {
                speed  = 0;
                power  = 0;
                torque = 0;
            }
        }
        else
        {
            zeroTimeCount = 0;
            torque = ( double ) deltaAccumulatedTorque / ( 32.0 * ( double ) deltaEventCount );
            if ( deltaWheelPeriod != 0 )
            {
                double angularVel = 2048.0 * ( double ) deltaEventCount / ( ( double ) deltaWheelPeriod );
                power             = torque * angularVel * C_TWO_PI;
                if ( isSpeedSensor )
                {
                    speed         = wheelCircumference * angularVel;
                }
            }
        }

        if ( outputAsJSON )
        {
            appendJSONItem( "power",               power,              getValuePrecision() );
            appendJSONItem( "cadence",             cadence );
            appendJSONItem( "torque",              torque,             getValuePrecision() );
            appendJSONItem( "wheel ticks",         wheelTicks );
            if ( isSpeedSensor )
            {
                appendJSONItem( "speed",               speed,              getValuePrecision() );
                appendJSONItem( "wheel circumference", wheelCircumference, getValuePrecision() );
            }
        }
        else
        {
            appendOutput( power, getValuePrecision() );
            appendOutput( cadence );
            appendOutput( torque, getValuePrecision() );
            appendOutput( wheelTicks );
            if ( isSpeedSensor )
            {
                appendOutput( speed, getValuePrecision() );
                appendOutput( wheelCircumference, getValuePrecision() );
            }
        }
    }
}

void antPowerProcessing::createPWRB12ResultString
(
    double       &power,
    unsigned int &cadence,
    double       &torque,
    unsigned int &zeroTimeCount,
    unsigned int  eventCount,
    unsigned int  deltaEventCount,
    unsigned int  deltaCrankPeriod,
    unsigned int  deltaAccumulatedTorque,
    unsigned int  instantaneousCadence,
    unsigned int  crankTicks,
    bool          isMakeshiftSpeedSensor,
    double       &speed,
    double        wheelCircumference,
    double        gearRatio
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "event count",              eventCount );
            appendJSONItem( "delta event count",        deltaEventCount );
            appendJSONItem( "delta crank period",       deltaCrankPeriod );
            appendJSONItem( "delta accumulated torque", deltaAccumulatedTorque );
            appendJSONItem( "instantaneous cadence",    instantaneousCadence );
            appendJSONItem( "crank ticks",              crankTicks );
        }
        else
        {
            appendOutput( deltaEventCount );
            appendOutput( deltaCrankPeriod );
            appendOutput( deltaAccumulatedTorque );
            appendOutput( instantaneousCadence );
            appendOutput( crankTicks );
            appendOutput( eventCount );
        }
    }
    else
    {
        if ( deltaEventCount == 0 )
        {
            ++zeroTimeCount;
            if ( zeroTimeCount > maxZeroTimeB12 )
            {
                cadence = 0;
                speed   = 0;
                power   = 0;
                torque  = 0;
            }
        }
        else
        {
            double cadenceAsDouble = ( double ) cadence;

            zeroTimeCount = 0;
            torque = ( double ) deltaAccumulatedTorque / ( 32.0 * ( double ) deltaEventCount );

            // Compute power
            if ( deltaCrankPeriod != 0 )
            {
                cadenceAsDouble   = ( double ) deltaEventCount / ( ( double ) deltaCrankPeriod / 2048.0 );
                double angularVel = C_TWO_PI * cadenceAsDouble;
                cadenceAsDouble  *= 60.0;
                cadence           = ( unsigned int ) round( cadenceAsDouble );
                power             = torque * angularVel;
                if ( isMakeshiftSpeedSensor )
                {
                    speed             = ( ( wheelCircumference > 0 ) && ( gearRatio > 0 ) ) ?
                                        wheelCircumference * cadenceAsDouble * gearRatio / 60.0 : 0;
                }
            }
        }

        if ( outputAsJSON )
        {
            appendJSONItem( "event count",           eventCount );
            appendJSONItem( "power",                 power,              getValuePrecision() );
            appendJSONItem( "cadence",               cadence );
            appendJSONItem( "instantaneous cadence", instantaneousCadence );
            appendJSONItem( "torque",                torque,             getValuePrecision() );
            appendJSONItem( "crank ticks",           crankTicks );
            if ( isMakeshiftSpeedSensor )
            {
                appendJSONItem( "speed",               speed,              getValuePrecision() );
                appendJSONItem( "wheel circumference", wheelCircumference, getValuePrecision() );
                appendJSONItem( "gear ratio",          gearRatio,          getValuePrecision() );
            }
        }
        else
        {
            appendOutput( power,              getValuePrecision() );
            appendOutput( cadence );
            appendOutput( torque,             getValuePrecision() );
            appendOutput( crankTicks );
            appendOutput( eventCount );
            if ( isMakeshiftSpeedSensor )
            {
                appendOutput( speed,              getValuePrecision() );
                appendOutput( wheelCircumference, getValuePrecision() );
                appendOutput( gearRatio,          getValuePrecision() );
            }
        }
    }
}

void antPowerProcessing::createPWRB13ResultString
(
    unsigned int rawLeftTorqueEffectiveness,
    unsigned int rawRightTorqueEffectiveness,
    unsigned int rawLeftPedalSmoothness,
    unsigned int rawRightPedalSmoothness,
    unsigned int deltaEventCount,
    unsigned int eventCount
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "left torque effectiveness",  rawLeftTorqueEffectiveness );
            appendJSONItem( "right torque effectiveness", rawRightTorqueEffectiveness );
            appendJSONItem( "left pedal smoothness",      rawLeftPedalSmoothness );
            appendJSONItem( "right pedal smoothness",     rawRightPedalSmoothness );
            appendJSONItem( "delta event count",          deltaEventCount );
        }
        else
        {
            appendOutput( rawLeftTorqueEffectiveness );
            appendOutput( rawRightTorqueEffectiveness );
            appendOutput( rawLeftPedalSmoothness );
            appendOutput( rawRightPedalSmoothness );
            appendOutput( deltaEventCount );
        }
    }
    else
    {
        double leftTorqueValue  = ( ( double ) rawLeftTorqueEffectiveness  ) / 2.0;
        double rightTorqueValue = ( ( double ) rawRightTorqueEffectiveness ) / 2.0;
        double leftPedalValue   = ( ( double ) rawLeftPedalSmoothness )      / 2.0;
        double rightPedalValue  = ( ( double ) rawRightPedalSmoothness )     / 2.0;

        if ( outputAsJSON )
        {
            if ( rawLeftTorqueEffectiveness == 255 )
            {
                appendJSONItem( "left torque effectiveness", C_INVALID_JSON );
            }
            else
            {
                appendJSONItem( "left torque effectiveness", leftTorqueValue, 1 );
            }
            if ( rawRightTorqueEffectiveness == 255 )
            {
                appendJSONItem( "right torque effectiveness", C_INVALID_JSON );
            }
            else
            {
                appendJSONItem( "right torque effectiveness", rightTorqueValue, 1 );
            }

            if ( rawRightPedalSmoothness == 254 )
            {
                // Pedal Smoothness is combined, no Left/Right Pedal Smoothness.
                if ( rawLeftPedalSmoothness == 255 )
                {
                    appendJSONItem( "common pedal smoothness", C_INVALID_JSON );
                }
                else
                {
                    appendJSONItem( "common pedal smoothness", leftPedalValue, 1 );
                }
            }
            else
            {
                // Separate Left and Right Pedal Smoothness
                if ( rawLeftPedalSmoothness == 255 )
                {
                    appendJSONItem( "left pedal smoothness", C_INVALID_JSON );
                }
                else
                {
                    appendJSONItem( "left pedal smoothness", leftPedalValue, 1 );
                }
                if ( rawRightPedalSmoothness == 255 )
                {
                    appendJSONItem( "right pedal smoothness", C_INVALID_JSON );
                }
                else
                {
                    appendJSONItem( "right pedal smoothness", rightPedalValue, 1 );
                }
            }

            appendJSONItem( "event count", eventCount );
        }
        else
        {
            appendOutput( C_LEFT_TORQUE_EFFECTIVENESS );
            if ( rawLeftTorqueEffectiveness == 255 )
            {
                appendOutput( C_INVALID );
            }
            else
            {
                appendOutput( leftTorqueValue, 1, "\%" );
            }

            appendOutput( C_RIGHT_TORQUE_EFFECTIVENESS );
            if ( rawRightTorqueEffectiveness == 255 )
            {
                appendOutput( C_INVALID );
            }
            else
            {
                appendOutput( rightTorqueValue, 1, "\%" );
            }

            if ( rawRightPedalSmoothness == 254 )
            {
                // Pedal Smoothness is combined, no Left/Right Pedal Smoothness.
                appendOutput( C_COMMON_PEDAL_SMOOTHNESS );
                if ( rawLeftPedalSmoothness == 255 )
                {
                    appendOutput( C_INVALID );
                }
                else
                {
                    appendOutput( leftPedalValue, 1, "\%" );
                }
            }
            else
            {
                // Separate Left and Right Pedal Smoothness
                appendOutput( C_LEFT_PEDAL_SMOOTHNESS );
                if ( rawLeftPedalSmoothness == 255 )
                {
                    appendOutput( C_INVALID );
                }
                else
                {
                    appendOutput( leftPedalValue, 1, "\%" );
                }

                appendOutput( C_RIGHT_PEDAL_SMOOTHNESS );
                if ( rawRightPedalSmoothness == 255 )
                {
                    appendOutput( C_INVALID );
                }
                else
                {
                    appendOutput( rightPedalValue, 1, "\%" );
                }
            }
            appendOutput( "EVENT_NO" );
            appendOutput( eventCount );
        }
    }
}

void antPowerProcessing::createPWRB20ResultString
(
    double       &power,
    unsigned int &cadence,
    double       &torque,
    unsigned int &zeroTimeCount,
    unsigned int  deltaEventCount,
    unsigned int  deltaTimeStamp,
    unsigned int  deltaTorqueTicks,
    unsigned int  factorySlope_Nm_10Hz,
    unsigned int  powerMeterOffset,
    unsigned int  userDefinedSlope_Nm_10Hz,
    bool          isMakeshiftSpeedSensor,
    double       &speed,
    double        wheelCircumference,
    double        gearRatio
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "delta event count",       deltaEventCount );
            appendJSONItem( "delta time stamp",        deltaTimeStamp );
            appendJSONItem( "delta torque ticks",      deltaTorqueTicks );
            appendJSONItem( "factory slope times ten", factorySlope_Nm_10Hz );
        }
        else
        {
            appendOutput( deltaEventCount );
            appendOutput( deltaTimeStamp );
            appendOutput( deltaTorqueTicks );
            appendOutput( factorySlope_Nm_10Hz );
        }
    }
    else
    {
        unsigned int powerMeterSlope_Nm_10Hz = ( userDefinedSlope_Nm_10Hz == C_SLOPE_DEFAULT ) ? factorySlope_Nm_10Hz : userDefinedSlope_Nm_10Hz;

        if ( deltaTimeStamp == 0 )
        {
            ++zeroTimeCount;
            if ( zeroTimeCount > maxZeroTimeB20 )
            {
                speed   = 0;
                power   = 0;
                torque  = 0;
                cadence = 0;
            }
        }
        else
        {
            zeroTimeCount = 0;
            if ( deltaEventCount != 0 )
            {
                double newCadence    = ( double ) cadence;
                double elapsedTime   = ( double ) deltaTimeStamp / 2000.0;
                double cadencePeriod = elapsedTime / ( double ) deltaEventCount;

                if ( cadencePeriod != 0 )
                {
                    newCadence = 60.0 / cadencePeriod;
                }

                if ( ( deltaTorqueTicks > 0 ) && ( cadencePeriod > 0 ) && ( powerMeterSlope_Nm_10Hz != 0 ) )
                {
                    double rawTorqueFrequency = ( ( double ) deltaTorqueTicks / elapsedTime );
                    double torqueFrequency    = rawTorqueFrequency - ( double ) powerMeterOffset;
                    double newTorque          = 10.0 * torqueFrequency / ( double ) powerMeterSlope_Nm_10Hz;
                    double newPower           = newTorque * C_TWO_PI / cadencePeriod;

                    if ( ( newPower   >= C_MIN_POWER_VALUE ) && ( newPower   <= C_MAX_POWER_VALUE ) &&
                         ( newCadence >= C_MIN_CADENCE     ) && ( newCadence <= C_MAX_CADENCE     ) )
                    {
                        power   = newPower;
                        torque  = newTorque;
                        cadence = ( unsigned int ) round( newCadence );
                    }
                    // Else: This is a spike. Use the previous value
                }
                else
                {
                    power  = 0;
                    torque = 0;
                }
                if ( isMakeshiftSpeedSensor )
                {
                    speed = antCadenceSpeedProcessing::computeSpeed( newCadence, wheelCircumference, gearRatio );
                }
            }
        }

        if ( outputAsJSON )
        {
            appendJSONItem( "power",               power,              getValuePrecision() );
            appendJSONItem( "cadence",             cadence );
            appendJSONItem( "torque",              torque,             getValuePrecision() );
            appendJSONItem( "power meter offset",  powerMeterOffset );
            appendJSONItem( "power meter slope",   powerMeterSlope_Nm_10Hz / 10.0, 1 );
            appendJSONItemConditional( "slope used", userDefinedSlope_Nm_10Hz == C_SLOPE_DEFAULT, "factory slope", "user-defined slope" );
            if ( isMakeshiftSpeedSensor )
            {
                appendJSONItem( "speed",               speed,              getValuePrecision() );
                appendJSONItem( "wheel circumference", wheelCircumference, getValuePrecision() );
                appendJSONItem( "gear ratio",          gearRatio,          getValuePrecision() );
            }
        }
        else
        {
            appendOutput( power,                          getValuePrecision() );
            appendOutput( cadence );
            appendOutput( torque,                         getValuePrecision() );
            appendOutput( powerMeterOffset );
            appendOutput( powerMeterSlope_Nm_10Hz / 10.0, 1 );
            appendOutputConditional( userDefinedSlope_Nm_10Hz == C_SLOPE_DEFAULT, "FACTORY_SLOPE", "USER_DEFINED_SLOPE" );
            appendOutput( factorySlope_Nm_10Hz / 10.0, 1 );
            if ( isMakeshiftSpeedSensor )
            {
                appendOutput( speed,                      getValuePrecision() );
                appendOutput( wheelCircumference,         getValuePrecision() );
                appendOutput( gearRatio,                  getValuePrecision() );
            }
        }
    }
}

double antPowerProcessing::scaleValue
(
    int value,
    int scaleFactor
)
{
    double result = 0;

    if ( scaleFactor >= 0 ) 
    {   
        result = ( double ) value;
        if ( scaleFactor > 0 ) 
        {   
            result *= pow( 2.0, scaleFactor );
        }   
    }   
    else
    {   
        double resultFraction = 0;
        int    nbBits         = -scaleFactor;
        int    intValue       = ( value >> nbBits );
        int    fractionValue  = ( value - ( intValue << nbBits ) );

        result = ( double ) intValue;
        while ( fractionValue )
        {   
            if ( fractionValue & 1 ) 
            {   
                ++resultFraction;
            }   
            resultFraction /= 2.0;
            fractionValue >>= 1;
        }   
        result += resultFraction;
    }   

    return result;
}


