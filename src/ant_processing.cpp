#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_processing.h"

// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

#define AAA( _XXX_ ) std::cerr << "AAA " << _XXX_ << std::endl;

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antProcessing::antProcessing
(
    void
)
{
    b2tVersion = BUILD_NUMBER;
    testMode   = false;
    resetAll();
}

void antProcessing::resetAll
(
    void
)
{
    errorCode = 0;
    resetOutBuffer();
    resetDiagnosticsBuffer();
    resetRawBuffer();
    setTimePrecision( C_DEFAULT_TIME_PRECISION_DEFAULT );
    setValuePrecision( C_DEFAULT_VALUE_PRECISION_DEFAULT );
    setOutputUnknown( C_DEFAULT_OUTPUT_UNKNOWN );
    setOutputBridge( C_DEFAULT_OUTPUT_BRIDGE );
    setDeviceFileName( C_DEFAULT_DEVICE_FILE );
    setInputFileName( C_DEFAULT_INPUT_FILE_NAME );
    setInterface( C_DEFAULT_INTERFACE );
    setMCAddressIn( C_DEFAULT_MC_ADDRESS_IN );
    setMCPortNoIn( C_DEFAULT_MC_PORT_NO_IN );
    setMCAddressOut( C_DEFAULT_MC_ADDRESS_OUT );
    setMCPortNoOut( C_DEFAULT_MC_PORT_NO_OUT );
    setTimeOutSec( C_DEFAULT_TIME_OUT_SEC );
    setWriteStdout( C_DEFAULT_WRITE_STDOUT );
    setSemiCookedIn( C_DEFAULT_SEMI_COOKED_IN );
    setSemiCookedOut(C_DEFAULT_SEMI_COOKED_OUT );
    setOutputAsJSON( C_DEFAULT_OUTPUT_AS_JSON );
    setOutputRaw( C_DEFAULT_OUTPUT_RAW );
    setOnlyRegisteredDevices( C_DEFAULT_ONLY_REGISTERED_DEVICES );

    setDiagnostics( C_DEFAULT_DIAGNOSTICS );

    initializeSupportedDeviceTypes();
}

unsigned int antProcessing::getDeltaInt
(
    bool                             &rollOverHappened,
    const amString                   &sensorID,
    unsigned int                      rollOver,
    std::map<amString, unsigned int> &valueTable,
    unsigned int                      newValue
)
{
    unsigned int prevValue  = valueTable[ sensorID ];
    valueTable[ sensorID ]  = newValue;
    rollOverHappened        = ( newValue < prevValue );
    unsigned int deltaValue = rollOverHappened ? ( rollOver - prevValue + newValue ) : ( newValue - prevValue );
    return deltaValue;
}

//---------------------------------------------------------------------------------------------------
//
// setTimePrecision
//
// Set the precision value for the floating point output of time values.
//
//---------------------------------------------------------------------------------------------------
void antProcessing::setTimePrecision
(
    int value
)
{
    if ( ( value >= C_MIN_PRECISION ) && ( value <= C_MAX_PRECISION ) )
    {
        timePrecision = value;
    }
}

//---------------------------------------------------------------------------------------------------
//
// setValuePrecision
//
// Set the precision value for floating point output of real integer quantities.
//
//---------------------------------------------------------------------------------------------------
void antProcessing::setValuePrecision
(
    int value
)
{
    if ( ( value >= C_MIN_PRECISION ) && ( value <= C_MAX_PRECISION ) )
    {
        valuePrecision = value;
    }
}

bool antProcessing::isSemiCookedFormat137
(
    const amString &inputBuffer
)
{
    amSplitString words( inputBuffer );
    amString      lastWord = words.back();
    bool          result   = lastWord.startsWith( "1.34." );
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
//    const amString &timeStampBuffer   IN   Device ID (number).
//    BYTE   payLoad[]         IN   Array of bytes with the data to be converted.
//
// Return amDeviceType SPEED_SENSOR, CADENCE_SENSOR, POWER_METER, AERO_SENSOR, or HEART_RATE_METER
//             if successful.
//        amDeviceType OTHER_DEVICE otherwise (device type ot recognized)
//
//---------------------------------------------------------------------------------------------------
amDeviceType antProcessing::processSensor
(
    int             deviceType,
    const amString &deviceID,
    const amString &timeStampBuffer,
    BYTE   payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    std::stringstream auxStringStream;
    auxStringStream << deviceType;
    appendErrorMessage( "ERROR: Unknown device type " );
    appendErrorMessage( auxStringStream.str() );
    appendErrorMessage( ".\n" );

    return result;
}

amDeviceType antProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( inputBuffer.empty() )
    {
        appendErrorMessage( "ERROR: Input Buffer is empty" );
    }
    else
    {
        appendErrorMessage( "ERROR: Unknown device type in \"" );
        appendErrorMessage( inputBuffer );
    }
    appendErrorMessage( "\".\n" );

    return result;
}


amDeviceType antProcessing::updateSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( inputBuffer == "TYPE" )
    {
        result = processUndefinedSensorType( inputBuffer );
    }

    if ( result == OTHER_DEVICE )
    {
        setOutBuffer( inputBuffer );
    }

    return result;
}

// ------------------------------------------------------------------------------------------------------
//
// processUndefinedSensorType
//
// Description:
//     This function checks for "TYPE" messages, i.e. ANT+ messages that could not be
//     processed by a previous version of 'ant2txt' and were output as
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
//
// ------------------------------------------------------------------------------------------------------
amDeviceType antProcessing::processUndefinedSensorType
(
    const amString &inputBuffer
)
{
    amDeviceType  result                 = OTHER_DEVICE;
    amString      deviceID;
    amString      deviceTypeString;
    amString      sensorID;
    amString      timeStampBuffer;
    amString      auxBuffer;
    amSplitString words;
    amSplitString sensorParts;
    unsigned int  nbWords                = words.split( inputBuffer );
    BYTE payLoad[ C_ANT_PAYLOAD_LENGTH ] = { 0 };
    unsigned int  nbSensorParts          = 0;
    unsigned int  deviceType             = 0;
    unsigned int  counter                = 0;
    unsigned int  syntaxError            = 0;

    syntaxError = ( nbWords >= 2 + C_ANT_PAYLOAD_LENGTH ) ? 0 : 1;
    if ( syntaxError == 0 )
    {
        // Example: sensorID = "TYPE11_98765"
        //          Extract deviceType = 11
        //          and     deviceID = "98765"
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
        deviceID  = sensorParts[ 1 ];
        syntaxError = ( ( unsigned int ) deviceID.toUInt() != 0 ) ? 0 : 5;
    }

    if ( syntaxError == 0 )
    {
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
                auxBuffer  = "payLoad [ ";
                auxBuffer += amString( counter );
                auxBuffer += " ]";
                appendDiagnosticsLine( auxBuffer, payLoad[ counter ] );
            }
        }

        result = processSensor( deviceType, deviceID, timeStampBuffer, payLoad );
    }
    else
    {
        setOutBuffer( inputBuffer );
    }


    return result;
}

void antProcessing::reset
(
    void
)
{
    zeroTimeCountTable.clear();
    registeredDevices.clear();
    eventTimeTable.clear();
    eventCountTable.clear();
    totalTimeIntTable.clear();
    totalCountTable.clear();
    operatingTimeTable.clear();
    sameEventCountTable.clear();
    totalTimeTable.clear();
    totalOperatingTimeTable.clear();
}

bool antProcessing::isSupportedSensor
(
    const amString &sensorID
)
{
    bool result = false;
    for ( int count = 0; !result && ( count < ( int ) supportedSensorTypes.size() ); ++count )
    {
        if ( sensorID.substr( 0, supportedSensorTypes[ count ].size() ) == supportedSensorTypes[ count ] )
        {
            result = true;
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------------------
//
// isRegisteredDevice
//
// Check if a device is registered.
//
// Parameters:
//    const amString &deviceID   IN   Device ID (key for the has table)
//
// Return true if the device hash table has an entry for the key.
//        false otherwise.
//
//---------------------------------------------------------------------------------------------------
bool antProcessing::isRegisteredDevice
(
    const amString &deviceID
)
{
    bool result = !onlyRegisteredDevices;
    if ( !result )
    {
        result = ( registeredDevices.count( deviceID ) > 0 );
    }
    return result;
}

void antProcessing::registerDevice
(
    const amString &deviceID
)
{
    if ( registeredDevices.count( deviceID ) == 0 )
    {
        registeredDevices.insert( std::pair<amString, bool>( deviceID, false ) );
    }
}

// -------------------------------------------------------------------------------------------------//
//
// Write the battery status for the given index into a buffer
//
// -------------------------------------------------------------------------------------------------//
bool antProcessing::getBatteryStatus
(
   amString &status,
   int       index,
   bool      lowerCase
)
{
    bool result = true;
    switch ( index )
    {
        case  0: status = "N/A";
                 break;
        case  1: status = "New";
                 break;
        case  2: status = "Good";
                 break;
        case  3: status = "Ok";
                 break;
        case  4: status = "Low";
                 break;
        case  5: status = "Critical";
                 break;
        case  6: status = "Reserved for future use";
                 break;
        case  7: status = "Invalid";
                 break;
        default: status = "Unknown";
                 result = false;
                 break;
    }
    if ( lowerCase )
    {
        status.toLower();
    }
    return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// Auxilairy functions
unsigned int antProcessing::uChar2UInt
(
    BYTE byte1,
    BYTE byte2,
    BYTE byte3,
    BYTE byte4
)
{
    unsigned int result = ( uChar2UInt( byte1, byte2, byte3 ) << 8 ) + ( unsigned int ) byte4;
    return result;
}

unsigned int antProcessing::uChar2UInt
(
    BYTE byte1,
    BYTE byte2,
    BYTE byte3
)
{
    unsigned int result = ( uChar2UInt( byte1, byte2 ) << 8 ) + ( unsigned int ) byte3;
    return result;
}

unsigned int antProcessing::uChar2UInt
(
    BYTE byte1,
    BYTE byte2
)
{
    unsigned int result = ( uChar2UInt( byte1 ) << 8 ) + ( unsigned int ) byte2;
    return result;
}

unsigned int antProcessing::uChar2UInt
(
    BYTE byte1
)
{
    unsigned int result = ( unsigned int ) byte1;
    return result;
}

void antProcessing::appendErrorMessage
(
    const amString &message
)
{
    errorMessage += message;
}

void antProcessing::appendErrorMessage
(
    int value
)
{
    errorMessage += amString( value );
}

void antProcessing::appendErrorMessage
(
    unsigned int value
)
{
    errorMessage += amString( value );
}

void antProcessing::appendErrorMessage
(
    BYTE value
)
{
    errorMessage += amString( value );
}
void antProcessing::appendErrorMessage
(
    double value,
    int precision
)
{
    errorMessage += amString( value, precision );
}

void antProcessing::appendDiagnosticsField
(
    const amString &fieldName
)
{
    amString auxBuffer( C_DIAGNOSTICS_INDENT );
    auxBuffer += fieldName;
    while ( auxBuffer.size() < S_DIAGNOSTICS_BASE_LENGTH )
    {
        auxBuffer += " ";
    }
    auxBuffer += ": ";
    diagnosticsBuffer += "\n";
    diagnosticsBuffer += auxBuffer;
}

void antProcessing::appendDiagnosticsItemName
(
    const amString &itemName
)
{
    amString auxBuffer;
    auxBuffer += C_DIAGNOSTICS_INDENT;
    auxBuffer += C_DIAGNOSTICS_INDENT;
    auxBuffer += itemName;
    while ( auxBuffer.size() < S_DIAGNOSTICS_BASE_LENGTH )
    {
        auxBuffer += " ";
    }
    diagnosticsBuffer += "\n";
    diagnosticsBuffer += auxBuffer;
    diagnosticsBuffer += ": ";
}

void antProcessing::appendDiagnosticsLine
(
    const amString &message
)
{
    diagnosticsBuffer += "\n";
    diagnosticsBuffer += C_DIAGNOSTICS_INDENT;
    diagnosticsBuffer += C_DIAGNOSTICS_INDENT;
    diagnosticsBuffer += message;
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    unsigned int    index,
    BYTE            itemValue
)
{
    diagnosticsBuffer += itemName;
    diagnosticsBuffer += "[ ";;
    diagnosticsBuffer += amString( index );
    diagnosticsBuffer += " ]";;
    diagnosticsBuffer += amString(itemValue ); 
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    const amString &itemValue
)
{
    appendDiagnosticsItemName( itemName );
    diagnosticsBuffer += "\"";
    diagnosticsBuffer += itemValue;
    diagnosticsBuffer += "\"";
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    unsigned int    itemValue,
    const amString &additionalInfo
)
{
    appendDiagnosticsItemName( itemName );
    diagnosticsBuffer += amString( itemValue );
    if ( !additionalInfo.empty() )
    {
        diagnosticsBuffer += " ";
        diagnosticsBuffer += additionalInfo;
    }
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    const amString &stringValue,
    unsigned int    itemValue,
    const amString &additionalInfo
)
{
    appendDiagnosticsItemName( itemName );
    diagnosticsBuffer += stringValue;
    diagnosticsBuffer += " = ";
    diagnosticsBuffer += amString( itemValue );
    if ( !additionalInfo.empty() )
    {
        diagnosticsBuffer += " ";
        diagnosticsBuffer += additionalInfo;
    }
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    BYTE            byteValue,
    unsigned int    itemValue,
    const amString &additionalInfo
)
{

    appendDiagnosticsItemName( itemName );
    diagnosticsBuffer += amString( byteValue );
    diagnosticsBuffer += " = ";
    diagnosticsBuffer += amString( itemValue );
    if ( !additionalInfo.empty() )
    {
        diagnosticsBuffer += " ";
        diagnosticsBuffer += additionalInfo;
    }
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    BYTE            byteValue1,
    BYTE            byteValue2,
    unsigned int    itemValue,
    const amString &additionalInfo
)
{
    appendDiagnosticsItemName( itemName );
    diagnosticsBuffer += amString( byteValue1, byteValue2 );
    diagnosticsBuffer += " = ";
    diagnosticsBuffer += amString( itemValue );
    if ( !additionalInfo.empty() )
    {
        diagnosticsBuffer += " ";
        diagnosticsBuffer += additionalInfo;
    }
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    BYTE            byteValue1,
    BYTE            byteValue2,
    BYTE            byteValue3,
    unsigned int    itemValue,
    const amString &additionalInfo
)
{
    appendDiagnosticsItemName( itemName );
    diagnosticsBuffer += amString( byteValue1, byteValue2, byteValue3 );
    diagnosticsBuffer += " = ";
    diagnosticsBuffer += amString( itemValue );
    if ( !additionalInfo.empty() )
    {
        diagnosticsBuffer += " ";
        diagnosticsBuffer += additionalInfo;
    }
}

void antProcessing::appendDiagnosticsLine
(
    const amString &itemName,
    BYTE            byteValue1,
    BYTE            byteValue2,
    BYTE            byteValue3,
    BYTE            byteValue4,
    unsigned int    itemValue,
    const amString &additionalInfo
)
{
    appendDiagnosticsItemName( itemName );
    diagnosticsBuffer += amString( byteValue1, byteValue2, byteValue3, byteValue4 );
    diagnosticsBuffer += " = ";
    diagnosticsBuffer += amString( itemValue );
    if ( !additionalInfo.empty() )
    {
        diagnosticsBuffer += " ";
        diagnosticsBuffer += additionalInfo;
    }
}

void antProcessing::appendOutput
(
    BYTE itemValue
)
{
    outBuffer += "\t";
    outBuffer += amString( itemValue );
}

void antProcessing::appendOutput
(
    int             itemValue,
    const amString &unit
)
{
    outBuffer += "\t";
    outBuffer += amString( itemValue );
    outBuffer += unit;
}

void antProcessing::appendOutput
(
    double          itemValue,
    unsigned int    precision,
    const amString &unit
)
{
    outBuffer += "\t";
    outBuffer += amString( itemValue, precision );
    outBuffer += unit;
}

void antProcessing::appendOutput
(
    unsigned int    itemValue,
    const amString &unit
)
{
    outBuffer += "\t";
    outBuffer += amString( itemValue );
    outBuffer += unit;
}

void antProcessing::appendOutput
(
    const amString &itemValue
)
{
    outBuffer += "\t";
    outBuffer += itemValue;
}

// -------------------------------------------------------------------------------------------------//
// Append an consitional value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendOutputConditional
(
    bool            condition,
    const amString &itemValueTrue,
    const amString &itemValueFalse
)
{
    outBuffer += "\t";
    if ( condition )
    {
        outBuffer += itemValueTrue;
    }
    else
    {
        outBuffer += itemValueFalse;
    }
}

void antProcessing::appendOutputConditional
(
    bool            condition,
    int             itemValueTrue,
    const amString &itemValueFalse
)
{
    outBuffer += "\t";
    if ( condition )
    {
        outBuffer += amString( itemValueTrue );
    }
    else
    {
        outBuffer += itemValueFalse;
    }
}

void antProcessing::appendOutputConditional
(
    bool            condition,
    unsigned int    itemValueTrue,
    const amString &itemValueFalse
)
{
    outBuffer += "\t";
    if ( condition )
    {
        outBuffer += amString( itemValueTrue );
    }
    else
    {
        outBuffer += itemValueFalse;
    }
}

void antProcessing::appendOutputConditional
(
    bool            condition,
    double          itemValueTrue,
    const amString &itemValueFalse,
    int             precision
)
{
    outBuffer += "\t";
    if ( condition )
    {
        outBuffer += amString( itemValueTrue, precision );
    }
    else
    {
        outBuffer += itemValueFalse;
    }
}

void antProcessing::appendOutput4Way
(
    int             condition,
    const amString &itemValue0,
    const amString &itemValue1,
    const amString &itemValue2,
    const amString &itemValue3
)
{
    outBuffer += "\t";
    if ( condition == 0 )
    {
        outBuffer += itemValue0;
    }
    else if ( condition == 1 )
    {
        outBuffer += itemValue1;
    }
    else if ( condition == 2 )
    {
        outBuffer += itemValue2;
    }
    else
    {
        outBuffer += itemValue3;
    }
}

// -------------------------------------------------------------------------------------------------//
// Append an BYTE (byte) value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendJSONItem
(
    const amString &itemName,
    BYTE   itemValue
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": ";
    outBuffer += amString( itemValue );
    outBuffer += ",\n";
}

// -------------------------------------------------------------------------------------------------//
// Append an unsigned integer value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendJSONItem
(
    const amString &itemName,
    unsigned int    itemValue
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": ";
    outBuffer += amString( itemValue );
    outBuffer += ",\n";
}

// -------------------------------------------------------------------------------------------------//
// Append an (signed) integer value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendJSONItem
(
    const amString &itemName,
    int             itemValue
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": ";
    outBuffer += amString( itemValue );
    outBuffer += ",\n";
}

// -------------------------------------------------------------------------------------------------//
// Append an boolean value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendJSONItemB
(
    const amString &itemName,
    bool            itemValue
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": ";
    outBuffer += ( itemValue ? C_TRUE_JSON : C_FALSE_JSON );
    outBuffer += ",\n";
}

// -------------------------------------------------------------------------------------------------//
// Append a string value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendJSONItem
(
    const amString &itemName,
    const amString &itemValue
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": \"";
    outBuffer += itemValue;
    outBuffer += "\",\n";
}

// -------------------------------------------------------------------------------------------------//
// Append an double precision value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendJSONItem
(
    const amString &itemName,
    double          itemValue,
    int             precision
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": ";
    outBuffer += amString( itemValue, precision );
    outBuffer += ",\n";
}

// -------------------------------------------------------------------------------------------------//
// Append an consitional value to a JSON object
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendJSONItemConditional
(
    const amString &itemName,
    bool            condition,
    const amString &itemValueTrue,
    const amString &itemValueFalse
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": \"";
    if ( condition )
    {
        outBuffer += itemValueTrue;
    }
    else
    {
        outBuffer += itemValueFalse;
    }
    outBuffer += "\",\n";
}

void antProcessing::appendJSONItemConditional
(
    const amString &itemName,
    bool            condition,
    int             itemValueTrue,
    const amString &itemValueFalse
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    if ( condition )
    {
        outBuffer += "\": ";
        outBuffer += amString( itemValueTrue );
        outBuffer += ",\n";
    }
    else
    {
        outBuffer += "\": \"";
        outBuffer += itemValueFalse;
        outBuffer += "\",\n";
    }
}

void antProcessing::appendJSONItemConditional
(
    const amString &itemName,
    bool            condition,
    unsigned int    itemValueTrue,
    const amString &itemValueFalse
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    if ( condition )
    {
        outBuffer += "\": ";
        outBuffer += amString( itemValueTrue );
        outBuffer += ",\n";
    }
    else
    {
        outBuffer += "\": \"";
        outBuffer += itemValueFalse;
        outBuffer += "\",\n";
    }
}

void antProcessing::appendJSONItemConditional
(
    const amString &itemName,
    bool            condition,
    double          itemValueTrue,
    const amString &itemValueFalse,
    int             precision
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    if ( condition )
    {
        outBuffer += "\": ";
        outBuffer += amString( itemValueTrue );
        outBuffer += ",\n";
    }
    else
    {
        outBuffer += "\": \"";
        outBuffer += itemValueFalse;
        outBuffer += "\",\n";
    }
}

void antProcessing::appendJSONItem4Way
(
    const amString &itemName,
    int             condition,
    const amString &itemValue0,
    const amString &itemValue1,
    const amString &itemValue2,
    const amString &itemValue3
)
{
    outBuffer += C_JSON_INDENT;
    outBuffer += "\"";
    outBuffer += itemName;
    outBuffer += "\": \"";
    if ( condition == 0 )
    {
        outBuffer += itemValue0;
    }
    else if ( condition == 1 )
    {
        outBuffer += itemValue1;
    }
    else if ( condition == 2 )
    {
        outBuffer += itemValue2;
    }
    else
    {
        outBuffer += itemValue3;
    }
    outBuffer += "\",\n";
}

// -------------------------------------------------------------------------------------------------//
//
// (If JSON, open the object and)
// Output sensor type, time stamp, and semi-cooked toggle.
//
// -------------------------------------------------------------------------------------------------//
void antProcessing::createOutputHeader
(
    const amString &sensorID,
    const amString &timeStampBuffer
)
{
    if ( outputAsJSON )
    {
        outBuffer += C_JSON_OPEN;
        outBuffer += "\n";

        outBuffer += C_JSON_INDENT;
        outBuffer += "\"";
        outBuffer += C_SENSOR_JSON;
        outBuffer += "\": \"";
        outBuffer += sensorID;
        outBuffer += "\",\n";

        outBuffer += C_JSON_INDENT;
        outBuffer += "\"";
        outBuffer += C_TIMESTAMP_JSON;
        outBuffer += "\": ";
        outBuffer += timeStampBuffer;
        outBuffer += ",\n";

        outBuffer += C_JSON_INDENT;
        outBuffer += "\"";
        outBuffer += C_SEMI_COOKED_JSON;
        outBuffer += "\": ";
        outBuffer += ( semiCookedOut ? C_TRUE_JSON : C_FALSE_JSON );
        outBuffer += ",\n";
    }
    else
    {
        outBuffer += sensorID;
        outBuffer += "\t";
        outBuffer += timeStampBuffer;
        if ( semiCookedOut )
        {
            outBuffer += "\t";
            outBuffer += C_SEMI_COOKED_SYMBOL_AS_STRING;
        }
    }
}

// -------------------------------------------------------------------------------------------------//
//
// Output ant2txt version (and if JSON, close the object)
//
// -------------------------------------------------------------------------------------------------//
void antProcessing::appendOutputFooter
(
    const amString &versionString
)
{
    if ( outputAsJSON )
    {
        outBuffer += C_JSON_INDENT;
        outBuffer += "\"program name\": \"";
        outBuffer += programName;
        outBuffer += "\",\n";

        outBuffer += C_JSON_INDENT;
        outBuffer += "\"version\": \"";
        outBuffer += versionString;
        outBuffer += "\"\n";

        outBuffer += C_JSON_CLOSE;
        outBuffer += "\n";
    }
    else
    {
        outBuffer += "\t";
        outBuffer += versionString;
    }
}

void antProcessing::createUnknownDeviceTypeString
(
    int             deviceType,
    int             deviceID,
    const amString &timeStampBuffer,
    BYTE   payLoad[]
)
{
    amString sensorID( C_UNKNOWN_TYPE_HEAD);
    sensorID += amString( deviceType );
    sensorID += "_";
    sensorID += amString( deviceID );
    if ( diagnostics )
    {
        appendDiagnosticsLine( "Unknown device Type", sensorID );
    }
    createOutputHeader( sensorID, timeStampBuffer );

    for ( int counter = 0; counter < C_ANT_PAYLOAD_LENGTH; ++counter )
    {
        if ( diagnostics )
        {
            appendDiagnosticsLine( "payLoad", counter, payLoad[ counter ] );
        }
        if ( outputAsJSON )
        {
            amString payload( "payLoad[ " );
            payload += amString( counter );
            payload += " ]";
            appendJSONItem( payload, payLoad[ counter ] );
        }
        else
        {
            appendOutput( payLoad[ counter ] );
        }
    }
    appendOutputFooter( b2tVersion );
}

// ---------------------------------------------------------------------------------
// te: This needs reworking - ANT-FS Documentation required
// ---------------------------------------------------------------------------------
bool antProcessing::createCommonResultStringPage67
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    value1,
    unsigned int    value2,
    unsigned int    value3,
    unsigned int    value4,
    unsigned int    value5
)
{
    // For now semi-cooked and fully cooked output are the same
    bool result = true;

    if ( outputPage )
    {
        int dataPage = 67;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( outputAsJSON )
    {
        appendJSONItem( "status byte 1",   value1 );
        appendJSONItem( "status byte 2",   value2 );
        appendJSONItem( "authentication",  value3 );
        appendJSONItem( "device type",     value4 );
        appendJSONItem( "manufacturer id", value5 );
    }
    else
    {
        appendOutput( value1 );
        appendOutput( value2 );
        appendOutput( value3 );
        appendOutput( value4 );
        appendOutput( value5 );
    }
    return result;
}

// ---------------------------------------------------------------------------------
// te: This needs reworking - ANT-FS Documentation required
// ---------------------------------------------------------------------------------
bool antProcessing::createCommonResultStringPage68
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    value1,
    unsigned int    value2,
    unsigned int    value3,
    unsigned int    value4
)
{
    // For now semi-cooked and fully cooked output are the same
    bool result = true;

    if ( outputPage )
    {
        int dataPage = 68;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( outputAsJSON )
    {
        appendJSONItem( "command/response id", value1 );
        appendJSONItem( "channel frequency",   value2 );
        appendJSONItem( "channel period",      value3 );
        appendJSONItem( "host serial number",  value4 );
    }
    else
    {
        appendOutput( value1 );
        appendOutput( value2 );
        appendOutput( value3 );
        appendOutput( value4 );
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
// Page Number 70 (0x46)
// -------------------------------------------------------------------------------------------------//
bool antProcessing::createCommonResultStringPage70
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    descriptor1,
    unsigned int    descriptor2,
    unsigned int    requestedResponse,
    unsigned int    requestedPage,
    unsigned int    commandType
)
{
    bool result = true;

    if ( outputPage )
    {
        int dataPage = 70;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( outputAsJSON )
    {
        if ( !semiCookedOut && ( descriptor1 == 255 ) )
        {
            appendJSONItem( "descriptor byte 1 page", C_NONE_ID );
        }
        else
        {
            appendJSONItem( "descriptor byte 1 page", descriptor1 );
        }

        if ( !semiCookedOut && ( descriptor2 == 255 ) )
        {
            appendJSONItem( "descriptor byte 2 page", C_NONE_ID );
        }
        else
        {
            appendJSONItem( "descriptor byte 2 page", descriptor2 );
        }

        if ( semiCookedOut )
        {
            appendJSONItem( "requested response",    requestedResponse );
            appendJSONItem( "requested page number", requestedPage );
            appendJSONItem( "command type",          commandType );
        }
        else
        {
            if ( requestedResponse == 0 )
            {
                appendJSONItem( "number of transmisions", C_INVALID_JSON );
            }
            else if ( requestedResponse == 128 )
            {
                appendJSONItem( "number of transmisions", "until success acknowledged" );
            }
            else
            {
                appendJSONItem( "number of transmisions", requestedResponse & 0x80 );
                appendJSONItem( "acknowledgement reply", ( requestedResponse & 128 ) ? C_TRUE_JSON : C_FALSE_JSON );
            }
            appendJSONItem( "requested page number", requestedPage );
            appendJSONItem( "command type", ( commandType == 1 ) ? "data page" : "ant fs session" );
        }
    }
    else
    {
        // Power Meters the page number is already identified by the device type (as PWRB46 - 46 = 0x70)

        if ( semiCookedOut )
        {
            appendOutput( descriptor1 );
            appendOutput( descriptor2 );
            appendOutput( requestedResponse );
            appendOutput( requestedPage );
            appendOutput( commandType );
        }
        else
        {
            appendOutput( "DESCRIPTOR_BYTE_1" );
            if ( descriptor1 == 255 )
            {
                appendOutput( "NONE" );
            }
            else
            {
                appendOutput( descriptor1 );
            }

            appendOutput( "DESCRIPTOR_BYTE_2" );
            if ( descriptor2 == 255 )
            {
                appendOutput( "NONE" );
            }
            else
            {
                appendOutput( descriptor2 );
            }


            if ( requestedResponse == 0 )
            {
                appendOutput( C_TRANSMIT_INVALID );
            }
            else if ( requestedResponse == 128 )
            {
                appendOutput( C_TRANSMIT_UNTIL_SUCCESS_ACKNOWLEDGED );
            }
            else
            {
                appendOutput( C_TRANSMIT_NB_TIMES );
                appendOutput( ( unsigned int ) requestedResponse & 0x7F );
                appendOutput( ( requestedResponse & 128 ) ? C_REPLY_ACKNOWLEDGE : C_NO_REPLY_ACKNOWLEDGE );
            }

            appendOutput( C_REQUESTED_PAGE_NO );
            appendOutput( requestedPage );
            appendOutput( ( commandType == 1 ) ? C_DATA_PAGE : C_ANT_FS_SESSION );
        }
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
// Page Number 80 (0x50)
// -------------------------------------------------------------------------------------------------//
bool antProcessing::createCommonResultStringPage80
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    manufacturerID,
    unsigned int    hardwareRevision,
    unsigned int    modelNumber
)
{
    bool result = true;

    if ( outputPage )
    {
        int dataPage = 80;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( outputAsJSON )
    {
        appendJSONItem( C_MANUFACTURER_JSON,      manufacturerID );
        appendJSONItem( C_HARDWARE_REVISION_JSON, hardwareRevision );
        appendJSONItem( C_MODEL_NUMBER_JSON,      modelNumber );
    }
    else
    {
        // Power Meters the page number is already identified by the device type (as PWRB50 - 50 = 0x80)
        appendOutput( manufacturerID );
        appendOutput( hardwareRevision );
        appendOutput( modelNumber );
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
// Page Number 81 (0x51)
// -------------------------------------------------------------------------------------------------//
bool antProcessing::createCommonResultStringPage81
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    serialNumber,
    unsigned int    softwareRevision
)
{
    bool result = true;

    if ( outputPage )
    {
        int dataPage = 81;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( outputAsJSON )
    {
        // Power Meters the page number is already identified by the device type (as PWRB51 - 51 = 0x81)
        if ( !semiCookedOut && ( int ( serialNumber ) == -1 ) )
        {
            appendJSONItem( C_SERIAL_NUMBER_JSON, C_NONE_ID );
        }
        else
        {
            appendJSONItem( C_SERIAL_NUMBER_JSON, serialNumber );
        }
        appendJSONItem( C_SOFTWARE_REVISION_JSON, softwareRevision );
    }
    else
    {
        // Power Meters the page number is already identified by the device type (as PWRB51 - 51 = 0x81)
        if ( semiCookedOut || ( ( int ) serialNumber >= 0 ) )
        {
            appendOutput( serialNumber );
        }
        else
        {
            appendOutput( "NO_SERIAL_NUMBER" );
        }
        appendOutput( softwareRevision );
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
// Page Number 82 (0x52)
// -------------------------------------------------------------------------------------------------//
bool antProcessing::createCommonResultStringPage82
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    voltage256,
    unsigned int    status,
    unsigned int    deltaOperatingTime,
    unsigned int    resolution,
    unsigned int    nbBatteries,
    unsigned int    batteryID
)
{
    bool result = true;

    if ( outputPage )
    {
        int dataPage = 82;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "voltage256",           voltage256 );
            appendJSONItem( "battery status index", status );
            appendJSONItem( "operating time",       deltaOperatingTime );
            appendJSONItem( "resolution",           resolution );
            appendJSONItem( "number of batteries",  nbBatteries );
            appendJSONItem( "battery no.",          batteryID );
        }
        else
        {
            appendOutput( voltage256 );
            appendOutput( status );
            appendOutput( deltaOperatingTime );
            appendOutput( resolution );
            appendOutput( nbBatteries );
            appendOutput( batteryID );
        }
    }
    else
    {
        amString batteryStatus;
        double   voltageDbl         = ( double ) voltage256 / 256.0;
        double   totalOperatingTime = getTotalOperationTime( sensorID );
        double   auxDouble          = ( double ) deltaOperatingTime;

        auxDouble          *= ( double ) resolution;
        totalOperatingTime += auxDouble;
        setTotalOperationTime( sensorID, totalOperatingTime );

        getBatteryStatus( batteryStatus, status, outputAsJSON );

        if ( outputAsJSON )
        {
            if ( nbBatteries == 0 )
            {
                appendJSONItem( "number of batteries", C_N_A_JSON );
                appendJSONItem( "battery no.",         C_N_A_JSON );
            }
            else
            {
                appendJSONItem( "number of batteries", nbBatteries );
                appendJSONItem( "battery no.",         batteryID );
            }
            appendJSONItem( "voltage",              voltageDbl, 2 );
            appendJSONItem( "battery status",       batteryStatus );
            appendJSONItem( "total operating time", totalOperatingTime, timePrecision );
        }
        else
        {
            unsigned int localNbBatteries = ( nbBatteries == 0 ) ? 1 : nbBatteries;
            appendOutput( localNbBatteries );
            appendOutput( batteryID );
            appendOutput( voltageDbl, 2 );
            appendOutput( batteryStatus );
            appendOutput( totalOperatingTime, timePrecision );
        }
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
// Page Number 83 (0x53)
// -------------------------------------------------------------------------------------------------//
bool antProcessing::createCommonResultStringPage83
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    seconds,
    unsigned int    minutes,
    unsigned int    hours,
    unsigned int    weekDayNo,
    unsigned int    monthDay,
    unsigned int    month,
    unsigned int    year
)
{
    bool     result = true;
    amString timeString;
    amString dateString;
    amString wDayString;

    if ( outputPage )
    {
        int dataPage = 83;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( !semiCookedOut )
    {
        int year2000 = year + 2000;
        dateString = date2String( year2000, month, monthDay );
        timeString = time2String( hours, minutes, seconds );
        switch( weekDayNo )
        {
            case  0: wDayString = "SUN";
                     break;
            case  1: wDayString = "MON";
                     break;
            case  2: wDayString = "TUE";
                     break;
            case  3: wDayString = "WED";
                     break;
            case  4: wDayString = "THU";
                     break;
            case  5: wDayString = "FRI";
                     break;
            case  6: wDayString = "SAT";
                     break;
            default: wDayString = C_UNKNOWN;
                     break;
        }
        wDayString.toLower();
    }

    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "year", year );
            appendJSONItem( "month", month );
            appendJSONItem( "monthDay", monthDay );
            appendJSONItem( "week day", weekDayNo );
            appendJSONItem( "hours", hours );
            appendJSONItem( "minutes", minutes );
            appendJSONItem( "seconds", seconds );
        }
        else
        {
            appendJSONItem( "date", dateString );
            appendJSONItem( "time", timeString );
            appendJSONItem( "week day", wDayString );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( seconds );
            appendOutput( minutes );
            appendOutput( hours );
            appendOutput( weekDayNo );
            appendOutput( monthDay );
            appendOutput( month );
            appendOutput( year );
        }
        else
        {
            appendOutput( "DATE" );
            appendOutput( dateString );
            appendOutput( "WEEK_DAY" );
            appendOutput( wDayString );
            appendOutput( "TIME" );
            appendOutput( timeString );
        }
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
// Page Number 84 (0x54)
// -------------------------------------------------------------------------------------------------//
bool antProcessing::createCommonResultStringPage84
(
    const amString &sensorID,
    bool            outputPage,
    unsigned int    subPage1,
    unsigned int    subPage2,
    unsigned int    dataField1,
    unsigned int    dataField2
)
{
    bool result = true;

    if ( outputPage )
    {
        int dataPage = 84;
        if ( outputAsJSON )
        {
            appendJSONItem( C_DATA_PAGE_JSON, dataPage );
        }
        else
        {
            appendOutput( dataPage );
        }
    }

    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "sub page 1",   subPage1 );
            appendJSONItem( "data field 1", dataField1 );
            appendJSONItem( "sub page 2",   subPage1 );
            appendJSONItem( "data field 2", dataField1 );
        }
        else
        {
            createDataPage84SubPage( sensorID, 1, subPage1, dataField1 );
            createDataPage84SubPage( sensorID, 2, subPage2, dataField2 );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( subPage1 );
            appendOutput( dataField1 );
            appendOutput( subPage2 );
            appendOutput( dataField2 );
        }
        else
        {
            createDataPage84SubPage( sensorID, 1, subPage1, dataField1 );
            createDataPage84SubPage( sensorID, 2, subPage2, dataField2 );
        }
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
//
// Output a message for a package in packet saver mode
//
// -------------------------------------------------------------------------------------------------//
void antProcessing::createPacketSaverModeString
(
    const amString      &timeStampBuffer,
    const BYTE *payLoad,
    BYTE        nbBytes
)
{
    createOutputHeader( C_PACKET_SAVER_NAME, timeStampBuffer );
    for ( int counter = 0; counter < nbBytes; ++counter )
    {
        if ( diagnostics )
        {
            appendDiagnosticsLine( "byte", counter, payLoad[ counter ] );
        }
        if ( outputAsJSON )
        {
            std::stringstream auxStream;
            auxStream << "byte[ " << counter << " ]";
            appendJSONItem( auxStream.str(), payLoad[ counter ] );
        }
        else
        {
            appendOutput( payLoad[ counter ] );
        }
    }
    appendOutputFooter( b2tVersion );
}

// -------------------------------------------------------------------------------------------------//
//
// Output a BRIDGE status message
//
// -------------------------------------------------------------------------------------------------//
void antProcessing::createBridgeString
(
    const amString &bridgeDeviceID,
    const amString &timeStampBuffer,
    const amString &bridgeName,
    const amString &bridgeMACAddess,
    const amString &firmwareVersion,
    int             voltageValue,
    int             powerIndicator,
    int             operatingMode,
    int             connectionStatus
)
{
    double dVoltage = semiCookedOut ? 0 : ( ( double ) ( 500 + voltageValue ) ) / 1000.0;

    createOutputHeader( bridgeDeviceID, timeStampBuffer );

    if ( outputAsJSON )
    {
        appendJSONItem( "bridge name",      bridgeName );
        appendJSONItem( "mac address",      bridgeMACAddess );
        appendJSONItem( "firmware version", firmwareVersion );

        if ( semiCookedOut )
        {
            appendJSONItem( "voltage", voltageValue );
            appendJSONItem( "power indicator", powerIndicator );
            appendJSONItem( "operating mode", operatingMode );
            appendJSONItem( "connection status", connectionStatus );
        }
        else
        {
            appendJSONItem( "voltage", dVoltage, 3 );
            appendJSONItem( "power indicator", powerIndicator ? "E" : "I" );
            appendJSONItem( "operating mode", operatingMode );
            appendJSONItem( "connection status", connectionStatus ? "I" : "A" );
        }
    }
    else
    {
        appendOutput( bridgeName );
        appendOutput( bridgeMACAddess );
        appendOutput( firmwareVersion );
        if ( semiCookedOut )
        {
            appendOutput( voltageValue );
        }
        else
        {
            appendOutput( dVoltage, 3, "V" );
        }
        appendOutput( powerIndicator ? "E" : "I" );
        appendOutput( operatingMode );
        appendOutput( connectionStatus ? "I" : "A" );
    }
    appendOutputFooter( b2tVersion );
}

// -------------------------------------------------------------------------------------------------//
//
// Output a PACKET SAVER MODE message
//
// -------------------------------------------------------------------------------------------------//
void antProcessing::createPacketSaverString
(
    const BYTE *line,
    int                  nbBytes,
    const amString      &timeStampBuffer
)
{
    createOutputHeader( C_PACKET_SAVER_NAME, timeStampBuffer );
    for ( int counter = 0; counter < nbBytes; ++counter )
    {
        if ( diagnostics )
        {
            appendDiagnosticsLine( "byte", counter, line[ counter ] );
        }
        if ( outputAsJSON )
        {
            std::stringstream auxStream;
            auxStream << "byte[ " << counter << " ]";
            appendJSONItem( auxStream.str(), line[ counter ] );
        }
        else
        {
            appendOutput( line[ counter ] );
        }
    }
    appendOutputFooter( b2tVersion );
}

// -------------------------------------------------------------------------------------------------//
//
// Output an UNKNOWN PACKET message
//
// -------------------------------------------------------------------------------------------------//
void antProcessing::createUnknownPacketString
(
    const BYTE *line,
    int                  nbBytes,
    const amString      &timeStampBuffer
)
{
    createOutputHeader( C_UNKOWN_PACKET_NAME, timeStampBuffer );
    for ( int counter = 0; counter < nbBytes; ++counter )
    {
        if ( diagnostics )
        {
            appendDiagnosticsLine( "byte", counter, line[ counter ] );
        }
        if ( outputAsJSON )
        {
            std::stringstream auxStream;
            auxStream << "byte[ " << counter << " ]";
            appendJSONItem( auxStream.str(), line[ counter ] );
        }
        else
        {
            appendOutput( line[ counter ] );
        }
    }
    appendOutputFooter( b2tVersion );
}

// -------------------------------------------------------------------------------------------------//
// -------------------------------------------------------------------------------------------------//
//
// Common Data Pages
//
// -------------------------------------------------------------------------------------------------//
// -------------------------------------------------------------------------------------------------//
bool antProcessing::processCommonPages
(
    const amString &sensorID,
    BYTE   payLoad[],
    bool            outputPage
)
{
    bool         result             = false;
    unsigned int dataPage           = payLoad[ 0 ];
    unsigned int descriptor1        = 0;
    unsigned int descriptor2        = 0;
    unsigned int requestedResponse  = 0;
    unsigned int requestedPage      = 0;
    unsigned int commandType        = 0;
    unsigned int modelNumber        = 0;
    unsigned int hardwareRevision   = 0;
    unsigned int manufacturerID     = 0;
    unsigned int serialNumber       = 0;
    unsigned int softwareRevision   = 0;
    unsigned int deltaOperatingTime = 0;
    unsigned int voltage256         = 0;
    unsigned int voltageInt         = 0;
    unsigned int nbBatteries        = 0;
    unsigned int batteryID          = 0;
    unsigned int status             = 0;
    unsigned int resolution         = 0;
    unsigned int seconds            = 0;
    unsigned int minutes            = 0;
    unsigned int hours              = 0;
    unsigned int weekDayNo          = 0;
    unsigned int monthDay           = 0;
    unsigned int month              = 0;
    unsigned int year               = 0;
    unsigned int subPage1           = 0;
    unsigned int subPage2           = 0;
    unsigned int dataField1         = 0;
    unsigned int dataField2         = 0;
    unsigned int value1             = 0;
    unsigned int value2             = 0;
    unsigned int value3             = 0;
    unsigned int value4             = 0;
    unsigned int value5             = 0;
    amString auxBuffer;

    switch ( dataPage )
    {
        case 67: // ---------------------------------------------------------------------------------
                 // Note: This needs reworking - ANT-FS Documentation required
                 // ---------------------------------------------------------------------------------

                 value1 = uChar2UInt( payLoad[ 1 ] );
                 value2 = uChar2UInt( payLoad[ 2 ] );
                 value3 = uChar2UInt( payLoad[ 3 ] );
                 value4 = uChar2UInt( payLoad[ 5 ], payLoad[ 4 ] );
                 value5 = uChar2UInt( payLoad[ 7 ], payLoad[ 6 ] );
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Status Byte 1",   payLoad[ 1 ], value1 );
                     appendDiagnosticsLine( "Status Byte 2",   payLoad[ 2 ], value2 );
                     appendDiagnosticsLine( "Authentication",  payLoad[ 3 ], value3 );
                     appendDiagnosticsLine( "Device Type",     payLoad[ 5 ], payLoad[ 4 ], value4 );
                     appendDiagnosticsLine( "Manufacturer ID", payLoad[ 7 ], payLoad[ 6 ], value5 );
                 }
                 result = createCommonResultStringPage67( sensorID, outputPage, value1, value2, value3, value4, value5 );
                 break;

        case 68: // ---------------------------------------------------------------------------------
                 // Note: This needs reworking - ANT-FS Documentation required
                 // ---------------------------------------------------------------------------------

                 value1 = uChar2UInt( payLoad[ 1 ] );
                 value2 = uChar2UInt( payLoad[ 2 ] );
                 value3 = uChar2UInt( payLoad[ 3 ] );
                 value4 = uChar2UInt( payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ] );
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Command/Response ID", payLoad[ 1 ], value1 );
                     appendDiagnosticsLine( "Channel Frequency",   payLoad[ 2 ], value2 );
                     appendDiagnosticsLine( "Channel Period",      payLoad[ 3 ], value3 );
                     appendDiagnosticsLine( "Host Serial Number",  payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ], value4 );
                 }
                 result = createCommonResultStringPage68( sensorID, outputPage, value1, value2, value3, value4 );
                 break;

        case 70: descriptor1       = uChar2UInt( payLoad[ 3 ] );
                 descriptor2       = uChar2UInt( payLoad[ 4 ] );
                 requestedResponse = uChar2UInt( payLoad[ 5 ] );
                 requestedPage   = uChar2UInt( payLoad[ 6 ] );
                 commandType       = uChar2UInt( payLoad[ 7 ] );
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Descriptor Byte 1",  payLoad[ 3 ], descriptor1 );
                     appendDiagnosticsLine( "Descriptor Byte 2",  payLoad[ 4 ], descriptor2 );
                     appendDiagnosticsLine( "Requested Response", payLoad[ 5 ], requestedResponse );
                     appendDiagnosticsLine( "Requested Page ",  payLoad[ 6 ], requestedPage );
                     appendDiagnosticsLine( "Command Type",       payLoad[ 7 ], commandType );
                 }
                 result = createCommonResultStringPage70( sensorID, outputPage, descriptor1, descriptor2, requestedResponse, requestedPage, commandType );
                 break;

        case 80: modelNumber      = uChar2UInt( payLoad[ 7 ], payLoad[ 6 ] );
                 hardwareRevision = uChar2UInt( payLoad[ 3 ] );
                 manufacturerID   = uChar2UInt( payLoad[ 5 ], payLoad[ 4 ] );
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Hardware Revision", payLoad[ 3 ],               hardwareRevision );
                     appendDiagnosticsLine( "Manufacturer ID",   payLoad[ 5 ], payLoad[ 4 ], manufacturerID );
                     appendDiagnosticsLine( "Model Number",      payLoad[ 7 ], payLoad[ 6 ], modelNumber );
                 }
                 result = createCommonResultStringPage80( sensorID, outputPage, manufacturerID, hardwareRevision, modelNumber );
                 break;

        case 81: serialNumber     = uChar2UInt( payLoad[ 3 ] );
                 softwareRevision = uChar2UInt( payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ] );
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Software Revision", payLoad[ 7 ], payLoad[ 6 ], payLoad[ 5 ], payLoad[ 4 ], softwareRevision );
                     appendDiagnosticsLine( "Serial Number",     payLoad[ 3 ], serialNumber );
                 }
                 result = createCommonResultStringPage81( sensorID, outputPage, serialNumber, softwareRevision );
                 break;

        case 82: // - - Page 82 (0x52) - Battery status - -
                 //
                 // Byte 2: Identifies the battery in the system to which this battery status
                 //         pertains and specifies how many batteries are available in the system.
                 //         Bits 0-3: Number of Batteries
                 //         Bits 4-7: Identifier
                 //         Set to 0xFF if not used.
                 // Byte 7: Bits 0-3: Integer part of the battery voltage
                 //         Bits 4-6: Battery status (index in battery status array)
                 //         Bit    7: Toggle: If 0 operating time is to be multiplied by 32, if it is 1, by 2.

                 nbBatteries        = ( payLoad[ 2 ] == 0xFF ) ? 0 : ( payLoad[ 2 ] & 0x0F );
                 batteryID          = ( payLoad[ 2 ] == 0xFF ) ? 0 : ( ( payLoad[ 2 ] & 0xF0 ) >> 4 );
                 deltaOperatingTime = uChar2UInt( payLoad[ 5 ], payLoad[ 4 ], payLoad[ 3 ] );
                 voltageInt         = uChar2UInt( payLoad[ 7 ] ) & 0x0F;
                 voltage256         = uChar2UInt( payLoad[ 6 ] ) + ( voltageInt << 8 );
                 status             = ( payLoad[ 7 ] >> 4 ) & 0x07;
                 resolution         = ( payLoad[ 7 ] & 0x80 ) ? 16 : 2;
                 if ( diagnostics )
                 {
                     double   voltageDbl = ( double ) voltage256 / 256.0;
                     auxBuffer = amString( voltageDbl, 3 );
                     auxBuffer += " - Second byte PLUS lower 4 bits of first byte)";
                     appendDiagnosticsLine( "Number of Batteries",   payLoad[ 2 ],               nbBatteries,  "(Lower 4 bits)" );
                     appendDiagnosticsLine( "Battery ID",            payLoad[ 2 ],               batteryID,    "(Upper 4 bits)" );
                     appendDiagnosticsLine( "Operating Time",        payLoad[ 5 ], payLoad[ 4 ], payLoad[ 3 ], deltaOperatingTime );
                     appendDiagnosticsLine( "Battery Voltage * 256", payLoad[ 7 ], payLoad[ 6 ], voltage256,   auxBuffer );
                     appendDiagnosticsLine( "Battery Status ID",     payLoad[ 7 ],               status,       "(bits 4-6)" );
                     appendDiagnosticsLine( "Resolution",            payLoad[ 7 ],               resolution,   "(7th bit: [0/1] for [32/2] seconds)" );
                 }
                 result = createCommonResultStringPage82( sensorID, outputPage, voltage256, status, deltaOperatingTime, resolution, nbBatteries, batteryID );
                 break;

        case 83: // Time And Date
                 seconds   = uChar2UInt( payLoad[ 1 ] );    // Seconds (0-59)
                 minutes   = uChar2UInt( payLoad[ 2 ] );    // Minutes (0-59)
                 hours     = uChar2UInt( payLoad[ 3 ] );    // Hours (0-24)
                 weekDayNo = uChar2UInt( payLoad[ 4 ] );    // Day of Week (0-6, Sun = 0)
                 monthDay  = uChar2UInt( payLoad[ 5 ] );    // Day of Month (1-31)
                 month     = uChar2UInt( payLoad[ 6 ] );    // Month (1-12)
                 year      = uChar2UInt( payLoad[ 7 ] );    // Year since 2000 (0-255)
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Seconds",         payLoad[ 1 ], seconds );
                     appendDiagnosticsLine( "Minutes",         payLoad[ 2 ], minutes );
                     appendDiagnosticsLine( "Hours",           payLoad[ 3 ], hours );
                     appendDiagnosticsLine( "Week Day Number", payLoad[ 4 ], weekDayNo );
                     appendDiagnosticsLine( "Month Day",       payLoad[ 5 ], monthDay );
                     appendDiagnosticsLine( "Month",           payLoad[ 6 ], month );
                     appendDiagnosticsLine( "Year",            payLoad[ 7 ], year );
                 }
                 result = createCommonResultStringPage83( sensorID, outputPage, seconds, minutes, hours, weekDayNo, monthDay, month, year );
                 break;

        case 84: // Weather Data
                 subPage1   = uChar2UInt( payLoad[ 2 ] );                      // SubPage1 (determines what type of data dataField1 is)
                 subPage2   = uChar2UInt( payLoad[ 3 ] );                      // SubPage2 (determines what type of data dataField2 is)
                 dataField1 = uChar2UInt( payLoad[ 5 ], payLoad[ 4 ] );        // Data Value 1
                 dataField2 = uChar2UInt( payLoad[ 7 ], payLoad[ 6 ] );        // Data Value 2
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Sub Page 1",   payLoad[ 2 ], subPage1 );
                     appendDiagnosticsLine( "Data Field 1", payLoad[ 5 ], payLoad[ 4 ], dataField1 );
                     appendDiagnosticsLine( "Sub Page 2",   payLoad[ 3 ], subPage2 );
                     appendDiagnosticsLine( "Data Field 2", payLoad[ 7 ], payLoad[ 6 ], dataField2 );
                 }
                 result = createCommonResultStringPage84( sensorID, outputPage, subPage1, subPage2, dataField1, dataField2 );
                 break;

        default: // - - Unknown Page - - - - - - - - - - - - - - - - -
                 if ( diagnostics )
                 {
                     appendDiagnosticsLine( "Unknown Data Page" );
                     appendDiagnosticsLine( "data page", payLoad[ 0 ], dataPage );
                     for ( int counter = 1; counter < C_ANT_PAYLOAD_LENGTH; ++counter )
                     {
                         appendDiagnosticsLine( "payLoad", counter, payLoad[ counter ] );
                     }
                 }
    }
    return result;
}

bool antProcessing::processCommonPagesSemiCooked
(
    const amSplitString &words,
    unsigned int         startCounter,
    unsigned int         dataPage,
    bool                 outputPage
)
{
    bool         result  = false;
    unsigned int nbWords = words.size();

    if ( nbWords > 5 )   // 5 is currently the smallest
    {
        bool           dataPageOK         = false;
        unsigned int   counter            = startCounter;
        unsigned int   seconds            = 0;
        unsigned int   minutes            = 0;
        unsigned int   hours              = 0;
        unsigned int   weekDayNo          = 0;
        unsigned int   monthDay           = 0;
        unsigned int   month              = 0;
        unsigned int   year               = 0;
        unsigned int   subPage1           = 0;
        unsigned int   subPage2           = 0;
        unsigned int   dataField1         = 0;
        unsigned int   dataField2         = 0;
        unsigned int   voltage256         = 0;
        unsigned int   status             = 0;
        unsigned int   resolution         = 0;
        unsigned int   deltaOperatingTime = 0;
        unsigned int   nbBatteries        = 0;
        unsigned int   batteryID          = 0;
        unsigned int   manufacturerID     = 0;
        unsigned int   hardwareRevision   = 0;
        unsigned int   modelNumber        = 0;
        unsigned int   serialNumber       = 0;
        unsigned int   softwareRevision   = 0;
        unsigned int   descriptor1        = 0;
        unsigned int   descriptor2        = 0;
        unsigned int   requestedResponse  = 0;
        unsigned int   requestedPage    = 0;
        unsigned int   commandType        = 0;
        unsigned int   value1             = 0;
        unsigned int   value2             = 0;
        unsigned int   value3             = 0;
        unsigned int   value4             = 0;
        unsigned int   value5             = 0;
        unsigned int   value6             = 0;
        unsigned int   value7             = 0;
        const amString sensorID           = words[ 0 ];

        switch ( dataPage )
        {
            case 67: // - - Page 67 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 11 )
                     {
                         value1 = words[ counter++ ].toUInt();
                         value2 = words[ counter++ ].toUInt();
                         value3 = words[ counter++ ].toUInt();
                         value4 = words[ counter++ ].toUInt();
                         value5 = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "Status Byte 1",   words[ counter++ ], value1 );
                             appendDiagnosticsLine( "Status Byte 2",   words[ counter++ ], value2 );
                             appendDiagnosticsLine( "Authentication",  words[ counter++ ], value3 );
                             appendDiagnosticsLine( "Device Type",     words[ counter++ ], value4 );
                             appendDiagnosticsLine( "Manufacturer ID", words[ counter++ ], value5 );
                         }
                         result = createCommonResultStringPage67( sensorID, outputPage, value1, value2, value3, value4, value5 );
                         break;
                     }
            case 68: // - - Page 68 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 11 )
                     {
                         value1 = words[ counter++ ].toUInt();
                         value2 = words[ counter++ ].toUInt();
                         value3 = words[ counter++ ].toUInt();
                         value4 = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "Command/Response ID", words[ counter++ ], value1 );
                             appendDiagnosticsLine( "Channel Frequency",   words[ counter++ ], value2 );
                             appendDiagnosticsLine( "Channel Period",      words[ counter++ ], value3 );
                             appendDiagnosticsLine( "Host Serial Number",  words[ counter++ ], value4 );
                         }
                         result = createCommonResultStringPage68( sensorID, outputPage, value1, value2, value3, value4 );
                         break;
                     }
            case 70: // - - Page 70 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 8 )
                     {
                         descriptor1       = words[ counter++ ].toUInt();
                         descriptor2       = words[ counter++ ].toUInt();
                         requestedResponse = words[ counter++ ].toUInt();
                         requestedPage     = words[ counter++ ].toUInt();
                         commandType       = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "Descriptor Byte 1",  words[ counter++ ], descriptor1 );
                             appendDiagnosticsLine( "Descriptor Byte 2",  words[ counter++ ], descriptor2 );
                             appendDiagnosticsLine( "Requested Response", words[ counter++ ], requestedResponse );
                             appendDiagnosticsLine( "Requested Page ",  words[ counter++ ], requestedPage );
                             appendDiagnosticsLine( "Command Type",       words[ counter++ ], commandType );
                         }
                         result = createCommonResultStringPage70( sensorID, outputPage, descriptor1, descriptor2, requestedResponse, requestedPage, commandType );
                     }
                     break;
            case 80: // - - Page 80 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 6 )
                     {
                         manufacturerID   = words[ counter++ ].toUInt();
                         hardwareRevision = words[ counter++ ].toUInt();
                         modelNumber      = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "Hardware Revision", words[ counter++ ], hardwareRevision );
                             appendDiagnosticsLine( "Manufacturer ID",   words[ counter++ ], manufacturerID );
                             appendDiagnosticsLine( "Model Number",      words[ counter++ ], modelNumber );
                         }
                         result = createCommonResultStringPage80( sensorID, outputPage, manufacturerID, hardwareRevision, modelNumber );
                     }
                     break;
            case 81: // - - Page 81 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 5 )
                     {
                         serialNumber     = words[ counter++ ].toUInt();
                         softwareRevision = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "Serial Number",     words[ counter++ ], serialNumber );
                             appendDiagnosticsLine( "Software Revision", words[ counter++ ], softwareRevision );
                         }
                         result = createCommonResultStringPage81( sensorID, outputPage, serialNumber, softwareRevision );
                     }
                     break;
            case 82: // - - Page 82 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 6 )
                     {
                         voltage256         = words[ counter++ ].toUInt();
                         status             = words[ counter++ ].toUInt();
                         deltaOperatingTime = words[ counter++ ].toUInt();
                         resolution         = words[ counter++ ].toUInt();
                         nbBatteries        = words[ counter++ ].toUInt();
                         batteryID          = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "voltage256",         words[ counter++ ], voltage256 );
                             appendDiagnosticsLine( "status",             words[ counter++ ], status );
                             appendDiagnosticsLine( "deltaOperatingTime", words[ counter++ ], deltaOperatingTime );
                             appendDiagnosticsLine( "resolution",         words[ counter++ ], resolution );
                             appendDiagnosticsLine( "nbBatteries",        words[ counter++ ], nbBatteries );
                             appendDiagnosticsLine( "batteryID",          words[ counter++ ], batteryID );
                         }
                         result = createCommonResultStringPage82( sensorID, outputPage, voltage256, status, deltaOperatingTime, resolution, nbBatteries, batteryID );
                     }
                     break;
            case 83: // - - Page 83 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 10 )
                     {
                         seconds   = words[ counter++ ].toUInt();
                         minutes   = words[ counter++ ].toUInt();
                         hours     = words[ counter++ ].toUInt();
                         weekDayNo = words[ counter++ ].toUInt();
                         monthDay  = words[ counter++ ].toUInt();
                         month     = words[ counter++ ].toUInt();
                         year      = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "Seconds",         words[ counter++ ], seconds );
                             appendDiagnosticsLine( "Minutes",         words[ counter++ ], minutes );
                             appendDiagnosticsLine( "Hours",           words[ counter++ ], hours );
                             appendDiagnosticsLine( "Week Day Number", words[ counter++ ], weekDayNo );
                             appendDiagnosticsLine( "Month Day",       words[ counter++ ], monthDay );
                             appendDiagnosticsLine( "Month",           words[ counter++ ], month );
                             appendDiagnosticsLine( "Year",            words[ counter++ ], year );
                         }
                         result = createCommonResultStringPage83( sensorID, outputPage, seconds, minutes, hours, weekDayNo, monthDay, month, year );
                     }
                     break;
            case 84: // - - Page 84 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     //
                     dataPageOK = true;
                     if ( nbWords > 7 )
                     {
                         subPage1   = words[ counter++ ].toUInt();
                         dataField1 = words[ counter++ ].toUInt();
                         subPage2   = words[ counter++ ].toUInt();
                         dataField2 = words[ counter++ ].toUInt();
                         if ( diagnostics )
                         {
                             counter = startCounter;
                             appendDiagnosticsLine( "Sub Page 1",   words[ counter++ ], subPage1 );
                             appendDiagnosticsLine( "Data Field 1", words[ counter++ ], dataField1 );
                             appendDiagnosticsLine( "Sub Page 2",   words[ counter++ ], subPage2 );
                             appendDiagnosticsLine( "Data Field 2", words[ counter++ ], dataField2 );
                         }
                         result = createCommonResultStringPage84( sensorID, outputPage, subPage1, subPage2, dataField1, dataField2 );
                     }
                     break;
            default: // - - Unknown Page - - - - - - - - - - - - - - - - -
                     if ( diagnostics )
                     {
                         counter = startCounter - 1;
                         if ( dataPageOK )
                         {
                             appendDiagnosticsLine( "Input data vector too short for Data Page" );
                         }
                         else
                         {
                             appendDiagnosticsLine( "Unknown Data Page" );
                         }
                         appendDiagnosticsLine( "data page", words[ counter   ], dataPage );
                         appendDiagnosticsLine( "value1",    words[ counter++ ], value1 );
                         appendDiagnosticsLine( "value2",    words[ counter++ ], value2 );
                         appendDiagnosticsLine( "value3",    words[ counter++ ], value3 );
                         appendDiagnosticsLine( "value4",    words[ counter++ ], value4 );
                         appendDiagnosticsLine( "value5",    words[ counter++ ], value5 );
                         appendDiagnosticsLine( "value6",    words[ counter++ ], value6 );
                         appendDiagnosticsLine( "value7",    words[ counter++ ], value7 );
                     }
                     break;
        }
    }
    return result;
}

// -------------------------------------------------------------------------------------------------//
// Sub Page for Page Number 84
// -------------------------------------------------------------------------------------------------//
void antProcessing::createDataPage84SubPage
(
    const amString &sensorID,
    unsigned int    subPageNo,
    unsigned int    subPage,
    unsigned int    dataField
)
{
    int      tempAsInt10 = 0;
    int      precision   = 3;
    double   dblValue    = 0;
    amString auxString;

    switch ( subPage )
    {
        case  1: // Temperature
                 tempAsInt10 = NEGATE_BINARY_INT( dataField, 16 );
                 dblValue = ( double ) tempAsInt10 / 100.0;
                 if ( outputAsJSON )
                 {
                     appendJSONItem( "temperature value", dblValue, 3 );
                     appendJSONItem( "temperature unit", "celsius" );
                 }
                 else
                 {
                     appendOutput( "TEMP" );
                     appendOutput( dblValue, 3, "C" );
                 }
                 break;
        case  2: // Barometric Pressure
                 dblValue = ( double ) dataField / 100.0;
                 if ( outputAsJSON )
                 {
                     appendJSONItem( "barometric pressure value", dblValue, precision );
                     appendJSONItem( "barometric pressure unit", "pascal" );
                 }
                 else
                 {
                     appendOutput( "PRESSURE" );
                     appendOutput( dblValue, 3 );
                 }
                 break;
        case  3: // Humidity
                 dblValue = ( double ) dataField / 100.0;
                 if ( outputAsJSON )
                 {
                     appendJSONItem( "humidity value", dblValue, precision );
                     appendJSONItem( "humidity unit", "percent" );
                 }
                 else
                 {
                     appendOutput( "HUMIDITY" );
                     appendOutput( dblValue, 3, "\%" );
                 }
                 break;
        case  4: // Wind Speed
                 dblValue = ( double ) dataField / 100.0;  // 0.01 km/h
                 if ( outputAsJSON )
                 {
                     appendJSONItem( "wind speed value", dblValue, precision );
                     appendJSONItem( "wind speed unit", "km/h" );
                 }
                 else
                 {
                     appendOutput( "WIND_SPEED" );
                     appendOutput( dblValue, 3 );
                 }
                 break;
        case  5: // Wind Direction
                 dblValue = ( double ) dataField / 200.0;   // 0.005 Degrees
                 if ( outputAsJSON )
                 {
                     appendJSONItem( "wind direction value", dblValue, precision );
                     appendJSONItem( "wind direction unit", "degrees" );
                 }
                 else
                 {
                     appendOutput( "WIND_DIR" );
                     appendOutput( dblValue, 3 );
                 }
                 break;
        default: // Undefined
                 if ( outputAsJSON )
                 {
                     auxString = "sub page ";
                     auxString += amString( subPageNo );
                     appendJSONItem( auxString, C_UNKNOWN_JSON );
                 }
                 else
                 {
                     appendOutput( "UNKNOWN"  );
                 }
                 break;
    }
}

int antProcessing::outputData
(
    void
)
{
    if ( outputRaw && ( rawBuffer.size() > 0 ) )
    {
        if ( multicastWrite.isUp() )
        {
            errorCode = multicastWrite.write( rawBuffer, errorCode, errorMessage );
        }
        if ( writeStdout )
        {
            std::cout << rawBuffer << std::endl;
        }
        rawBuffer.clear();
    }
    if ( ( errorCode == 0 ) && diagnostics && ( diagnosticsBuffer.size() > 0 ) )
    {
        if ( multicastWrite.isUp() )
        {
            errorCode = multicastWrite.write( diagnosticsBuffer, errorCode, errorMessage );
        }
        if ( writeStdout )
        {
            std::cout << diagnosticsBuffer << std::endl;
        }
    }
    if ( ( errorCode == 0 ) && !outputRaw && ( outBuffer.size() > 0 ) )
    {
        if ( multicastWrite.isUp() )
        {
            errorCode = multicastWrite.write( outBuffer, errorCode, errorMessage );
            if ( errorCode == 0 )
            {
                errorCode = multicastWrite.write( "\n", errorCode, errorMessage );
            }
        }
        if ( writeStdout )
        {
            std::cout << outBuffer << std::endl;
        }
    }

    if ( errorCode == E_EMPTY_MESSAGE )
    {
        errorCode = 0;
    }

    return errorCode;
}

int antProcessing::readAntFromStream
(
    std::istream &inStream
)
{
    while ( errorCode == 0 )
    {
        resetOutBuffer();
        resetDiagnosticsBuffer();
        if ( semiCookedIn )
        {
            errorCode = readSemiCookedSingleLineFromStream( inStream );
        }
        else
        {
            resetRawBuffer();
            errorCode = readAntSingleLineFromStream( inStream );
        }
    }
    return errorCode;
}

int antProcessing::readSemiCookedSingleLineFromStream
(
    std::istream &inStream
)
{
    char line[ C_BUFFER_SIZE ];

    inStream.getline( line, C_BUFFER_SIZE );
    if ( strlen( line ) > 0 )
    {
        bool resultDevice = false;
        if ( semiCookedOut )
        {
            resultDevice = updateSensorSemiCooked( line );
        }
        else
        {
            resultDevice = processSensorSemiCooked( line );
        }
        errorCode = outputData();
    }

    if ( inStream.eof() || inStream.fail() )
    {
        errorCode = E_END_OF_FILE;
    }

    return errorCode;
}


int antProcessing::readAntSingleLineFromStream
(
    std::istream &inStream
)
{
    int  nbBytes = 0;
    BYTE byte    = 0;
    BYTE line[ C_BUFFER_SIZE ];

    while ( errorCode == 0 )
    {
        byte = inStream.get();
        if ( inStream.eof() || inStream.fail() )
        {
            errorCode = ant2txtLine( line, nbBytes );
            if ( errorCode == 0 )
            {
                errorCode = E_END_OF_FILE;
            }
            break;
        }

        line[ nbBytes ] = byte;
        ++nbBytes;
        if ( nbBytes == 1 )
        {
            if ( ( unsigned int ) line[ 0 ] != ( unsigned int ) 'A' )
            {
                if ( outputRaw || diagnostics )
                {
                    rawBuffer += amString( line[ 0 ] );
                }
                nbBytes = 0;
            }
        }
        else if ( nbBytes == 2 )
        {
            if ( ( unsigned int ) line[ 1 ] != ( unsigned int ) 'N' )
            {
                if ( outputRaw || diagnostics )
                {
                    rawBuffer += amString( line[ 0 ] );
                    rawBuffer += " ";
                    rawBuffer += amString( line[ 1 ] );
                }
                nbBytes = 0;
            }
        }
        else if ( ( ( unsigned int ) line[ nbBytes - 1 ] == ( unsigned int ) 'N' ) &&
                  ( ( unsigned int ) line[ nbBytes - 2 ] == ( unsigned int ) 'A' ) )
        {
            // Put 'A' and 'N' back into the stream (they belong to the next message).
            inStream.putback( line[ nbBytes - 1 ] );
            inStream.putback( line[ nbBytes - 2 ] );
            nbBytes -= 2;

            errorCode = ant2txtLine( line, nbBytes );
            break;
        }
    }

    return errorCode;
}

int antProcessing::ant2txtLine
(
    const BYTE *line,
    int                  nbBytes
)
{
    const char    operatingModeString7[] = "continuous scan mode";
    const char    operatingModeString6[] = "proximity scan mode";
    const char    operatingModeString5[] = "ANT-FS access point mode";
    const char    operatingModeString0[] = "undefined";

    char          bridgeName[ C_WASP_NAME_LENGTH ];
    char          deviceIDCString[ C_SMALL_BUFFER_SIZE ];
    char          operatingModeString[ C_MEDIUM_BUFFER_SIZE ];
    BYTE payLoad[ C_ANT_PAYLOAD_LENGTH ];
    BYTE packetID[ 2 ];
    BYTE rssi[ 2 ];
    BYTE timeStamp[ 2 ];
    BYTE reserved[ 2 ];
    BYTE voltage[ 2 ];
    BYTE macAddress[ C_MAC_ADDRESS_LENGTH ];
    BYTE packetCommand      = 0;
    BYTE sequence           = 0;
    BYTE messageType        = 0;
    BYTE channel            = 0;
    BYTE messageFlag        = 0;
    BYTE deviceIDmsb        = 0;
    BYTE deviceIDlsb        = 0;
    BYTE transType          = 0;
    BYTE measurementType    = 0;
    BYTE thresholding       = 0;
    BYTE crc                = 0;
    BYTE majorVersion       = 0;
    BYTE minorVersion       = 0;
    BYTE connectionStatus   = 0;
    BYTE waspProductVersion = 0;
    BYTE powerIndicator     = 0;
    BYTE operatingMode      = 0;
    BYTE bridgeNameLength   = 0;
    int           counter            = 0;
    int           deviceType         = 0;
    int           byteCount          = 0;
    int           payLoadLength      = 0;
    int           deviceIDint        = 0;
    int           rssiValue          = 0;
    int           reservedValue      = 0;
    int           voltageValue       = 0;
    int           timeStampValue     = 0;
    size_t        charCount          = 0;
    amString      deviceID           = "";
    amString      timeStampBuffer;
    amDeviceType  resultDevice;

    resetOutBuffer();
    resetRawBuffer();
    resetDiagnosticsBuffer();

    getUnixTimeAsString( timeStampBuffer );

    if ( outputRaw || diagnostics )
    {
        charCount = 0;
        if ( diagnostics )
        {
            rawBuffer += "\nRAW> ";
        }
        for ( byteCount = 0; byteCount < nbBytes; ++byteCount )
        {
            if ( byteCount )
            {
                rawBuffer += " ";
            }
            rawBuffer += amString( line[ byteCount ] );
        }
    }

    if ( !outputRaw || diagnostics )
    {
        if ( diagnostics )
        {
            diagnosticsBuffer  = "\n\nDIAGNOSTICS:\n\nCOOKED>\n";
            appendDiagnosticsField( programName );
            diagnosticsBuffer += b2tVersion;
        }

        byteCount = 0;
        packetID[ 0 ] = line[ byteCount ];
        ++byteCount;
        packetID[ 1 ] = line[ byteCount ];
        ++byteCount;

        if ( diagnostics )
        {
            appendDiagnosticsField( "Packet ID" );
            diagnosticsBuffer += amString( packetID[ 0 ], packetID[ 1 ] );
        }

        packetCommand = line[ byteCount ];
        ++byteCount;
        if ( diagnostics )
        {
            appendDiagnosticsField( "Packet Command" );
            diagnosticsBuffer += amString( ( BYTE ) packetCommand );
            diagnosticsBuffer += "    (";
            diagnosticsBuffer += amString( ( unsigned int ) packetCommand );
            diagnosticsBuffer += ")";
        }

        if ( packetCommand == C_ANT_ASYNC_MSG )
        {
            sequence = line[ byteCount ];
            ++byteCount;
            if ( diagnostics )
            {
                appendDiagnosticsField( "Sequence Number" );
                diagnosticsBuffer += amString( ( BYTE ) sequence );
                diagnosticsBuffer += "    (";
                diagnosticsBuffer += amString( ( unsigned int ) sequence );
                diagnosticsBuffer += ")";
            }

            payLoadLength = ( int ) line[ byteCount ];
            ++byteCount;
            if ( diagnostics )
            {
                appendDiagnosticsField( "Payload Length" );
                diagnosticsBuffer += amString( ( unsigned int ) payLoadLength );
            }

            messageType = line[ byteCount ];
            ++byteCount;
            if ( diagnostics )
            {
                appendDiagnosticsField( "ANT message Type" );
                diagnosticsBuffer += amString( ( BYTE ) messageType );
                diagnosticsBuffer += "    (";
                diagnosticsBuffer += amString( ( unsigned int ) messageType );
                diagnosticsBuffer += ")";
            }

            channel = ( int ) line[ byteCount ];
            ++byteCount;
            if ( diagnostics )
            {
                appendDiagnosticsField( "Channel Number" );
                diagnosticsBuffer += amString( ( BYTE ) channel );
                diagnosticsBuffer += "    (";
                diagnosticsBuffer += amString( ( unsigned int ) channel );
                diagnosticsBuffer += ")";
            }

            bzero( ( void * ) payLoad, C_ANT_PAYLOAD_LENGTH );
            if ( diagnostics )
            {
                appendDiagnosticsField( "Payload" );
            }
            for ( counter = 0; ( counter < C_ANT_PAY_LOAD_COUNT ) && ( byteCount < nbBytes ); ++counter, ++byteCount )
            {
                payLoad[ counter ] = line[ byteCount ];
                if ( diagnostics )
                {
                    diagnosticsBuffer += toHex( payLoad[ counter ] );
                }
            }

            messageFlag = line[ byteCount ];
            ++byteCount;
            if ( diagnostics )
            {
                appendDiagnosticsField( "Extended Message Flag" );
                diagnosticsBuffer += amString( ( BYTE ) messageFlag );
                diagnosticsBuffer += "    (";
                diagnosticsBuffer += amString( ( unsigned int ) messageFlag );
                diagnosticsBuffer += ")";
            }

            if ( messageFlag & 0x80 )
            {
                deviceIDlsb = line[ byteCount ];
                ++byteCount;
                deviceIDmsb = line[ byteCount ];
                ++byteCount;
                deviceIDint = 256 * deviceIDmsb + deviceIDlsb;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Device ID" );
                    diagnosticsBuffer += amString(  deviceIDlsb, deviceIDmsb );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( deviceIDint );
                    diagnosticsBuffer += ")";
                }

                deviceType = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Device Type" );
                    diagnosticsBuffer += amString( ( BYTE ) deviceType );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) deviceType );
                    diagnosticsBuffer += ")";
                }

                transType = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Transaction Type" );
                    diagnosticsBuffer += amString( ( BYTE ) transType );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) transType );
                    diagnosticsBuffer += ")";
                }

                sprintf( deviceIDCString, "%d", deviceIDint );
                deviceID     = amString( deviceIDCString );
                resultDevice = processSensor( deviceType, deviceID, timeStampBuffer, payLoad );

                if ( resultDevice == OTHER_DEVICE )
                {
                    if ( outputUnknown )
                    {
                        createUnknownDeviceTypeString( deviceType, deviceIDint, timeStampBuffer, payLoad );
                    }
                    else
                    {
                        resetOutBuffer();
                    }
                }
                else if ( resultDevice == DEVICE_ERROR )
                {
                    std::cerr << "ERROR: " << errorMessage << std::endl;
                }
            }
            if ( messageFlag & 0x40 )
            {
                measurementType = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Measurement Type" );
                    diagnosticsBuffer += amString( ( BYTE ) measurementType );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) measurementType );
                    diagnosticsBuffer += ")";
                }

                rssi[ 0 ] = line[ byteCount ];
                ++byteCount;
                rssi[ 1 ] = line[ byteCount ];
                ++byteCount;
                rssiValue = 256 * rssi[ 1 ] + rssi[ 0 ];
                if ( diagnostics )
                {
                    appendDiagnosticsField( "RSSI" );
                    diagnosticsBuffer += amString( ( BYTE ) rssi[ 0 ], rssi[ 1 ] ); 
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) rssiValue );
                    diagnosticsBuffer += ")";
                }

                thresholding = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Thresholding" );
                    diagnosticsBuffer += amString( ( BYTE ) thresholding );
                }

                crc = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "CRC" );
                    diagnosticsBuffer += amString( ( BYTE ) crc );
                }
            }
            if ( messageFlag & 0x20 )
            {
                timeStamp[ 0 ] = line[ byteCount ];
                ++byteCount;
                timeStamp[ 1 ] = line[ byteCount ];
                ++byteCount;
                timeStampValue = 256 * timeStamp[ 1 ] + timeStamp[ 0 ];
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Timestamp" );
                    diagnosticsBuffer += amString( ( BYTE ) timeStamp[ 0 ], timeStamp[ 1 ] );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) timeStampValue );
                    diagnosticsBuffer += ")";
                }
            }
        }
        else if ( packetCommand == C_QUERY_RESP )
        {
            if ( outputBridge )
            {
                sequence = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Sequence Number" );
                    diagnosticsBuffer += amString( ( BYTE ) sequence );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) sequence );
                    diagnosticsBuffer += ")";
                }

                payLoadLength = ( int ) line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Payload Length" );
                    diagnosticsBuffer += amString( ( unsigned int ) payLoadLength );
                }

                reserved[ 0 ] = line[ byteCount ];
                ++byteCount;
                reserved[ 1 ] = line[ byteCount ];
                ++byteCount;
                reservedValue = 256 * reserved[ 1 ] + reserved[ 0 ];
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Reserved Bytes" );
                    diagnosticsBuffer += amString( ( BYTE ) reserved[ 0 ], reserved[ 1 ] );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) reservedValue );
                    diagnosticsBuffer += ")";
                }

                majorVersion = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Major version" );
                    diagnosticsBuffer += amString( ( BYTE ) majorVersion );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) majorVersion );
                    diagnosticsBuffer += ")";
                }

                minorVersion = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Minor Version" );
                    diagnosticsBuffer += amString( ( BYTE ) minorVersion );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) minorVersion );
                    diagnosticsBuffer += ")";
                }

                waspProductVersion = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "WASP Product Version" );
                    diagnosticsBuffer += amString( ( BYTE ) waspProductVersion );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) waspProductVersion );
                    diagnosticsBuffer += ")";
                }

                connectionStatus = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Connection Status" );
                    diagnosticsBuffer += amString( ( BYTE ) connectionStatus );
                    if ( connectionStatus == 0 )
                    {
                        diagnosticsBuffer += " (TCP In Use)";
                    }
                    else if ( connectionStatus == 1 )
                    {
                        diagnosticsBuffer += " (TCP In Available)";
                    }
                }

                reserved[ 0 ] = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Reserved Bytes" );
                    diagnosticsBuffer += amString( ( BYTE ) reserved[ 0 ], reserved[ 1 ] );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) reservedValue );
                    diagnosticsBuffer += ")";
                }

                voltage[ 0 ] = line[ byteCount ];
                ++byteCount;
                voltage[ 1 ] = line[ byteCount ];
                ++byteCount;
                voltageValue = voltage[ 1 ];
                voltageValue <<= 8;
                voltageValue += voltage[ 0 ];
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Battery Level (mV)" );
                    diagnosticsBuffer += amString( ( BYTE ) voltage[ 0 ], voltage[ 1 ] );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) voltageValue );
                    diagnosticsBuffer += ")";
                }

                powerIndicator = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Power Indicator" );
                    diagnosticsBuffer += amString( ( BYTE ) powerIndicator );
                    if ( powerIndicator == 1 )
                    {
                        diagnosticsBuffer += "  (Charging)";
                    }
                }

                operatingMode = line[ byteCount ];
                ++byteCount;
                *operatingModeString = 0;
                if ( operatingMode & ( 1 << 7 ) )
                {
                    strcat( operatingModeString, operatingModeString7 );
                }
                if ( operatingMode & ( 1 << 6 ) )
                {
                    if ( *operatingModeString != 0 )
                    {
                        strcat( operatingModeString, " | " );
                    }
                    strcat( operatingModeString, operatingModeString6 );
                }
                if ( operatingMode & ( 1 << 5 ) )
                {
                    if ( *operatingModeString != 0 )
                    {
                        strcat( operatingModeString, " | " );
                    }
                    strcat( operatingModeString, operatingModeString5 );
                }
                if ( *operatingModeString == 0 )
                {
                    if ( operatingMode & ( ( 1 << 4 ) + ( 1 << 3 ) + ( 1 << 2 ) + ( 1 << 1 ) ) )
                    {
                        strcat( operatingModeString, operatingModeString0 );
                    }
                }
                if ( diagnostics )
                {
                    appendDiagnosticsField( "Operating Mode" );
                    diagnosticsBuffer += amString( ( BYTE ) operatingMode );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += operatingModeString;
                    diagnosticsBuffer += ")";
                }

                bridgeNameLength = line[ byteCount ];
                ++byteCount;
                if ( diagnostics )
                {
                    appendDiagnosticsField( "WASP Name Length" );
                    diagnosticsBuffer += amString( ( BYTE ) bridgeNameLength );
                    diagnosticsBuffer += "    (";
                    diagnosticsBuffer += amString( ( unsigned int ) bridgeNameLength );
                    diagnosticsBuffer += ")";
                }
                for ( counter = 0; counter < ( int ) C_WASP_NAME_LENGTH; ++counter, ++byteCount )
                {
                    bridgeName[ counter ] = ( char ) line[ byteCount ];
                }
                if ( bridgeName[ strlen( bridgeName ) - 1 ] == ':' )
                {
                    bridgeName[ strlen( bridgeName ) - 1 ] = 0;
                }
                if ( diagnostics )
                {
                    appendDiagnosticsField( "WASP Name" );
                    diagnosticsBuffer += bridgeName;
                    appendDiagnosticsField( "MAC Address" );
                }
                ++byteCount;
                for ( counter = 0; counter < C_MAC_ADDRESS_LENGTH; ++counter, ++byteCount )
                {
                    macAddress[ counter ] = line[ byteCount ];
                    if ( diagnostics )
                    {
                        diagnosticsBuffer += " ";
                        diagnosticsBuffer += amString( ( BYTE ) macAddress[ counter ] );
                    }
                }

                char bridgeDeviceID [ C_TINY_BUFFER_SIZE ];
                char bridgeMACAddess[ C_TINY_BUFFER_SIZE ];
                char versionString  [ C_TINY_BUFFER_SIZE ];
                sprintf( bridgeDeviceID, "%s%02X%02X%02X", C_BRIDGE_MESSAGE_ID, macAddress[ 3 ], macAddress[ 4 ], macAddress[ 5 ] );
                sprintf( bridgeMACAddess, "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[ 0 ], macAddress[ 1 ], macAddress[ 2 ], macAddress[ 3 ], macAddress[ 4 ], macAddress[ 5 ] );
                sprintf( versionString, "%d.%d.%d", ( int ) majorVersion, ( int ) minorVersion, ( int ) waspProductVersion );

                createBridgeString
                (
                    bridgeDeviceID,
                    timeStampBuffer,
                    bridgeName,
                    bridgeMACAddess,
                    versionString,
                    voltageValue,
                    powerIndicator,
                    operatingMode,
                    connectionStatus
                );
            }
        }
        else if ( packetCommand == C_ANT_PACKET_SAVER )
        {
            resetOutBuffer();
            if ( outputUnknown )
            {
                createPacketSaverString( line, nbBytes, timeStampBuffer );
            }
        }
        else
        {
            resetOutBuffer();
            if ( outputUnknown )
            {
                createUnknownPacketString( line, nbBytes, timeStampBuffer );
            }
        }
    }

    if ( diagnostics )
    {
        diagnosticsBuffer += "\n\n\n------------------------------------------------------------------\n\n";
        outBuffer += diagnosticsBuffer;
        diagnosticsBuffer.clear();
    }

    errorCode = outputData();

    return errorCode;
}

int antProcessing::readAntFromMultiCast
(
    void
)
{
    if ( errorCode == 0 )
    {
        if ( interface == C_AUTO_INTERFACE )
        {
            amString localInterface;
            bool result = true;
            for ( unsigned int counter = 0; result && ( counter < C_MAX_INTERFACE_COUNT ); ++counter )
            {
                localInterface = C_INTERFACE_ROOT + amString( counter );
                errorCode = multicastRead.connect( localInterface, mcAddressIn, mcPortNoIn, timeOutSec, errorMessage );
                if ( errorCode != 0 )
                {
                    if ( diagnostics )
                    {
                        std::cerr << std::endl;
                        std::cerr << "Multicast connection for reading on interface \"";
                        std::cerr << localInterface << "\", IP address \"" << mcAddressIn << "\" and port number " << mcPortNoIn << " failed." << std::endl;
                        std::cerr << std::endl;
                    }
                }
                else
                {
                    result = false;
                    if ( diagnostics )
                    {
                        std::cerr << std::endl;
                        std::cerr << "Successfully created multicast connection for reading on interface \"";
                        std::cerr << localInterface << "\", IP address \"" << mcAddressIn << "\" and port number " << mcPortNoIn << "." << std::endl;
                        std::cerr << std::endl;
                    }
                }
            }
        }
        else
        {
            errorCode = multicastRead.connect( interface, mcAddressIn, mcPortNoIn, timeOutSec, errorMessage );
            if ( ( errorCode == 0 ) && diagnostics )
            {
                std::cerr << std::endl;
                std::cerr << "Successfully created multicast connection for reading on interface \"";
                std::cerr << interface << "\", IP address \"" << mcAddressIn << "\" and port number " << mcPortNoIn << "." << std::endl;
                std::cerr << std::endl;
            }
        }
    }

    if ( errorCode == 0 )
    {
        BYTE line[ C_BUFFER_SIZE ];
        int           nbBytes   = 1;

        resetOutBuffer();
        resetDiagnosticsBuffer();
        resetRawBuffer();

        while ( errorCode == 0 )
        {
            nbBytes = multicastRead.read( line, C_BUFFER_SIZE );
            if ( nbBytes < 0 )
            {
                size_t startCount = errorMessage.size();
                errorMessage.resize( startCount + C_BUFFER_SIZE );
                strerror_r( errno, &( errorMessage[ startCount ] ), C_BUFFER_SIZE );
                if ( errno == E_READ_TIMEOUT_C )
                {
                    errorCode     = E_READ_TIMEOUT;
                    appendErrorMessage( "Read source timed out.\n" );
                }
                else
                {
                    errorCode = E_READ_ERROR;
                    appendErrorMessage("Error while reading.\n" );
                }
            }
            else
            {
                errorCode = ant2txtLine( line, nbBytes );
            }
        }
    }

    multicastRead.close();

    return errorCode;
}

//-------------------------------------------------------------------------------------
// A package has the following syntax:
//
//  ANT+ messages
//
//     Byte  0 -  1: Packet Type Identifier
//                   (two byte header 'A', 'N' or '41', '4E' written in hex ASCII)
//     Byte       2: Packet Command Byte (Refer to table XXX), For ANT+ messages: 0x12 (18 dec)
//     Byte       3: Message ID (Incremental counter)
//     Byte       4: Payload Message Length (number of bytes, in our case: 14)
//     Byte       5: Payload Command Byte
//     Byte       6: Channel Number
//     Byte  7 - 14: Payload Data              if Byte 4 is '0E' = 14
//     Byte      15: Flag Type                 if Byte 4 is '0E' = 14
//     Byte 16 - 17: Device ID                 if Byte 4 is '0E' = 14
//     Byte      18: Device Type               if Byte 4 is '0E' = 14
//     Byte      19: Trans Type                if Byte 4 is '0E' = 14
//
//  Bridge messages
//
//     Byte  0 -  1: Packet Type Identifier
//                   (two byte header 'A', 'N' or '41', '4E' written in hex ASCII)
//     Byte       2: Packet Command Byte (Refer to table XXX). For Bridge messages: 0x43 (67 dec)
//     Byte       3: Message Sequence # Same Message Sequence # used to query ...
//     Byte       4: Message Length Number of Bytes in the Payload (Starting at Byte 7)
//     Byte  5 -  6: Reserved Contents t Defined
//     Byte       7: Major Version Bridge Firmware Major Version - Hex
//     Byte       8: Minor Version Bridge Firmware Minor Version - Hex
//     Byte       9: Product Type Bridge Product Version
//     Byte      10: Connection Stat 0 - TCP Available, 1 - TCP In Use
//     Byte      11: Reserved Reserved
//     Byte 12 - 13: Battery Level LSB, MSB Voltage in millivolts
//     Byte      14: Power Indicator 1 - Battery Charging
//     Byte      15: Operating Mode ...
//     Byte      16: Bridge Name Length
//     Byte 17 - 48: Bridge Name ASCII Name of Bridge
//     Byte 49 - 55: Bridge Mac Mac Address of Bridge in HEX
//-------------------------------------------------------------------------------------
int antProcessing::ant2txt
(
    void
)
{
    errorMessage.clear();

    if ( !deviceFileName.empty() )
    {
        std::ifstream inStr( deviceFileName.c_str() );
        if ( inStr.fail() )
        {
            appendErrorMessage( "Could not open device file \"" );
            appendErrorMessage( deviceFileName );
            appendErrorMessage( "\" for reading.\n" );
            errorCode = E_READ_FILE_NOT_OPEN;
        }
        else
        {
            readDeviceFileStream( inStr );
        }
    }

    if ( ( errorCode == 0 ) && ( !mcAddressOut.empty() ) && ( mcPortNoOut > 0 ) )
    {
        errorCode = multicastWrite.connect( mcAddressOut, mcPortNoOut, errorMessage );
        if ( ( errorCode != 0 ) && diagnostics )
        {
            std::cerr << std::endl;
            std::cerr << "Successfully created multicast connection for writing on IP address \"" << mcAddressOut << "\" and port number " << mcPortNoOut << "." << std::endl;
            std::cerr << std::endl;
        }
    }

    if ( errorCode == 0 )
    {
        if ( mcAddressIn.empty() )
        {
            // Read from stdin or from file
            if ( inputFileName.empty() )
            {
                if ( diagnostics )
                {
                    std::cerr << "READ data from STDIN." << std::endl;
                }
                errorCode = readAntFromStream( std::cin );
            }
            else
            {
                std::ifstream inputStream( inputFileName.c_str() );
                if ( inputStream.bad() )
                {
                   appendErrorMessage( "Could not open file \"" );
                   appendErrorMessage( inputFileName );
                   appendErrorMessage( "\" for reading.\n" );
                   errorCode = E_READ_FILE_NOT_OPEN;
                }
                else
                {
                    if ( diagnostics )
                    {
                        std::cerr << std::endl;
                        std::cerr << "Successfully opened input file \"" << inputFileName << "\" for reading." << std::endl;
                        std::cerr << std::endl;
                    }
                    errorCode = readAntFromStream( inputStream );
                    inputStream.close();
                }
            }
        }
        else
        {
            errorCode = readAntFromMultiCast();
        }

        multicastWrite.close();
    }
    if ( errorCode == E_END_OF_FILE )
    {
        errorCode = 0;
    }
    return errorCode;
}

void antProcessing::readDeviceFileStream
(
    std::ifstream &deviceFileStream
)
{
    char line[ C_BUFFER_SIZE ];
    while ( errorCode == 0 )
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
        readDeviceFileLine( lPtr );
        if ( ( errorCode == E_BAD_PARAMETER_VALUE ) && !getExitOnWarnings() )
        {
            std::cerr << errorMessage << std::endl;
            clearErrors();
        }
    }
}

void antProcessing::help
(
    void
)
{
    std::stringstream outputMessage;
    amString          indent( "\n    " );
    amString          indent2( indent );
    amString          option( "H:" );
    bool              argumentForH = validOptions.contains( option );

    indent2 += "    ";

    outputMessage << "\n";
    outputMessage << "\n";
    outputMessage << "The program \"";
    outputMessage << programName;
    outputMessage << "\"";

    outputMessage << indent;
    outputMessage << "reads binary ANT+ data from multicast (default - see options '-1', '-f' and '-M'),";

    outputMessage << indent;
    outputMessage << "transforms the binary data according to the ANT+ specifications (also see option '-S'),";

    outputMessage << indent;
    outputMessage << "and outputs the data to stdout (default) or a network connection (see options '-2' and '-m').";
    outputMessage << "\n";
    outputMessage << "\n";

    outputMessage << programName;
    outputMessage << " takes the following arguments:";
    outputMessage << "\n";

    option = "1";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Read from stdin.";
        outputMessage << "\n";
    }

    option = "2";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Write to stdout.";
        outputMessage << indent2;
        outputMessage << programName;
        outputMessage << " writes to stdout by default.";
        outputMessage << indent2;
        outputMessage << "Note: This option only applies with option '-m' to create output to both multicast and stdout.";
        outputMessage << "\n";
    }

    option = "B";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output Bridge Status messages. By default, Bridge Status messages are not output.";
        outputMessage << "\n";
    }

    option = "d";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << " <device_file>: Read parameters for ANT+ devices which need them (AERO, all speed, PWRB20) from the file \"<device_file>\".";
        outputMessage << indent2;
        outputMessage << "Notes: 1. If no device file is specified the default paramters for the ANT+ devices will be used.";
        outputMessage << indent2;
        outputMessage << "          See option '-H";
        if ( argumentForH )
        {
            outputMessage << " <device_type>";
        }
        outputMessage << "' for the values.";
        outputMessage << indent2;
        outputMessage << "       2. This option is obsolete when option '-S' (semi-cooked output) is used.";
        outputMessage << "\n";
    }

    option = "D";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output diagnostics information.";
        outputMessage << "\n";
    }

    option = "f";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": <file_name>: Read ANT+ input data from file \"<file_name>\".";
        outputMessage << "\n";
    }

    option = "H";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        if ( argumentForH )
        {
            outputMessage << " <device_type>: Show the output format and parameter definition for device type \"<device_type>\".";
            outputMessage << indent2;
            outputMessage << "To list all supported device types use option '-H ?'.";
        }
        else
        {
            outputMessage << ": Show the output format and parameter definition for this device type.";
        }
        outputMessage << "\n";
    }

    option = "h";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Display this message.";
        outputMessage << "\n";
    }

    option = "I";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Create a multicast connection on interface \"<interface>\".";
        outputMessage << indent2;
        outputMessage << "Note: If '-I auto' is specified the program will loop through all interfaces and";
        outputMessage << indent2;
        outputMessage << "      use the FIRST interface on which a connection could be created.";
        outputMessage << indent2;
        outputMessage << "Default: <interface> = \"";
        outputMessage << C_DEFAULT_INTERFACE;
        outputMessage << "\".";
        outputMessage << "\n";
    }

    option = "J";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output result as JSON object.";
        outputMessage << "\n";
    }

    option = "m";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << " <mc_address>: Publish using Multicast on IP address <mc_address>.";
        outputMessage << indent2;
        outputMessage << "Note: The '-m' option must be followed by a '-p <port_number>' option.";
        outputMessage << "\n";
    }

    option = "M";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << " <mc_address>: Read from Multicast IP address <mc_address>.";
        outputMessage << indent2;
        outputMessage << "Notes: 1. The '-M' option may be followed by a '-p <port_number>' option.";
        outputMessage << indent2;
        outputMessage << "          If no port number is specified the default port number (";
        outputMessage << C_PORT_NO_BRIDGE_MULTICAST;
        outputMessage << ") is used.";
        outputMessage << indent2;
        outputMessage << "       2. The interface on which to connect can be specified with the '-I <interface>' option.";
        outputMessage << indent2;
        outputMessage << "          If no interface is specified the default (\"";
        outputMessage << C_DEFAULT_INTERFACE;
        outputMessage << "\") is used.";
        outputMessage << indent2;
        outputMessage << "Default: <mc_address> = \"";
        outputMessage << C_DEFAULT_MC_ADDRESS_IN;
        outputMessage << "\".";
        outputMessage << "\n";
    }

    option = "p";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << " <port_number>: Set the port number from which to read (if preceeded by '-M') or to which to write (if preceeded by '-m') to <port_number>.";
        outputMessage << indent2;
        outputMessage << "Default: For option '-M', <port_number> = ";
        outputMessage << C_DEFAULT_MC_PORT_NO_IN;
        outputMessage << ".";
        outputMessage << indent2;
        outputMessage << "         For option '-m', no default port number exists.";
        outputMessage << "\n";
    }

    option = "P";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << " <precision>: Set the number of decimal places for floating point output of values other than time (for time see '-T').";
        outputMessage << indent2;
        outputMessage << "Default: <precision> = ";
        outputMessage << C_DEFAULT_VALUE_PRECISION_DEFAULT;
        outputMessage << ".\n";
    }

    option = "r";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output raw data, i.e. data as bytes in text form.";
        outputMessage << indent2;
        outputMessage << "Note: This option does not apply when input is semi-cooked.";
        outputMessage << "\n";
    }

    option = "R";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Only output results for registered devices (i.e. devices listed in the deviceIDs file, see option '-d').";
        outputMessage << "\n";
    }

    option = "s";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Input data are 'semi-cooked'.";
        outputMessage << indent2;
        outputMessage << "Note: This option only applies when options '-1' (read from stdin) or '-f' (read from file) are present.";
        outputMessage << "\n";
    }

    option = C_SEMI_COOKED_SYMBOL_AS_STRING;
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output data in 'semi-cooked' mode.";
        outputMessage << indent2;
        outputMessage << "Default: \"fully-cooked\" mode.";
        outputMessage << "\n";
    }

    option = "t";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << " <time_out>: Time out after <time_out> seconds of no data being received (<time_out> = 0 means no timeout).";
        outputMessage << indent2;
        outputMessage << "Default: <time_out> = ";
        outputMessage << C_TIME_OUT_SEC_DEFAULT;
        outputMessage << "\n";
    }

    option = "T";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << " <precision>: Set the number of decimal places for floating point output of time values (for other values , see '-P').";
        outputMessage << indent2;
        outputMessage << "Default: <precision> = ";
        outputMessage << C_DEFAULT_TIME_PRECISION_DEFAULT;
        outputMessage << ".\n";
    }

    option = "U";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output unknown ANT+ messages as 'TYPE' messages. By default, unknown ANT+ messages are not output.";
        outputMessage << "\n";
    }

    option = "v";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output version (short form).";
        outputMessage << "\n";
    }

    option = "V";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Output version (long form).";
        outputMessage << "\n";
    }

    option = "w";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Exit the program on warnings.";
        outputMessage << "\n";
    }

    option = "x";
    if ( validOptions.contains( option ) )
    {
        outputMessage << indent;
        outputMessage << "-";
        outputMessage << option;
        outputMessage << ": Test mode. In order for 'diff' to work:";
        outputMessage << indent2;
        outputMessage << "Use fixed time stamps.";
        outputMessage << indent2;
        outputMessage << "Use fixed number of output decimals.";
        outputMessage << "\n";
    }

    std::cout << outputMessage.str() << std::endl;
}

void antProcessing::outputOptionPOWER
(
    std::stringstream &outputMessage,
    const amString    &indent
)
{
    outputMessage << indent;
    outputMessage << "PWRB01    Power Meter Calibration Request.";
    outputMessage << indent;
    outputMessage << "PWRB02    Get/Set Power Meter Parameters.";
    outputMessage << indent;
    outputMessage << "PWRB03    Power Meter Measurement Output.";
    outputMessage << indent;
    outputMessage << "PWRB10    Standard Power-Only Sensor.";
    outputMessage << indent;
    outputMessage << "PWRB11    Wheel-Torque Power Meter.";
    outputMessage << indent;
    outputMessage << "PWRB12    Crank-Torque Power Meter.";
    outputMessage << indent;
    outputMessage << "PWRB13    Power Meter Torque Effectiveness & Pedal Smoothness.";
    outputMessage << indent;
    outputMessage << "PWRB20    Crank-Torque-Frequency Power Meter.";
    outputMessage << indent;
    outputMessage << "PWRB46    Power Meter Request Data Page.";
    outputMessage << indent;
    outputMessage << "PWRB50    Power Meter Manufacturer Information.";
    outputMessage << indent;
    outputMessage << "PWRB51    Power Meter Product Identification.";
    outputMessage << indent;
    outputMessage << "PWRB52    Power Meter Battery Information.";
}

void antProcessing::outputOptionF
(
    std::stringstream &outputMessage,
    const amString    &indent
)
{
    outputMessage << "Currently supported device types and messages are";
    outputMessage << indent;
    outputMessage << "AERO      Alphamantis Aero Sensor.";
    outputMessage << indent;
    outputMessage << "AUDIO     Audio Controls.";
    outputMessage << indent;
    outputMessage << "BDG       Bridge Status.";
    outputMessage << indent;
    outputMessage << "BLDPR     Blood Pressure.";
    outputMessage << indent;
    outputMessage << "CAD7A     Cadence Only Sensor.";
    outputMessage << indent;
    outputMessage << "ENV       Environment Sensor.";
    outputMessage << indent;
    outputMessage << "HRM       Heart Rate Monitor.";
    outputMessage << indent;
    outputMessage << "MSSDM     Multi-Sport Speed and Distance Monitor.";
    outputOptionPOWER( outputMessage, indent );
    outputMessage << indent;
    outputMessage << "SBSDM     Stride-Based Speed and Distance Monitor.";
    outputMessage << indent;
    outputMessage << "SPB7      Speed Only Sensor.";
    outputMessage << indent;
    outputMessage << "SPCAD790  Speed and Cadence Sensor.";
    outputMessage << indent;
    outputMessage << "WEIGHT    Weight Scale.";
    outputMessage << std::endl;
}

void antProcessing::outputFormats
(
    const amString &deviceType
)
{
    std::stringstream outputMessage;
    amString          deviceTypeUC = deviceType;
    amString          auxBuffer;
    amString          auxBuffer1;
    amString          speedBuffer;
    amString          separator( "   " );
    amString          indent( "\n    " );
    amString          indent2( indent );
    indent2 += "    ";
    amString          indent3( indent2 );
    indent3 += "    ";
    deviceTypeUC.toUpper();

    speedBuffer  = separator;
    speedBuffer += "<speed>";
    speedBuffer += separator;
    speedBuffer += "<wheel_circumference>";
    speedBuffer += separator;
    speedBuffer += "<gear_ratio>";

    if ( ( deviceTypeUC == "ALL"         ) ||
         ( deviceTypeUC == "AERO"        ) ||
         ( deviceTypeUC == "AUDIO"       ) ||
         ( deviceTypeUC == "BDG"         ) ||
         ( deviceTypeUC == "BRIDGE"      ) ||
         ( deviceTypeUC == "BLDPR"       ) ||
         ( deviceTypeUC == "BLOOD"       ) ||
         ( deviceTypeUC == "CAD"         ) ||
         ( deviceTypeUC == "CAD7A"       ) ||
         ( deviceTypeUC == "CADENCE"     ) ||
         ( deviceTypeUC == "ENV"         ) ||
         ( deviceTypeUC == "ENVIRONMENT" ) ||
         ( deviceTypeUC == "HRM"         ) ||
         ( deviceTypeUC == "HEART"       ) ||
         ( deviceTypeUC == "HEARTRATE"   ) ||
         ( deviceTypeUC == "MSSDM"       ) ||
         ( deviceTypeUC == "MULTI"       ) ||
         ( deviceTypeUC == "MULTI-SPORT" ) ||
         ( deviceTypeUC == "POWER"       ) ||
         ( deviceTypeUC == "PWRB01"      ) ||
         ( deviceTypeUC == "PWRB02"      ) ||
         ( deviceTypeUC == "PWRB03"      ) ||
         ( deviceTypeUC == "PWRB10"      ) ||
         ( deviceTypeUC == "PWRB11"      ) ||
         ( deviceTypeUC == "PWRB12"      ) ||
         ( deviceTypeUC == "PWRB13"      ) ||
         ( deviceTypeUC == "PWRB20"      ) ||
         ( deviceTypeUC == "PWRB46"      ) ||
         ( deviceTypeUC == "PWRB50"      ) ||
         ( deviceTypeUC == "PWRB51"      ) ||
         ( deviceTypeUC == "PWRB52"      ) ||
         ( deviceTypeUC == "SPEED"       ) ||
         ( deviceTypeUC == "SBSDM"       ) ||
         ( deviceTypeUC == "STRIDE"      ) ||
         ( deviceTypeUC == "SPB7"        ) ||
         ( deviceTypeUC == "SPCAD"       ) ||
         ( deviceTypeUC == "SPCAD790"    ) ||
         ( deviceTypeUC == "WEIGHT"      )
       )
    {
        outputMessage << std::endl;
        outputMessage << "Output format for sensor" << std::endl;
        outputMessage << std::endl;
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "AERO" ) )
        {
            outputMessage << "AERO (Wind Speed and Yaw Angle)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "AERO_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "<air_speed>";
            outputMessage << separator;
            outputMessage << "<yaw_angle>";
            outputMessage << separator;
            outputMessage << "<calibration_air_density>";
            outputMessage << separator;
            outputMessage << "<air_speed_multiplier>";
            outputMessage << separator;
            outputMessage << "<cur_air_density>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent2;
            outputMessage << "The AERO sensor takes 3 parameters";
            outputMessage << indent3;
            outputMessage << "current air density            (default: ";
            outputMessage << C_RHO_DEFAULT;
            outputMessage << " kg/m^3),";
            outputMessage << indent3;
            outputMessage << "air density during calibration (default: ";
            outputMessage << C_CALIBRATION_RHO_DEFAULT;
            outputMessage << " kg/m^3),";
            outputMessage << indent3;
            outputMessage << "air speed muliplier            (default: ";
            outputMessage << C_AIR_SPEED_MULTIPLIER_DEFAULT;
            outputMessage << ").";
            outputMessage << indent2;
            outputMessage << "The parameters are defined in the deviceIDs file (see option '-d') as:";
            outputMessage << indent3;
            outputMessage << "AERO";
            outputMessage << separator;
            outputMessage << "AERO_<id>";
            outputMessage << separator;
            outputMessage << "<calibration_air_density>";
            outputMessage << separator;
            outputMessage << "<air_speed_multiplier>";
            outputMessage << indent3;
            outputMessage << "RHO";
            outputMessage << separator;
            outputMessage << "<current_air_density>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "AERO_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "<air_speed_raw>";
            outputMessage << separator;
            outputMessage << "<yaw_angle_raw>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "AUDIO" ) )
        {
            outputMessage << "AUDIO (ANT+ Audio Controls)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "AUDIO_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "VOLUME";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "TOTAL_TRACK_TIME";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "CURR_TRACK_TIME";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "STATE";
            outputMessage << separator;
            outputMessage << "<state>";
            outputMessage << separator;
            outputMessage << "REPEAT";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "SHUFFLE";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;
            outputMessage << "or";
            outputMessage << indent2;
            outputMessage << "AUDIO_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "16";
            outputMessage << separator;
            outputMessage << "SERIAL_NO";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "COMMAND";
            outputMessage << separator;
            outputMessage << "<command>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "AUDIO_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "<volume>";
            outputMessage << separator;
            outputMessage << "<total_track_time>";
            outputMessage << separator;
            outputMessage << "<curr_track_time>";
            outputMessage << separator;
            outputMessage << "<status>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;
            outputMessage << "or";
            outputMessage << indent2;
            outputMessage << "AUDIO_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "16";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<command_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "BLDPR" ) || ( deviceTypeUC == "BLOOD" ) )
        {
            outputMessage << "BLDPR (ANT+ Blood Pressure) - not yet implemented";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "BLDPR_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "<data_page>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "BLDPR_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<data_page>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "BDG" ) || ( deviceTypeUC == "BRIDGE" ) )
        {
            outputMessage << "BDG (Bridge Status)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "BDG_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "<bridge_name>";
            outputMessage << separator;
            outputMessage << "<mac_address>";
            outputMessage << separator;
            outputMessage << "<firmware_version>";
            outputMessage << separator;
            outputMessage << "<voltage>";
            outputMessage << separator;
            outputMessage << "<power_indicator>";
            outputMessage << separator;
            outputMessage << "<operating_mode>";
            outputMessage << separator;
            outputMessage << "<connection_status>";
            outputMessage << separator;
            outputMessage << "<sw_version_ant2txt>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "BDG_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<bridge_name>";
            outputMessage << separator;
            outputMessage << "<mac_address>";
            outputMessage << separator;
            outputMessage << "<firmware_version>";
            outputMessage << separator;
            outputMessage << "<voltage>";
            outputMessage << separator;
            outputMessage << "<power_indicator>";
            outputMessage << separator;
            outputMessage << "<operating_mode>";
            outputMessage << separator;
            outputMessage << "<connection_status>";
            outputMessage << separator;
            outputMessage << "<sw_version_ant2txt>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "CAD7A" ) ||
             ( deviceTypeUC == "CAD" ) || ( deviceTypeUC == "CADENCE" ) )
        {
            auxBuffer  = indent2;
            auxBuffer += "CAD7A_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "<cadence>";
            auxBuffer += separator;

            outputMessage << "CAD7A (Cadence Only)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            outputMessage << auxBuffer;
            outputMessage << "0/128";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "1/129";
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2/130";
            outputMessage << separator;
            outputMessage << "<manufacturer_id>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "<model_number>";
            outputMessage << separator;
            outputMessage << "<software_version>";
            outputMessage << separator;
            outputMessage << "<hardware_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent2;
            outputMessage << "Any cadence sensor can become a makeshift speed sensor.";
            outputMessage << std::endl;

            outputMessage << auxBuffer;
            outputMessage << "0/128";
            outputMessage << speedBuffer;
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "1/129";
            outputMessage << speedBuffer;
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2/130";
            outputMessage << speedBuffer;
            outputMessage << separator;
            outputMessage << "<manufacturer_id>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3/131";
            outputMessage << speedBuffer;
            outputMessage << "<model_number>";
            outputMessage << separator;
            outputMessage << "<software_version>";
            outputMessage << separator;
            outputMessage << "<hardware_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent2;
            outputMessage << "To turn a cadence sensor into a speed sensor it must be defined as a such in the deviceIDs file by";
            outputMessage << indent3;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "CAD7A_<id>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<gear_ratio>";
            outputMessage << indent2;
            outputMessage << "and loaded with option '-d'. The values for <wheel_circumference> and <gear_ratio> are optional, the defaults are:";
            outputMessage << indent3;
            outputMessage << "<wheel_circumference> = ";
            outputMessage << C_WHEEL_CIRCUMFERENCE_DEFAULT;
            outputMessage << " (m)";
            outputMessage << indent3;
            outputMessage << "<gear_ratio>          = ";
            outputMessage << C_GEAR_RATIO_DEFAULT;
            outputMessage << " (";
            outputMessage << C_GEAR_RATIO_DEFAULT_AS_RATIO;
            outputMessage << ")";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "CAD7A_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_time>";
            outputMessage << separator;
            outputMessage << "<delta_revolution_count>";
            outputMessage << separator;
            outputMessage << "0/128";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;
            outputMessage << "CAD7A_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_time>";
            outputMessage << separator;
            outputMessage << "<delta_revolution_count>";
            outputMessage << separator;
            outputMessage << "1/129";
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;
            outputMessage << "CAD7A_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_time>";
            outputMessage << separator;
            outputMessage << "<delta_revolution_count>";
            outputMessage << separator;
            outputMessage << "2/130";
            outputMessage << separator;
            outputMessage << "<manufacturer_id>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;
            outputMessage << "CAD7A_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_time>";
            outputMessage << separator;
            outputMessage << "<delta_revolution_count>";
            outputMessage << separator;
            outputMessage << "3/131";
            outputMessage << separator;
            outputMessage << "<model_number>";
            outputMessage << separator;
            outputMessage << "<software_version>";
            outputMessage << separator;
            outputMessage << "<hardware_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "ENV" ) || ( deviceTypeUC == "ENVIRONMENT") )
        {
            outputMessage << "ENV (ANT+ Environment Sensor)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "ENV_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "0";
            outputMessage << separator;
            outputMessage << "LOCAL_TIME";
            outputMessage << separator;
            outputMessage << "<local_time_set_value>";
            outputMessage << separator;
            outputMessage << "UTC_TIME";
            outputMessage << separator;
            outputMessage << "<utc_time_set_value>";
            outputMessage << separator;
            outputMessage << "TRANSMISSION_HZ";
            outputMessage << separator;
            outputMessage << "<transmission_rate_value>";
            outputMessage << separator;
            outputMessage << "SUPPORTED_PAGES";
            outputMessage << separator;
            outputMessage << "<supported_page_nos>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;
            outputMessage << "ENV_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "CURR_TEMP";
            outputMessage << separator;
            outputMessage << "<current_temperature>";
            outputMessage << separator;
            outputMessage << "LOW_TEMP_24H";
            outputMessage << separator;
            outputMessage << "<low_temperature_24h>";
            outputMessage << separator;
            outputMessage << "HIGH_TEMP_24H";
            outputMessage << separator;
            outputMessage << "<high_temperature_24h>";
            outputMessage << separator;
            outputMessage << "EVENT_NO";
            outputMessage << separator;
            outputMessage << "<event_no>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "ENV_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "0";
            outputMessage << separator;
            outputMessage << "<local_time>";
            outputMessage << separator;
            outputMessage << "<utc_time>";
            outputMessage << separator;
            outputMessage << "<current_track_time>";
            outputMessage << separator;
            outputMessage << "<status>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;
            outputMessage << "ENV_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "<current_temperature_100>";
            outputMessage << separator;
            outputMessage << "<low_temperature_24h_10>";
            outputMessage << separator;
            outputMessage << "<high_temperature_24h_10>";
            outputMessage << separator;
            outputMessage << "<delta_event_count>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "HEART"    ) ||
             ( deviceTypeUC == "HRM" ) || ( deviceTypeUC == "HEARTRATE") )
        {
            outputMessage << "HRM (Heart Rate)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "HRM_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "<heart_rate>";
            auxBuffer += separator;
            auxBuffer += "<total_hb_count>";
            auxBuffer += separator;
            auxBuffer += "<total_hb_event_time>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "0/128";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "1/129";
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2/130";
            outputMessage << separator;
            outputMessage << "<manufactured_id>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3/131";
            outputMessage << separator;
            outputMessage << "<model_number>";
            outputMessage << separator;
            outputMessage << "<sw_version_sensor>";
            outputMessage << separator;
            outputMessage << "<hw_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "4/132";
            outputMessage << separator;
            outputMessage << "<previous_heartbeat_event_time>";
            outputMessage << separator;
            outputMessage << "<manufacturer_specific_data>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "HRM_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += C_SEMI_COOKED_SYMBOL_AS_STRING;
            auxBuffer += separator;
            auxBuffer += "<heart_rate>";
            auxBuffer += separator;
            auxBuffer += "<total_hb_count>";
            auxBuffer += separator;
            auxBuffer += "<total_hb_event_time>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "0/128";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "1/129";
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2/130";
            outputMessage << separator;
            outputMessage << "<manufacturer_id>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3/131";
            outputMessage << separator;
            outputMessage << "<model_no>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << separator;
            outputMessage << "<hw_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "4/132";
            outputMessage << separator;
            outputMessage << "<previous_hb_time>";
            outputMessage << separator;
            outputMessage << "<manufacturer_specific_data>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "MSSDM" ) || ( deviceTypeUC == "MULTI") || ( deviceTypeUC == "MULTI-SPORT") )
        {
            outputMessage << "MSSDM (ANT+ Multi-Sport Speed and Distance Monitor)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "SPB7_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "T";
            outputMessage << separator;
            outputMessage << "<event_time>";
            outputMessage << separator;
            outputMessage << "D";
            outputMessage << separator;
            outputMessage << "<event_distance>";
            outputMessage << separator;
            outputMessage << "V";
            outputMessage << separator;
            outputMessage << "<event_speed>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2";
            outputMessage << separator;
            outputMessage << "LAT";
            outputMessage << separator;
            outputMessage << "<latitude>";
            outputMessage << separator;
            outputMessage << "LON";
            outputMessage << separator;
            outputMessage << "<longitude>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3";
            outputMessage << separator;
            outputMessage << "FIX_TYPE";
            outputMessage << separator;
            outputMessage << "<fix_type>";
            outputMessage << separator;
            outputMessage << "HEADING";
            outputMessage << separator;
            outputMessage << "<heading>";
            outputMessage << separator;
            outputMessage << "ELEVATION";
            outputMessage << separator;
            outputMessage << "<elevation>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            auxBuffer += "48";
            auxBuffer += separator;
            auxBuffer += "MODE";
            auxBuffer += separator;
            auxBuffer += "<mode>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "SCALE_FACTOR";
            outputMessage << separator;
            outputMessage << "<scale_factor>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "CALIBRATION_REQUEST";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "MSSDM_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += C_SEMI_COOKED_SYMBOL_AS_STRING;
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "<delta_event_time>";
            outputMessage << separator;
            outputMessage << "<delta_event_distance>";
            outputMessage << separator;
            outputMessage << "<event_speed>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2";
            outputMessage << separator;
            outputMessage << "<latitude>";
            outputMessage << separator;
            outputMessage << "<longitude>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3";
            outputMessage << separator;
            outputMessage << "<fix_type>";
            outputMessage << separator;
            outputMessage << "<heading>";
            outputMessage << separator;
            outputMessage << "<elevation>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "48";
            outputMessage << separator;
            outputMessage << "<mode>";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB01" ) )
        {
            outputMessage << "PWRB01: (Power Meter Calibration)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB01_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "REQUEST";
            outputMessage << separator;
            outputMessage << "ZERO_OFFSET";
            outputMessage << separator;
            outputMessage << "<zero_offset_value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "REQUEST";
            outputMessage << separator;
            outputMessage << "SLOPE";
            outputMessage << separator;
            outputMessage << "<slope_value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "REQUEST";
            outputMessage << separator;
            outputMessage << "SERIAL_NO";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "RESPONSE";
            outputMessage << separator;
            outputMessage << "ZERO_OFFSET";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "RESPONSE";
            outputMessage << separator;
            outputMessage << "SLOPE";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "RESPONSE";
            outputMessage << separator;
            outputMessage << "SERIAL_NO";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "AUTO_ZERO_ENABLE";
            outputMessage << separator;
            outputMessage << "<support_status>";
            outputMessage << separator;
            outputMessage << "AUTO_ZERO_STATUS";
            outputMessage << separator;
            outputMessage << "<on_off_status>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "MANUAL_ZERO_REQUEST";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "AUTO_ZERO_REQUEST";
            outputMessage << separator;
            outputMessage << "AUTO_ZERO";
            outputMessage << separator;
            outputMessage << "ON/OFF/NOT_SUPPORTED";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "MANUAL_ZERO_SUCCESS";
            outputMessage << separator;
            outputMessage << "AUTO_ZERO";
            outputMessage << separator;
            outputMessage << "ON/OFF/NOT_SUPPORTED";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "MANUAL_ZERO_FAILED";
            outputMessage << separator;
            outputMessage << "AUTO_ZERO";
            outputMessage << separator;
            outputMessage << "ON/OFF/NOT_SUPPORTED";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "CUSTOM_CALIBRATION_PARAMS_REQUEST";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "CUSTOM_CALIBRATION_PARAMS_RESPONSE";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "CUSTOM_CALIBRATION_PARAMS_UPDATE";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "CUSTOM_CALIBRATION_PARAMS_UPDATE_RESPONSE";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";

            auxBuffer += C_SEMI_COOKED_SYMBOL_AS_STRING;
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "16";
            outputMessage << separator;
            outputMessage << "<ctf_defined_id>";
            outputMessage << separator;
            outputMessage << "<message_value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "18";
            outputMessage << separator;
            outputMessage << "<auto_zero_enable>";
            outputMessage << separator;
            outputMessage << "<auto_zero_status>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "170";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "171";
            outputMessage << separator;
            outputMessage << "<auto_zero_status>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "172";
            outputMessage << separator;
            outputMessage << "<auto_zero_status>";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "175";
            outputMessage << separator;
            outputMessage << "<auto_zero_status>";
            outputMessage << separator;
            outputMessage << "<value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "186";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "187";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "188";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "189";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB02" ) )
        {
            outputMessage << "PWRB02 (Get/Set Parameters)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB02_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "CRANK_LENGTH";
            outputMessage << separator;
            outputMessage << "<cl_value>";
            outputMessage << separator;
            outputMessage << "CRANK_LENGTH_USED";
            outputMessage << separator;
            outputMessage << "<clu_value>";
            outputMessage << separator;
            outputMessage << "SW_MISMATCH";
            outputMessage << separator;
            outputMessage << "<swm_value>";
            outputMessage << separator;
            outputMessage << "SENSOR_AVAILABILTY";
            outputMessage << separator;
            outputMessage << "<availability>";
            outputMessage << separator;
            outputMessage << "CUSTOM_CALIBRATION";
            outputMessage << separator;
            outputMessage << "REQUIRED/NOT_REQUIRED";
            outputMessage << separator;
            outputMessage << "DETERMINE_CRANK_LENGTH";
            outputMessage << separator;
            outputMessage << "AUTOMATICALLY/MANUALLY";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB02_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<sub_page_number>";
            outputMessage << separator;
            outputMessage << "<crank_length>";
            outputMessage << separator;
            outputMessage << "<status>";
            outputMessage << separator;
            outputMessage << "<auto_crank_length>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB03" ) )
        {
            outputMessage << "PWRB03 (Measurement Output)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB03_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "NB_DATA_TYPES";
            outputMessage << separator;
            outputMessage << "<number_of_data_types>";
            outputMessage << separator;
            outputMessage << "TIME_VALUE";
            outputMessage << separator;
            outputMessage << "<time_value>";
            outputMessage << separator;
            outputMessage << "<data_type>";
            outputMessage << separator;
            outputMessage << "<data_value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB03_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<number_of_data_types>";
            outputMessage << separator;
            outputMessage << "<data_type>";
            outputMessage << separator;
            outputMessage << "<scale_factor>";
            outputMessage << separator;
            outputMessage << "<delta_time_stamp>";
            outputMessage << separator;
            outputMessage << "<measurement_value>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB10" ) )
        {
            outputMessage << "PWRB10 (Power-Only) Power Meter";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB10_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "<power>";
            auxBuffer += separator;
            auxBuffer += "<cadence>";
            auxBuffer += separator;
            auxBuffer += "<power_pedal_contribution>";
            auxBuffer += separator;
            auxBuffer += "R_PEDAL/PEDAL_UNKNOWN";

            outputMessage << auxBuffer;
            outputMessage << separator; 
            outputMessage << "<sw_version>";

            outputMessage << indent3;
            outputMessage << "or";

            outputMessage << auxBuffer;
            outputMessage << speedBuffer;
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent2;
            outputMessage << "Any power-only power meter is also a cadence sensor and can therefore become a makeshift speed sensor.";
            outputMessage << indent2;
            outputMessage << "To turn a power-only power meter into a speed sensor it must be defined as a such in the deviceIDs file by";
            outputMessage << indent3;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "PWRB10_<id>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<gear_ratio>";
            outputMessage << indent2;
            outputMessage << "and loaded with option '-d'. The values for <wheel_circumference> and <gear_ratio> are optional, the defaults are:";
            outputMessage << indent3;
            outputMessage << "<wheel_circumference> = ";
            outputMessage << C_WHEEL_CIRCUMFERENCE_DEFAULT;
            outputMessage << " (m)";
            outputMessage << indent3;
            outputMessage << "<gear_ratio>          = ";
            outputMessage << C_GEAR_RATIO_DEFAULT;
            outputMessage << " (";
            outputMessage << C_GEAR_RATIO_DEFAULT_AS_RATIO;
            outputMessage << ")";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB10_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_count>";
            outputMessage << separator;
            outputMessage << "<delta_accumulated_power>";
            outputMessage << separator;
            outputMessage << "<pm_sw_version>";
            outputMessage << separator;
            outputMessage << "<instantaneous_cadence>";
            outputMessage << separator;
            outputMessage << "<pedal_power>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB11" ) )
        {
            outputMessage << "PWRB11 (Wheel-Torque Power Meter)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB11_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "<power>";
            auxBuffer += separator;
            auxBuffer += "<cadence>";
            auxBuffer += separator;
            auxBuffer += "<torque>";
            auxBuffer += separator;
            auxBuffer += "<wheel_ticks>";
            auxBuffer += separator;

            outputMessage << auxBuffer; 
            outputMessage << "<sw_version>";

            outputMessage << indent3;
            outputMessage << "or";

            outputMessage << auxBuffer; 
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent2;
            outputMessage << "Any wheel-torque power meter can also be used as a speed sensor.";
            outputMessage << indent2;
            outputMessage << "To turn a wheel-torque power meter into a speed sensor it must be defined as a such in the deviceIDs file by";
            outputMessage << indent3;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "PWRB11_<id>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << indent2;
            outputMessage << "and loaded with option '-d'. The value for <wheel_circumference> is optional, the defaults is:";
            outputMessage << indent3;
            outputMessage << "<wheel_circumference> = ";
            outputMessage << C_WHEEL_CIRCUMFERENCE_DEFAULT;
            outputMessage << " (m)";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB11_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_count>";
            outputMessage << separator;
            outputMessage << "<delta_wheel_period>";
            outputMessage << separator;
            outputMessage << "<delta_accumulated_torque>";
            outputMessage << separator;
            outputMessage << "<instantaneous_cadence>";
            outputMessage << separator;
            outputMessage << "<wheel_ticks>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB12" ) )
        {
            outputMessage << "PWRB12 (Crank-Torque (CT) Power Meter)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB12_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "<power>";
            auxBuffer += separator;
            auxBuffer += "<cadence>";
            auxBuffer += separator;
            auxBuffer += "<torque>";
            auxBuffer += separator;
            auxBuffer += "<crank_ticks>";

            outputMessage << auxBuffer;
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << indent3;
            outputMessage << "or";

            outputMessage << auxBuffer;
            outputMessage << speedBuffer;
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << indent2;

            outputMessage << indent2;
            outputMessage << "Any CT power meter is also a cadence sensor and can therefore become a makeshift speed sensor.";
            outputMessage << indent2;
            outputMessage << "To turn a CT power meter into a speed sensor it must be defined as a such in the deviceIDs file by";
            outputMessage << indent3;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "PWRB12_<id>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<gear_ratio>";
            outputMessage << indent2;
            outputMessage << "and loaded with option '-d'. The values for <wheel_circumference> and <gear_ratio> are optional, the defaults are:";
            outputMessage << indent3;
            outputMessage << "<wheel_circumference> = ";
            outputMessage << C_WHEEL_CIRCUMFERENCE_DEFAULT;
            outputMessage << " (m)";
            outputMessage << indent3;
            outputMessage << "<gear_ratio>          = ";
            outputMessage << C_GEAR_RATIO_DEFAULT;
            outputMessage << " (";
            outputMessage << C_GEAR_RATIO_DEFAULT_AS_RATIO;
            outputMessage << ")";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB12_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_count>";
            outputMessage << separator;
            outputMessage << "<delta_crank_period>";
            outputMessage << separator;
            outputMessage << "<delta_accumulated_torque>";
            outputMessage << separator;
            outputMessage << "<instantaneous_cadence>";
            outputMessage << separator;
            outputMessage << "<crank_ticks>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB13" ) )
        {
            outputMessage << "PWRB13 (Torque Effiectiveness and Pedal Smoothness)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB13_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "L_TRQ_EFF";
            auxBuffer += separator;
            auxBuffer += "<lte_value>";
            auxBuffer += separator;
            auxBuffer += "R_TRQ_EFF";
            auxBuffer += separator;
            auxBuffer += "<rte_value>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "C_PDL_SMOOTH";
            outputMessage << separator;
            outputMessage << "<common_pedal_smoothness>";
            outputMessage << separator;
            outputMessage << "EVENT_NO";
            outputMessage << separator;
            outputMessage << "<event_no>";
            outputMessage << separator;
            outputMessage << " <sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "L_PDL_SMOOTH";
            outputMessage << separator;
            outputMessage << "<left_pedal_smoothness>";
            outputMessage << separator;
            outputMessage << "R_PDL_SMOOTH";
            outputMessage << separator;
            outputMessage << "<right_pedal_smoothness>";
            outputMessage << separator;
            outputMessage << "EVENT_NO";
            outputMessage << separator;
            outputMessage << "<event_no>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;

            outputMessage << "PWRB13_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<raw_left_torque_effectiveness>";
            outputMessage << separator;
            outputMessage << "<raw_right_torque_effectiveness>";
            outputMessage << separator;
            outputMessage << "<raw_left_pedal_smoothness>";
            outputMessage << separator;
            outputMessage << "<raw_right_pedal_smoothness>";
            outputMessage << separator;
            outputMessage << "<delta_event_count>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB20" ) )
        {
            outputMessage << "PWRB20 (Crank-Torque-Frequency (CTF) Power Meter)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB20_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "<power>";
            auxBuffer += separator;
            auxBuffer += "<cadence>";
            auxBuffer += separator;
            auxBuffer += "<torque>";
            auxBuffer += separator;
            auxBuffer += "<offset>";
            auxBuffer += separator;
            auxBuffer += "<slope>";
            auxBuffer += separator;
            auxBuffer += "<factory_slope>";
            auxBuffer += separator;
            auxBuffer += "<slope_used>";

            outputMessage << auxBuffer;
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << indent3;
            outputMessage << "or";

            outputMessage << auxBuffer;
            outputMessage << speedBuffer;
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent2;
            outputMessage << "The CTF power meter takes 2 parameters";
            outputMessage << indent3;
            outputMessage << "power meter offset (default: ";
            outputMessage << C_OFFSET_DEFAULT;
            outputMessage << " Hz),";
            outputMessage << indent3;
            outputMessage << "power meter slope  (default: ";
            outputMessage << ( int ) C_SLOPE_DEFAULT;
            outputMessage << " [use factory slope], unit: Nm/Hz). ";
            outputMessage << indent2;
            outputMessage << "The parameters are defined in the deviceIDs file (see option '-d') as:";
            outputMessage << indent3;
            outputMessage << "POWER";
            outputMessage << separator;
            outputMessage << "PWRB20_<id>";
            outputMessage << separator;
            outputMessage << "<offset>";
            outputMessage << separator;
            outputMessage << "<slope>";
            outputMessage << indent2;
            outputMessage << "Define <slope> = -1 to use the factory slope.";
            outputMessage << std::endl;

            outputMessage << indent2;
            outputMessage << "Any CTF power meter is also a cadence sensor and can therefore become a makeshift speed sensor.";
            outputMessage << indent2;
            outputMessage << "To turn a CTF power meter into a speed sensor it must be defined as a such in the deviceIDs file by";
            outputMessage << indent3;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "PWRB20_<id>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<gear_ratio>";
            outputMessage << indent2;
            outputMessage << "and loaded with option '-d'. The values for <wheel_circumference> and <gear_ratio> are optional, the defaults are:";
            outputMessage << indent3;
            outputMessage << "<wheel_circumference> = ";
            outputMessage << C_WHEEL_CIRCUMFERENCE_DEFAULT;
            outputMessage << " (m)";
            outputMessage << indent3;
            outputMessage << "<gear_ratio>          = ";
            outputMessage << C_GEAR_RATIO_DEFAULT;
            outputMessage << " (";
            outputMessage << C_GEAR_RATIO_DEFAULT_AS_RATIO;
            outputMessage << ")";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB20_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_event_count>";
            outputMessage << separator;
            outputMessage << "<delta_time_stamp>";
            outputMessage << separator;
            outputMessage << "<delta_torque_ticks>";
            outputMessage << separator;
            outputMessage << "<factory_slope>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB46" ) )
        {
            outputMessage << "PWRB46 (Request)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB46_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += "DESCRIPTOR_BYTE_1";
            auxBuffer += separator;
            auxBuffer += "<d_byte_1>";
            auxBuffer += separator;
            auxBuffer += "DESCRIPTOR_BYTE_2";
            auxBuffer += separator;
            auxBuffer += "<d_byte_2>";
            auxBuffer += separator;

            auxBuffer1  = separator;
            auxBuffer1 += "REQUESTED_PAGE_NO";
            auxBuffer1 += separator;
            auxBuffer1 += "<page_no>";
            auxBuffer1 += separator;
            auxBuffer1 += "<command_type>";
            auxBuffer1 += separator;
            auxBuffer1 += "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "TRANSMIT_INVALID";
            outputMessage << auxBuffer1;

            outputMessage << auxBuffer;
            outputMessage << "TRANSMIT_UNTIL_SUCCESS";
            outputMessage << auxBuffer1;

            outputMessage << auxBuffer;
            outputMessage << "TRANSMIT_NB_TIMES";
            outputMessage << auxBuffer1;

            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB46_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<descriptor_1>";
            outputMessage << separator;
            outputMessage << "<descriptor_2>";
            outputMessage << separator;
            outputMessage << "<requested_response>";
            outputMessage << separator;
            outputMessage << "<requested_page_number>";
            outputMessage << separator;
            outputMessage << "<command_type>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB50" ) )
        {
            outputMessage << "PWRB50 (Manufacturer Information)";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "PWRB50_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";

            outputMessage << auxBuffer;
            outputMessage << separator;
            outputMessage << "<model_number>";
            outputMessage << separator;
            outputMessage << "<hw_revision>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";

            outputMessage << auxBuffer;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<manufacturer_id>";
            outputMessage << separator;
            outputMessage << "<model_no>";
            outputMessage << separator;
            outputMessage << "<hw_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB51" ) )
        {
            outputMessage << "PWRB51 (Product Information)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB51_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<pm_sw_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB51_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<pm_software_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "POWER" ) || ( deviceTypeUC == "PWRB52" ) )
        {
            outputMessage << "PWRB52 (Battery Status)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB52_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "<number_of_batteries>";
            outputMessage << separator;
            outputMessage << "<battery_id>";
            outputMessage << separator;
            outputMessage << "<voltage>";
            outputMessage << separator;
            outputMessage << "<battery_status>";
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "PWRB52_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<voltage_256>";
            outputMessage << separator;
            outputMessage << "<battery_status>";
            outputMessage << separator;
            outputMessage << "<operation_time>";
            outputMessage << separator;
            outputMessage << "<resolution>";
            outputMessage << separator;
            outputMessage << "<number_of_batteries>";
            outputMessage << separator;
            outputMessage << "<battery_id>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "SBSDM" ) || ( deviceTypeUC == "STRIDE" ) )
        {
            outputMessage << "SBSDM (Stride-Based Speed and Distance Monitor)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "SBSDM_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "<speed>";
            outputMessage << separator;
            outputMessage << "TOTAL_TIME";
            outputMessage << separator;
            outputMessage << "<total_time>";
            outputMessage << separator;
            outputMessage << "TODAL_DIST";
            outputMessage << separator;
            outputMessage << "<total_distance>";
            outputMessage << separator;
            outputMessage << "LATENCY";
            outputMessage << separator;
            outputMessage << "<latency>";
            outputMessage << separator;
            outputMessage << "TOTAL_STRIDE_COUNT";
            outputMessage << separator;
            outputMessage << "<total_stride_count>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2, 4 - 15";
            outputMessage << separator;
            outputMessage << "CADENCE";
            outputMessage << separator;
            outputMessage << "<cadence>";
            outputMessage << separator;
            outputMessage << "DEVICE_LOCATION";
            outputMessage << separator;
            outputMessage << "<device_location>";
            outputMessage << separator;
            outputMessage << "BATTERY_STATUS";
            outputMessage << separator;
            outputMessage << "<battery_status>";
            outputMessage << separator;
            outputMessage << "DEVICE_HEALTH";
            outputMessage << separator;
            outputMessage << "<device_health>";
            outputMessage << separator;
            outputMessage << "USE_STATE";
            outputMessage << separator;
            outputMessage << "<use_state>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3";
            outputMessage << separator;
            outputMessage << "CADENCE";
            outputMessage << separator;
            outputMessage << "<cadence>";
            outputMessage << separator;
            outputMessage << "CALORIES";
            outputMessage << separator;
            outputMessage << "<calories>";
            outputMessage << separator;
            outputMessage << "DEVICE_LOCATION";
            outputMessage << separator;
            outputMessage << "<device_location>";
            outputMessage << separator;
            outputMessage << "BATTERY_STATUS";
            outputMessage << separator;
            outputMessage << "<battery_status>";
            outputMessage << separator;
            outputMessage << "DEVICE_HEALTH";
            outputMessage << separator;
            outputMessage << "<device_health>";
            outputMessage << separator;
            outputMessage << "USE_STATE";
            outputMessage << separator;
            outputMessage << "<use_state>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "16";
            outputMessage << separator;
            outputMessage << "<strides_since _reset>";
            outputMessage << separator;
            outputMessage << "<distance_since_reset>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "22";
            outputMessage << separator;
            outputMessage << "TIME";
            outputMessage << separator;
            outputMessage << "<time_capability>";
            outputMessage << separator;
            outputMessage << "DIST";
            outputMessage << separator;
            outputMessage << "<distance_capability>";
            outputMessage << separator;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "<speed_capability>";
            outputMessage << separator;
            outputMessage << "LATENCY";
            outputMessage << separator;
            outputMessage << "<latency_capability>";
            outputMessage << separator;
            outputMessage << "CANDECE";
            outputMessage << separator;
            outputMessage << "<cadence_capability>";
            outputMessage << separator;
            outputMessage << "CALORIES";
            outputMessage << separator;
            outputMessage << "<calories_capability>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "SBSDM_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += C_SEMI_COOKED_SYMBOL_AS_STRING;
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "<time_fract_part>";
            outputMessage << separator;
            outputMessage << "<delta_time_int_part>";
            outputMessage << separator;
            outputMessage << "<speed_fract_part>";
            outputMessage << separator;
            outputMessage << "<speed_int_part>";
            outputMessage << separator;
            outputMessage << "<delta_stride_count>";
            outputMessage << separator;
            outputMessage << "<latency>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2,4-15";
            outputMessage << separator;
            outputMessage << "<cadence_fract_part>";
            outputMessage << separator;
            outputMessage << "<cadence_int_part>";
            outputMessage << separator;
            outputMessage << "<speed_fract_part>";
            outputMessage << separator;
            outputMessage << "<speed_int_part>";
            outputMessage << separator;
            outputMessage << "<status>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3";
            outputMessage << separator;
            outputMessage << "<cadence_fract_part>";
            outputMessage << separator;
            outputMessage << "<cadence_int_part>";
            outputMessage << separator;
            outputMessage << "<speed_fract_part>";
            outputMessage << separator;
            outputMessage << "<speed_int_part>";
            outputMessage << separator;
            outputMessage << "<calories>";
            outputMessage << separator;
            outputMessage << "<status>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "16";
            outputMessage << separator;
            outputMessage << "<strides_since_reset>";
            outputMessage << separator;
            outputMessage << "<distance_since_reset>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "22";
            outputMessage << separator;
            outputMessage << "<capabilities>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" ) || ( deviceTypeUC == "SPB7" ) || ( deviceTypeUC == "SPEED" ) )
        {
            outputMessage << "SPB7 (Speed Only)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "SPB7_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += "<speed>";
            auxBuffer += separator;
            auxBuffer += "<wheel_circumference>";
            auxBuffer += separator;
            auxBuffer += "<number_of_magnets>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "0/128";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "1/129";
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2/130";
            outputMessage << separator;
            outputMessage << "<manufacturer_id>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3/131";
            outputMessage << separator;
            outputMessage << "<model_number>";
            outputMessage << separator;
            outputMessage << "<sw_version_sensor>";
            outputMessage << separator;
            outputMessage << "<hw_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;

            outputMessage << indent2;
            outputMessage << "The speed-only sensor SPB7 takes 2 parameters";
            outputMessage << indent3;
            outputMessage << "wheel circumference (default: ";
            outputMessage << C_WHEEL_CIRCUMFERENCE_DEFAULT;
            outputMessage << " m),";
            outputMessage << indent3;
            outputMessage << "number of magnets   (default: ";
            outputMessage << C_NB_MAGNETS_DEFAULT;
            outputMessage << ").";
            outputMessage << indent2;
            outputMessage << "The parameters are defined in the deviceIDs file (see option '-d') as:";
            outputMessage << indent3;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "SPB7_<id>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<nb_magnets>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "SPB7_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;
            auxBuffer += C_SEMI_COOKED_SYMBOL_AS_STRING;
            auxBuffer += separator;
            auxBuffer += "<delta_event_time>";
            auxBuffer += separator;
            auxBuffer += "<delta_revolution_count>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "0/128";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "1/129";
            outputMessage << separator;
            outputMessage << "<operating_time>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2/130";
            outputMessage << separator;
            outputMessage << "<manufacturer_id>";
            outputMessage << separator;
            outputMessage << "<serial_number>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3/131";
            outputMessage << separator;
            outputMessage << "<model_number>";
            outputMessage << separator;
            outputMessage << "<software_version>";
            outputMessage << separator;
            outputMessage << "<hardware_version>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" )   || ( deviceTypeUC == "SPCAD790" ) ||
             ( deviceTypeUC == "SPCAD" ) || ( deviceTypeUC == "SPEED" ) )
        {
            outputMessage << "SPCAD790 (Speed and Cadence)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";
            outputMessage << indent2;
            outputMessage << "SPCAD790_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << "<speed>";
            outputMessage << separator;
            outputMessage << "<cadence>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<number_of_magnets>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl;
            outputMessage << indent2;
            outputMessage << "The speed and cadence sensor SPCAD790 takes 2 parameters";
            outputMessage << indent3;
            outputMessage << "wheel circumference (default: ";
            outputMessage << C_WHEEL_CIRCUMFERENCE_DEFAULT;
            outputMessage << " m),";
            outputMessage << indent3;
            outputMessage << "number of magnets   (default: ";
            outputMessage << C_NB_MAGNETS_DEFAULT;
            outputMessage << ").";
            outputMessage << indent2;
            outputMessage << "The parameters are defined in the deviceIDs file (see option '-d') as:";
            outputMessage << indent3;
            outputMessage << "SPEED";
            outputMessage << separator;
            outputMessage << "SPCAD790_<id>";
            outputMessage << separator;
            outputMessage << "<wheel_circumference>";
            outputMessage << separator;
            outputMessage << "<nb_magnets>";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";
            outputMessage << indent2;
            outputMessage << "SPCAD790_<id>";
            outputMessage << separator;
            outputMessage << "<time>";
            outputMessage << separator;
            outputMessage << C_SEMI_COOKED_SYMBOL_AS_STRING;
            outputMessage << separator;
            outputMessage << "<delta_speed_event_time>";
            outputMessage << separator;
            outputMessage << "<delta_wheel_revolution_count>";
            outputMessage << separator;
            outputMessage << "<delta_cadence_event_time>";
            outputMessage << separator;
            outputMessage << "<delta_crank_revolution_count>";
            outputMessage << separator;
            outputMessage << "<sw_version>";
            outputMessage << std::endl << std::endl;
        }
        if ( ( deviceTypeUC == "ALL" )   || ( deviceTypeUC == "WEIGHT" ) ||
             ( deviceTypeUC == "WEIGHT_SCALE" ) || ( deviceTypeUC == "SCALE" ) )
        {
            outputMessage << "WEIGHT (Weight Scale)";
            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Fully Cooked\"";

            auxBuffer  = indent2;
            auxBuffer += "WEIGHT_<id>";
            auxBuffer += separator;
            auxBuffer += "<time>";
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "<user_profile_capability>";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "<ant_fs_channel_availability>";
            outputMessage << separator;
            outputMessage << "<user_profile_exchange_capability>";
            outputMessage << separator;
            outputMessage << "BODY_WEIGHT";
            outputMessage << separator;
            outputMessage << "<body_weight>";
            outputMessage << separator;
            outputMessage << " <sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2";
            outputMessage << separator;
            outputMessage << "<user_profile_capability>";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "HYDRATION";
            outputMessage << separator;
            outputMessage << "<hydration>";
            outputMessage << separator;
            outputMessage << "BODY_FAT";
            outputMessage << separator;
            outputMessage << "<body_fat>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3";
            outputMessage << separator;
            outputMessage << "<user_profile_capability>";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "ACTIVE_METABOLIC_RATE";
            outputMessage << separator;
            outputMessage << "<active_metabolic_rate>";
            outputMessage << separator;
            outputMessage << "BASAL_METABOLIC_RATE";
            outputMessage << separator;
            outputMessage << "<basal_metabolic_rate>";
            outputMessage << separator;
            outputMessage << " <sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "4";
            outputMessage << separator;
            outputMessage << "<user_profile_capability>";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "MUSCLE_MASS";
            outputMessage << separator;
            outputMessage << "<muscle_mass>";
            outputMessage << separator;
            outputMessage << "BONE_MASS";
            outputMessage << separator;
            outputMessage << "<bone_mass>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "58";
            outputMessage << separator;
            outputMessage << "<user_profile_capability>";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "<ant_fs_channel_availability>";
            outputMessage << separator;
            outputMessage << "<user_profile_exchange_capability>";
            outputMessage << separator;
            outputMessage << "GENDER";
            outputMessage << separator;
            outputMessage << "<gender>";
            outputMessage << separator;
            outputMessage << "AGE";
            outputMessage << separator;
            outputMessage << "<age>";
            outputMessage << separator;
            outputMessage << "HEIGHT";
            outputMessage << separator;
            outputMessage << "<height>";
            outputMessage << separator;
            outputMessage << "ATHLETE_TYPE";
            outputMessage << separator;
            outputMessage << "<athlete_type>";
            outputMessage << separator;
            outputMessage << "ACTIVITY_CLASS";
            outputMessage << separator;
            outputMessage << "<activity_class>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl;
            outputMessage << indent;
            outputMessage << "\"Semi-Cooked\"";

            auxBuffer += C_SEMI_COOKED_SYMBOL_AS_STRING;
            auxBuffer += separator;

            outputMessage << auxBuffer;
            outputMessage << "1";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "<capabilities>";
            outputMessage << separator;
            outputMessage << "<body_weight>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "2";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "<hydration>";
            outputMessage << separator;
            outputMessage << "<body_fat>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "3";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "<active_metabolic_rate>";
            outputMessage << separator;
            outputMessage << "<basal_metabolic_rate>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "4";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "<muscle_mass>";
            outputMessage << separator;
            outputMessage << "<bone_mass>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << auxBuffer;
            outputMessage << "58";
            outputMessage << separator;
            outputMessage << "<user_profile>";
            outputMessage << separator;
            outputMessage << "<gender>";
            outputMessage << separator;
            outputMessage << "<age>";
            outputMessage << separator;
            outputMessage << "<height>";
            outputMessage << separator;
            outputMessage << "<descriptor_bit>";
            outputMessage << separator;
            outputMessage << "<sw_version>";

            outputMessage << std::endl << std::endl;
        }
    }
    else
    {
        if ( deviceTypeUC != "?" )
        {
            outputMessage << indent;
            outputMessage << "Unknown device type \"";
            outputMessage << deviceType;
            outputMessage << "\".";
            outputMessage << std::endl;
        }
        outputMessage << std::endl;
        if ( deviceType == "POWER" )
        {
            outputMessage << "Currently supported power meter data pages are";
            outputOptionPOWER( outputMessage, indent );
        }
        else
        {
            outputOptionF( outputMessage, indent );
        }
    }

    std::cout << outputMessage.str() << std::endl;
}

bool antProcessing::processArguments
(
    const amString &programNameIn,
    const amString &validOptionsIn,
    const amString &deviceType,
    int             argc,
    char           *argv[]
)
{
    const char           *prevOption    = 0;
    bool                  running       = true;
    bool                  readStdin     = false;
    bool                  readFile      = false;
    bool                  readMulticast = true;
    bool                  outPort       = false;
    int                   counter       = 0;
    int                   option        = 0;
    int                   intArg        = 0;
    std::vector<amString> labels;
    amString              thisLabel;

    programName  = programNameIn;
    validOptions = validOptionsIn;

    resetAll();

    opterr = 0;

    while ( running && ( option = getopt( argc, argv, validOptions.c_str() ) ) != -1 )
    {
        prevOption = argv[ counter ];
        ++counter;

        switch ( option )
        {
            case '1':
                 readStdin = true;
                 break;
            case '2':
                 setWriteStdout( true );
                 break;
            case 'B':
                 setOutputBridge( true );
                 break;
            case 'D':
                 setDiagnostics( true );
                 break;
            case 'd':
                 ++counter;
                 setDeviceFileName( optarg );
                 break;
            case 'f':
                 readFile = true;
                 setInputFileName( optarg );
                 ++counter;
                 break;
            case 'H':
                 if ( validOptions.contains( "H:" ) )
                 {
                     ++counter;
                     outputFormats( optarg );
                 }
                 else
                 {
                     outputFormats( currentDeviceType );
                 }
                 running = false;
                 break;
            case 'h':
                 help();
                 running = false;
                 break;
            case 'I':
                 setInterface( optarg );
                 ++counter;
                 break;
            case 'J':
                 setOutputAsJSON( true );
                 break;
            case 'l':
                 labels.push_back( optarg );
                 ++counter;
                 break;
            case 'M':
                 setMCAddressIn( optarg );
                 ++counter;
                 outPort = false;
                 break;
            case 'm':
                 setMCAddressOut( optarg );
                 ++counter;
                 outPort = true;
                 break;
            case 'P':
                 intArg = atoi( optarg );
                 setValuePrecision( intArg );
                 ++counter;
                 break;
            case 'p':
                 intArg  = atoi( optarg );
                 if ( outPort )
                 {
                     setMCPortNoOut( intArg );
                 }
                 else
                 {
                     setMCPortNoIn( intArg );
                 }
                 ++counter;
                 break;
            case 'R':
                 setOnlyRegisteredDevices( true );
                 break;
            case 'r':
                 setOutputRaw( true );
                 break;
            case 'S':
                 setSemiCookedOut( true );
                 break;
            case 's':
                 setSemiCookedIn( true );
                 break;
            case 'T':
                 intArg = atoi( optarg );
                 setTimePrecision( intArg );
                 ++counter;
                 break;
            case 't':
                 intArg = atoi( optarg );
                 setTimeOutSec( intArg );
                 ++counter;
                 break;
            case 'U':
                 setOutputUnknown( true );
                 break;
            case 'v':
                 std::cout << b2tVersion << std::endl;
                 running = false;
                 break;
            case 'V':
                 std::cout << programName << " Version " << b2tVersion << C_COPYRIGHT << std::endl;
                 running = false;
                 break;
            case 'w':
                 setExitOnWarnings( true );
                 break;
            case 'x':
                 setTestMode( true );
                 break;
            case '?':
                 running = false;
                 if ( strcmp( argv[ counter ], "-H" ) == 0 )
                 {
                     std::stringstream outputMessage;
                     amString          indent( "\n    " );
                     if ( deviceType == "POWER" )
                     {
                         outputMessage << std::endl;
                         outputMessage << "Currently supported power meter data pages are";
                         outputOptionPOWER( outputMessage, indent );
                         outputMessage << std::endl;
                     }
                     else
                     {
                         outputOptionF( outputMessage, indent );
                     }
                     std::cout << outputMessage.str() << std::endl;
                     break;
                 }
                 errorCode = E_BAD_OPTION;
                 appendErrorMessage( "Command line option '" );
                 appendErrorMessage( prevOption );
                 appendErrorMessage( "' is missing its argument value.\n" );
                 break;
            default:
                 running   = false;
                 errorCode = E_UNKNOWN_OPTION;
                 appendErrorMessage( "Unknown command line option \"" );
                 appendErrorMessage( argv[ counter ] );
                 appendErrorMessage( "\"" );
                 break;
        }
    }

    if ( running && testMode && ( errorCode == 0 ) )
    {
        testCounter = 0;
        setTimePrecision( C_DEFAULT_TIME_PRECISION_DEFAULT );
        setValuePrecision( C_DEFAULT_VALUE_PRECISION_DEFAULT );
    }

    if ( running && ( errorCode == 0 ) )
    {
        // -------------------------------
        // Deal with source of input
        // -------------------------------
        if ( readFile )
        {
            readMulticast = readStdin = false;
            setMCAddressIn( "" );
        }
        if ( readStdin )
        {
            readMulticast = readFile = false;
            setInputFileName( "" );
            setMCAddressIn( "" );
        }
        if ( readMulticast )
        {
            if ( C_DEFAULT_INTERFACE == 0 )
            {
                errorCode = E_MC_NO_INTERFACE;
            }
        }
    }

    if ( running && ( errorCode == 0 ) )
    {
        // -------------------------------
        // Deal with output target
        // -------------------------------
        if ( !getMCAddressOut().empty() )
        {
            if ( getMCPortNoOut() <= 0 )
            {
                errorCode     = E_MC_NO_IP_ADDRESS;
                appendErrorMessage( "IP address for multicast write connection is present, port number is missing." );
            }
        }
    }

    if ( running && ( errorCode == 0 ) )
    {
        // -------------------------------
        // Start reading ...
        // -------------------------------
        if ( getMCAddressOut().empty() || ( getMCPortNoOut() <= 0 ) )
        {
            // Make sure that the output goes somewhere....
            setWriteStdout( true );
        }
    }

    return running;
}

void antProcessing::setZeroTimeCount
(
    const amString &sensorID,
    unsigned int    value
)
{
    if ( zeroTimeCountTable.count( sensorID ) > 0 )
    {
        zeroTimeCountTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
    }
    zeroTimeCountTable[ sensorID ] = value;
}

unsigned int antProcessing::getZeroTimeCount
(
    const amString &sensorID
)
{
    unsigned int value = 0;
    if ( zeroTimeCountTable.count( sensorID ) > 0 )
    {
        value = zeroTimeCountTable[ sensorID ];
    }
    return value;
}

void antProcessing::getUnixTimeAsString
(
    amString &timeStampBuffer
)
{
    double subSecondTimer = 0;
    if ( testMode )
    {
        subSecondTimer = ( double ) testCounter * 0.1;
        ++testCounter;
    }
    else
    {
        subSecondTimer = getUnixTime();
    }
    getUnixTimeAsString( timeStampBuffer, subSecondTimer );
}

void antProcessing::getUnixTimeAsString
(
    amString &timeStampBuffer,
    double    subSecondTimer
)
{
    timeStampBuffer = amString( subSecondTimer, timePrecision );
}

void antProcessing::setTotalOperationTime
(
    const amString &sensorID,
    double          value
)
{
    if ( totalOperatingTimeTable.count( sensorID ) == 0 )
    {
        totalOperatingTimeTable.insert( std::pair<amString, double>(  sensorID, 0 ) );
    }
    totalOperatingTimeTable[ sensorID ] = value;
}

double antProcessing::getTotalOperationTime
(
    const amString &sensorID
)
{
    double result = 0;
    if ( totalOperatingTimeTable.count( sensorID ) > 0 )
    {
        result = totalOperatingTimeTable[ sensorID ];
    }
    return result;
}

double antProcessing::getUnixTime
(
    void
)
{
    struct timeval tv; 
    gettimeofday( &tv, NULL );
    double result = ( double ) tv.tv_sec + ( ( double ) tv.tv_usec ) / 1.0E6;

    return result;
}

int antProcessing::hex2Int
(
    BYTE b4, 
    BYTE b3, 
    BYTE b2, 
    BYTE b1
)
{
    int res = hex2Int( b4 );
    res <<= 24; 
    res += hex2Int( b3, b2, b1 );
    return res;
}

int antProcessing::hex2Int 
(
    BYTE b3, 
    BYTE b2, 
    BYTE b1
)
{
    int res = hex2Int( b3 );
    res <<= 16; 
    res += hex2Int( b2, b1 );
    return res;
}

int antProcessing::hex2Int 
(
    BYTE b2, 
    BYTE b1
)
{
    int res = hex2Int( b2 );
    res <<= 8;
    res += hex2Int( b1 );
    return res;
}

int antProcessing::hex2Int 
(
    BYTE b1
)
{
    int res = ( unsigned int ) b1; 
    return res;
}

void antProcessing::outputError
(
    void
)
{
    switch ( errorCode )
    {
        case 0:
             appendErrorMessage( "Exited without error." );
             break;
        case E_BAD_PARAMETER_VALUE:
        case E_MC_WRITE_FAIL:
        case E_MC_NO_INTERFACE:
        case E_MC_NO_IP_ADDRESS:
        case E_NO_IP_ADDRESS_IF:
        case E_MC_NO_PORT_NUMBER:
        case E_BAD_OPTION:
        case E_UNKNOWN_OPTION:
        case E_SOCKET_CREATE_FAIL:
        case E_SOCKET_SET_OPT_FAIL:
        case E_LOOP_BACK_IP_ADDRESS:
        case E_SOCKET_BIND_FAIL:
        case E_READ_ERROR:
        case E_READ_FILE_NOT_OPEN:
        case E_READ_TIMEOUT:
             break;
        default:
             appendErrorMessage( "Unknown error code " );
             appendErrorMessage( errorCode );
             appendErrorMessage( ".\n" );
             break;
    }

    std::cerr << std::endl;
    std::cerr << programName;
    std::cerr << ": ";
    std::cerr << std::endl;
    std::cerr << errorMessage;
    std::cerr << std::endl;
}

