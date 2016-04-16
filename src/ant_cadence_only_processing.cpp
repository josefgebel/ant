// -------------------------------------------------------------------------------------------------------------------------

// System C++ libraries
#include <iostream>
#include <fstream>
#include <iomanip>

// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "b2t_utils.h"

#include "ant_cadence_only_processing.h"

// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// CADENCE ONLY ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antCadenceOnlyProcessing::antCadenceOnlyProcessing
(
    void
) : antProcessing(),
    antCadenceSpeedProcessing()
{
    reset();
}

bool antCadenceOnlyProcessing::isCadenceOnlySensor
(
    const std::string &deviceID
)
{
    bool result = startsWith( deviceID, C_CAD_DEVICE_HEAD );
    return result;
}

bool antCadenceOnlyProcessing::appendCadenceOnlySensor
(
    const std::string &sensorID
)
{
    bool result = isCadenceOnlySensor( sensorID );
    if ( result )
    {
        result = appendCadenceSensor( sensorID );
        if ( result )
        {
            if ( !isRegisteredDevice( sensorID ) )
            {
                registerDevice( sensorID );
            }
        }
    }
    return result;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// CADENCE
//
// CAD7A: Cadence only Sensor
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//

unsigned int antCadenceOnlyProcessing::splitFormat137_CAD7A
(
    const char    *inputBuffer,
    amSplitString &outWords
)
{
    amSplitString inWords;
    unsigned int  nbWords = inWords.split( inputBuffer );

    // Convert Version 1.37:
    //     <sensor_id>  <time_stamp>    <S>   <dataPage>          <deltaEventTime>    <deltaRevCount>   ...
    // to Version 2.0:
    //     <sensor_id>  <time_stamp>    <S>   <deltaEventTime>    <deltaRevCount>     <dataPage>        ...

    if ( nbWords > 5 )
    {
        unsigned int counter = 0;
        std::string  dataPage;
        std::string  deltaEventTime;
        std::string  deltaRevCount;

        outWords.push_back( inWords[ counter++ ] );       // Device ID
        outWords.push_back( inWords[ counter++ ] );       // Time Stamp
        outWords.push_back( inWords[ counter++ ] );       // Semi-cooked Symbol
        dataPage       = inWords[ counter++ ];            // Data Page
        deltaEventTime = inWords[ counter++ ];            // Delta Event Time
        deltaRevCount  = inWords[ counter++ ];            // Delta Revolution Count
        outWords.push_back( deltaEventTime );
        outWords.push_back( deltaRevCount );
        outWords.push_back( dataPage );
        while ( counter < nbWords )
        {
            outWords.push_back( inWords[ counter++ ] );   // ...
        }
    }

    return outWords.size();
}

// ---------------------------------------------------------------------------------------------------
//
// processCadenceOnlySensor
//
// Convert the raw ant data into semi-cooked text data and put the result string into the resultBuffer.
// The output string has the form
// If semiCookedOut == true
//     Depending on the value of <data_page> & 127:
//        1: "CAD7A_<device_ID> <data_page> <event_time> <revolution_count> <operating_time>"
//        2: "CAD7A_<device_ID> <data_page> <event_time> <revolution_count> <manufacturer_ID> <serial_number>"
//        3: "CAD7A_<device_ID> <data_page> <event_time> <revolution_count> <hardware_version> <software_version> <model_number>"
// Otherwise,
//     "CAD7A_<device_ID> <sub_second_timer> <cadence>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antCadenceOnlyProcessing::processCadenceOnlySensor
(
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
    amDeviceType result                            = OTHER_DEVICE;
    std::string      sensorID                      = std::string( C_CAD_DEVICE_HEAD ) + deviceIDNo;
    unsigned int dataPage                          = 0;
    unsigned int eventTime                         = 0;
    unsigned int revCount                          = 0;
    unsigned int deltaRevCount                     = 0;
    unsigned int deltaEventTime                    = 0;
    unsigned int additionalData1                   = 0;
    unsigned int additionalData2                   = 0;
    unsigned int additionalData3                   = 0;
    unsigned int rollOver                          = 0;
    bool         rollOverHappened                  = false;
    bool         commonPage                        = false;
    bool         outputPageNo                      = true;

    if ( isRegisteredDevice( sensorID ) )
    {
        if ( ( eventTimeTable.count    ( sensorID ) == 0 ) ||
             ( eventCountTable.count   ( sensorID ) == 0 ) ||
             ( operatingTimeTable.count( sensorID ) == 0 ) )
        {
            eventTimeTable.insert    ( std::pair<std::string, unsigned int>( sensorID, 0 ) );
            eventCountTable.insert   ( std::pair<std::string, unsigned int>( sensorID, 0 ) );
            operatingTimeTable.insert( std::pair<std::string, unsigned int>( sensorID, 0 ) );
        }

        // - - - - - - - - - - - - - - - - - - - - -
        // Data Page
        dataPage = hex( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        // - - - - - - - - - - - - - - - - - - - - -
        // Event Time
        eventTime      = hex( payLoad[ 5 ], payLoad[ 4 ] );
        rollOver       = 65536;  // 256^2
        deltaEventTime = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, eventTime );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Bike Cadence Event Time", payLoad[ 5 ], payLoad[ 4 ], eventTime );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Bike Cadence Event Time", deltaEventTime, auxBuffer );
        }


        // - - - - - - - - - - - - - - - - - - - - -
        // Cumulated Wheel Count
        revCount      = hex( payLoad[ 7 ], payLoad[ 6 ] );
        rollOver      = 65536;  // 256^2
        deltaRevCount = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, revCount );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Cumulative Cadence Revolution Count", payLoad[ 7 ], payLoad[ 6 ], revCount );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Cumulative Cadence Revolution Count", deltaRevCount, auxBuffer );
            *auxBuffer = 0;
        }

        switch ( dataPage & 0x0F )
        {
            case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                     result = CADENCE_SENSOR;
                     break;

            case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                     result          = CADENCE_SENSOR;
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
                     result          = CADENCE_SENSOR;
                     additionalData1 = hex( payLoad[ 1 ] );                // Manufacturer ID
                     additionalData2 = hex( payLoad[ 3 ], payLoad[ 2 ] );  // Serial Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Manufacturer ID", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Serial Number", payLoad[ 3 ], payLoad[ 2 ], additionalData2 );
                     }
                     break;

            case  3: // - - Page 3: Product Information  - - - - - - - - - - - - - -
                     result          = CADENCE_SENSOR;
                     additionalData1 = hex( payLoad[ 1 ] );   // H/W Version
                     additionalData2 = hex( payLoad[ 2 ] );   // S/W Version
                     additionalData3 = hex( payLoad[ 3 ] );   // Model Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Hardware Version", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Software Version", payLoad[ 2 ], additionalData2 );
                         appendDiagnosticsLine( "Model Number",     payLoad[ 3 ], additionalData3 );
                     }
                     break;

            default: commonPage = true;
                     result     = CADENCE_SENSOR;
                     break;
        }
    }

    if ( result == CADENCE_SENSOR )
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
            unsigned int cadence = getCadence( sensorID );
            if ( isUsedAsSpeedSensor( sensorID ) )
            {
                double wheelCircumference = getWheelCircumference( sensorID );
                double gearRatio          = getNbMagnets( sensorID );
                double speed              = getSpeed( sensorID );
                createCadenceResultString
                (
                    cadence,
                    dataPage,
                    deltaRevCount,
                    deltaEventTime,
                    additionalData1,
                    additionalData2,
                    additionalData3,
                    speed,
                    wheelCircumference,
                    gearRatio
                );
                setSpeed( sensorID, speed );
            }
            else
            {
                createCadenceResultString( cadence, dataPage, deltaRevCount, deltaEventTime, additionalData1, additionalData2, additionalData3 );
            }
            setCadence( sensorID, cadence );
        }
        appendOutputFooter( b2tVersion );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = strToInt( deviceIDNo );
            createUnknownDeviceTypeString( C_CAD_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processCadenceOnlySensorSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the resultBuffer.
// The output string has the form
//     "CAD7A_<device_ID> <sub_second_timer> <cadence>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antCadenceOnlyProcessing::processCadenceOnlySensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        unsigned int  nbWords         = 0;
        unsigned int  counter         = 0;
        unsigned int  startCounter    = 0;
        unsigned int  dataPage        = 0;
        unsigned int  deltaRevCount   = 0;
        unsigned int  deltaEventTime  = 0;
        unsigned int  additionalData1 = 0;
        unsigned int  additionalData2 = 0;
        unsigned int  additionalData3 = 0;
        bool          commonPage      = false;
        bool          outputPageNo    = true;
        std::string   curVersion      = b2tVersion;
        std::string   sensorID;
        std::string   semiCookedString;
        std::string   timeStampBuffer;
        amSplitString words;

        if ( isSemiCookedFormat137( inputBuffer ) )
        {
            nbWords = splitFormat137_CAD7A( inputBuffer, words );
        }
        else
        {
            nbWords = words.split( inputBuffer );
        }

        if ( nbWords > 5 )
        {
            sensorID         = words[ counter++ ];                                           // 0
            timeStampBuffer  = words[ counter++ ];                                           // 1
            semiCookedString = words[ counter++ ];                                           // 2
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isCadenceOnlySensor( sensorID ) )
            {
                startCounter   = counter;
                deltaRevCount  = ( unsigned int ) strToInt( words[ counter++ ] );                         // 3
                deltaEventTime = ( unsigned int ) strToInt( words[ counter++ ] );                         // 4
                dataPage       = ( unsigned int ) strToInt( words[ counter++ ] );                         // 5
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Data Page", dataPage );
                    appendDiagnosticsLine( "Delta Revolution Count", deltaRevCount );
                    appendDiagnosticsLine( "Delta Event Time", deltaEventTime );
                }

                switch ( dataPage & 0x0F )
                {
                    case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                             result = CADENCE_SENSOR;
                             break;

                    case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                             if ( nbWords > 6 )
                             {
                                 result          = CADENCE_SENSOR;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );   // deltaOperatingTime
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Delta Cumulative Operating Time", additionalData1 );
                                 }
                             }
                             break;

                    case  2: // - - Page 2: Manufacturer Information - - - - - - - - - - - -
                             if ( nbWords > 7 )
                             {
                                 result          = CADENCE_SENSOR;
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
                             if ( nbWords > 8 )
                             {
                                 result          = CADENCE_SENSOR;
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

                    default: counter    = startCounter;
                             result     = CADENCE_SENSOR;
                             commonPage = true;
                             break;
                }
            }
        }

        if ( result == CADENCE_SENSOR )
        {
            if ( nbWords > counter )
            {
                curVersion = words[ counter++ ];
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
                unsigned int cadence = getCadence( sensorID );
                if ( isUsedAsSpeedSensor( sensorID ) )
                {
                    double wheelCircumference = getWheelCircumference( sensorID );
                    double gearRatio          = getNbMagnets( sensorID );
                    double speed              = getSpeed( sensorID );
                    createCadenceResultString
                    (
                        cadence,
                        dataPage,
                        deltaRevCount,
                        deltaEventTime,
                        additionalData1,
                        additionalData2,
                        additionalData3,
                        speed,
                        wheelCircumference,
                        gearRatio
                    );
                    setSpeed( sensorID, speed );
                }
                else
                {
                    createCadenceResultString( cadence, dataPage, deltaRevCount, deltaEventTime, additionalData1, additionalData2, additionalData3 );
                }
                setCadence( sensorID, cadence );
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
// the result string into the resultBuffer.
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
amDeviceType antCadenceOnlyProcessing::processSensor
(
    int                deviceType,
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_CAD_TYPE )
    {
        result = processCadenceOnlySensor( deviceIDNo, timeStampBuffer, payLoad );
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

amDeviceType antCadenceOnlyProcessing::processSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        // Cadence-Only Sensor
        if ( isCadenceOnlySensor( inputBuffer ) )
        {
            result = processCadenceOnlySensorSemiCooked( inputBuffer );
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

int antCadenceOnlyProcessing::readDeviceFileStream
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
            else if ( isCadenceOnlySensor( deviceName ) )
            {
                if ( deviceType == C_CADENCE_DEVICE_ID )
                {
                    antCadenceProcessing::evaluateDeviceLine( words );
                }
                else if ( deviceType == C_SPEED_DEVICE_ID )
                {
                    antCadenceSpeedProcessing::evaluateDeviceLine( words );
                }
            }
        }
    }

    return errorCode;
}

void antCadenceOnlyProcessing::reset
(
    void
)
{
    antProcessing::reset();
    antCadenceSpeedProcessing::reset();
}

void antCadenceOnlyProcessing::createCadenceResultString
(
    unsigned int &cadence,
    unsigned int  dataPage,
    unsigned int  deltaRevolutionCount,
    unsigned int  deltaEventTime,
    unsigned int  additionalData1,
    unsigned int  additionalData2,
    unsigned int  additionalData3
)
{
    if ( !semiCookedOut )
    {
        cadence = computeCadence( cadence, deltaRevolutionCount, deltaEventTime );
    }

    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "delta event time",       deltaEventTime );
            appendJSONItem( "delta revolution count", deltaRevolutionCount );
        }
        else
        {
            appendJSONItem( C_CADENCE_JSON, cadence );
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
            appendJSONItem( C_HARDWARE_REVISION_JSON, additionalData1 );
            appendJSONItem( C_SOFTWARE_REVISION_JSON, additionalData2 );
            appendJSONItem( C_MODEL_NUMBER_JSON,      additionalData3 );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( deltaEventTime );
            appendOutput( deltaRevolutionCount );
            appendOutput( dataPage );

            if ( ( ( dataPage & 0x0F ) == 1 ) || ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) )
            {
                appendOutput( additionalData1 );             // deltaOperatingTime (1) / manufacturerID (2) / hwVersion (3)
                if ( ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) )
                {
                     appendOutput( additionalData2 );        // serialNumber (2) / swVersion (3)
                     if ( ( dataPage & 0x0F ) == 3 )
                     {
                          appendOutput( additionalData3 );   // modelNumber (3)
                    }
                }
            }
        }
        else
        {
            appendOutput( cadence );
            appendOutput( dataPage );
            if ( ( dataPage & 0x0F ) == 1 )
            {
                appendOutput( 2 * additionalData1, "s" );    // operatingTimeSeconds
            }
            else if ( ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) )
            {
                appendOutput( additionalData1 );             // manufacturerID (2) / hwVersion (3)
                appendOutput( additionalData2 );             // serialNumber (2)   / swVersion (3)
                if ( ( dataPage & 0x0F ) == 3 )
                {
                    appendOutput( additionalData3 );         // modelNumber (3)
                }
            }
        }
    }
}

void antCadenceOnlyProcessing::createCadenceResultString
(
    unsigned int &cadence,
    unsigned int  dataPage,
    unsigned int  deltaRevolutionCount,
    unsigned int  deltaEventTime,
    unsigned int  additionalData1,
    unsigned int  additionalData2,
    unsigned int  additionalData3,
    double       &speed,
    double        wheelCircumference,
    double        gearRatio
)
{
    if ( !semiCookedOut )
    {
        cadence = computeCadence( cadence, deltaRevolutionCount, deltaEventTime );
        speed   = antCadenceSpeedProcessing::computeSpeed( cadence, wheelCircumference, gearRatio );
    }

    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "delta event time",       deltaEventTime );
            appendJSONItem( "delta revolution count", deltaRevolutionCount );
        }
        else
        {
            appendJSONItem( C_CADENCE_JSON, cadence );
            if ( ( wheelCircumference > 0 ) && ( gearRatio > 0 ) )
            {
                appendJSONItem( "speed",               speed,              getValuePrecision() );
                appendJSONItem( "wheel circumference", wheelCircumference, getValuePrecision() );
                appendJSONItem( "gear ratio",          gearRatio,          getValuePrecision() );
            }
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
            appendJSONItem( C_HARDWARE_REVISION_JSON, additionalData1 );
            appendJSONItem( C_SOFTWARE_REVISION_JSON, additionalData2 );
            appendJSONItem( C_MODEL_NUMBER_JSON,      additionalData3 );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( deltaEventTime );
            appendOutput( deltaRevolutionCount );
            appendOutput( dataPage );
            if ( ( ( dataPage & 0x0F ) == 1 ) || ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) )
            {
                appendOutput( additionalData1 );             // deltaOperatingTime (1) / manufacturerID (2) / hwVersion (3)
                if ( ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) )
                {
                     appendOutput( additionalData2 );        // serialNumber (2) / swVersion (3)
                     if ( ( dataPage & 0x0F ) == 3 )
                     {
                          appendOutput( additionalData3 );   // modelNumber (3)
                    }
                }
            }
        }
        else
        {
            appendOutput( cadence );
            appendOutput( dataPage );
            appendOutput( speed,              getValuePrecision() );
            appendOutput( wheelCircumference, 3 );
            appendOutput( gearRatio,          getValuePrecision() );

            if ( ( dataPage & 0x0F ) == 1 )
            {
                appendOutput( 2 * additionalData1, "s" );    // operatingTimeSeconds
            }
            else if ( ( ( dataPage & 0x0F ) == 2 ) || ( ( dataPage & 0x0F ) == 3 ) )
            {
                appendOutput( additionalData1 );             // manufacturerID (2) / hwVersion (3)
                appendOutput( additionalData2 );             // serialNumber (2)   / swVersion (3)
                if ( ( dataPage & 0x0F ) == 3 )
                {
                    appendOutput( additionalData3 );         // modelNumber (3)
                }
            }
        }
    }
}

