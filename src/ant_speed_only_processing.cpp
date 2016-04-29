// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_speed_only_processing.h"

// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// SPEED ONLY ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antSpeedOnlyProcessing::antSpeedOnlyProcessing
(
    void
) : antProcessing(),
    antSpeedProcessing()
{
    reset();
    setCurrentDeviceType( "SPB7" );
    setMaxZeroTime( C_MAX_ZERO_TIME_SPEED );
}

bool antSpeedOnlyProcessing::isSpeedOnlySensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_SPEED_DEVICE_HEAD ) || deviceID.startsWith( C_SPEED_OBSOLETE_HEAD );
    return result;
}

void antSpeedOnlyProcessing::replaceObsoleteHeader
(
    amString &sensorID
)
{
    if ( sensorID.startsWith( C_SPEED_OBSOLETE_HEAD ) )
    {
        sensorID.replace( 0, strlen( C_SPEED_OBSOLETE_HEAD ), C_SPEED_DEVICE_HEAD );
    }
}

bool antSpeedOnlyProcessing::appendSpeedSensor
(
    const amString &sensorID,
    double          wheelCircumference,
    double          nbMagnets
)
{
    bool result = isSpeedOnlySensor( sensorID );
    if ( result )
    {
        result = antSpeedProcessing::appendSpeedSensor( sensorID, wheelCircumference, nbMagnets );
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
amDeviceType antSpeedOnlyProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_SPEED_TYPE )
    {
        result = processBikeSpeedSensor( deviceIDNo, timeStampBuffer, payLoad );
    }

    if ( deviceType == OTHER_DEVICE )
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

amDeviceType antSpeedOnlyProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        if ( isSpeedOnlySensor( inputBuffer ) )
        {
            result = processBikeSpeedSensorSemiCooked( inputBuffer );
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

void antSpeedOnlyProcessing::reset
(
    void
)
{
    antProcessing::reset();
    antSpeedProcessing::reset();
}

void antSpeedOnlyProcessing::readDeviceFileLine
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
            amString deviceName = words[ 1 ];
 
            if ( isSpeedOnlySensor( deviceName ) )
            {
                if ( deviceType == C_SPEED_DEVICE_ID )
                {
                    amString curErrorMessage;
                    errorCode = antSpeedProcessing::readDeviceFileLine1( line, curErrorMessage );
                    if ( errorCode )
                    {
                        appendErrorMessage( curErrorMessage );
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// SPB7: Speed only Sensor
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antSpeedOnlyProcessing::processBikeSpeedSensor
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
    unsigned int dataPage                          = 0;
    unsigned int bikeSpeedEventTime                = 0;
    unsigned int deltaBikeSpeedEventTime           = 0;
    unsigned int wheelRevolutionCount              = 0;
    unsigned int deltaWheelRevolutionCount         = 0;
    unsigned int additionalData1                   = 0;
    unsigned int additionalData2                   = 0;
    unsigned int additionalData3                   = 0;
    unsigned int rollOver                          = 0;
    bool         rollOverHappened                  = false;
    bool         commonPage                        = false;
    bool         outputPageNo                      = true;
    amDeviceType result                            = OTHER_DEVICE;
    amString     sensorID                          = amString( C_SPEED_DEVICE_HEAD ) + deviceIDNo;

    if ( isRegisteredDevice( sensorID ) )
    {
        if ( ( eventTimeTable.count    ( sensorID ) == 0 ) ||
             ( eventCountTable.count   ( sensorID ) == 0 ) ||
             ( operatingTimeTable.count( sensorID ) == 0 ) )
        {
            eventTimeTable.insert    ( std::pair<amString, unsigned int>( sensorID, 0 ) );
            eventCountTable.insert   ( std::pair<amString, unsigned int>( sensorID, 0 ) );
            operatingTimeTable.insert( std::pair<amString, unsigned int>( sensorID, 0 ) );
            setSpeed( sensorID, 0 );
        }

        dataPage = byte2UInt( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        bikeSpeedEventTime      = byte2UInt( payLoad[ 5 ], payLoad[ 4 ] );
        rollOver                = 65536;  // 256^2
        deltaBikeSpeedEventTime = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, bikeSpeedEventTime );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Bike Speed Event Time", payLoad[ 5 ], payLoad[ 4 ], bikeSpeedEventTime );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Bike Speed Event Time", deltaBikeSpeedEventTime, auxBuffer );
        }

        wheelRevolutionCount      = byte2UInt( payLoad[ 7 ], payLoad[ 6 ] );
        rollOver                  = 65536;  // 256^2
        deltaWheelRevolutionCount = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, wheelRevolutionCount );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Cumulative Wheel Revolution Count", payLoad[ 7 ], payLoad[ 6 ], wheelRevolutionCount );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Cumulative Wheel Revolution Count", deltaWheelRevolutionCount, auxBuffer );
        }

        switch ( dataPage & 0x0F )
        {
            case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                     result = SPEED_SENSOR;
                     break;

            case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                     result          = SPEED_SENSOR;
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
                     result          = SPEED_SENSOR;
                     additionalData1 = byte2UInt( payLoad[ 1 ] );                // Manufacturer ID
                     additionalData2 = byte2UInt( payLoad[ 3 ], payLoad[ 2 ] );  // Serial Number
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "Manufacturer ID", payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Serial Number", payLoad[ 3 ], payLoad[ 2 ], additionalData2 );
                     }
                     break;

            case  3: // - - Page 3: Product Information  - - - - - - - - - - - - - -
                     result          = SPEED_SENSOR;
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

            default: commonPage = true;
                     result = SPEED_SENSOR;
                     break;
        }
    }

    if ( result == SPEED_SENSOR )
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
            unsigned int nbMagnets          = ( unsigned int) round( getNbMagnets( sensorID ) );
            unsigned int zeroTime           = getZeroTimeCount( sensorID );
            double       wheelCircumference = getWheelCircumference( sensorID );
            double       speed              = getSpeed( sensorID );
            createSPB7ResultString
            (
                speed,
                dataPage,
                deltaBikeSpeedEventTime,
                deltaWheelRevolutionCount,
                additionalData1,
                additionalData2,
                additionalData3,
                wheelCircumference,
                nbMagnets,
                zeroTime
            );
            setZeroTimeCount( sensorID, zeroTime );
            setSpeed( sensorID, speed );
        }
        appendOutputFooter( getVersion() );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = deviceIDNo.toInt();
            createUnknownDeviceTypeString( C_SPEED_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

unsigned int antSpeedOnlyProcessing::splitFormat137_SPB7
(
    const amString &inputBuffer,
    amSplitString &outWords
)
{
    unsigned int nbWords = 0;
    if ( !inputBuffer.empty() )
    {
        amSplitString inWords;
        nbWords = inWords.split( inputBuffer );

        // Convert Version 1.37:
        //     <sensor_id>  <time_stamp>    <S>   <dataPage>                   <deltaBikeSpeedEventTime>     <deltaWheelRevolutionCount>   ...
        // to Version 2.0:
        //     <sensor_id>  <time_stamp>    <S>   <deltaBikeSpeedEventTime>    <deltaWheelRevolutionCount>   <dataPage>...

        if ( nbWords > 5 )
        {
            unsigned int counter = 0;
            amString  dataPage;
            amString  deltaEventTime;
            amString  deltaRevolutionCount;

            outWords.push_back( inWords[ counter++ ] );       // Device ID
            outWords.push_back( inWords[ counter++ ] );       // Time Stamp
            outWords.push_back( inWords[ counter++ ] );       // Semi-cooked Symbol

            dataPage             = inWords[ counter++ ];            // Data Page
            deltaEventTime       = inWords[ counter++ ];            // Delta Event Time
            deltaRevolutionCount = inWords[ counter++ ];            // Delta Revolution Count

            outWords.push_back( dataPage );
            outWords.push_back( deltaEventTime );
            outWords.push_back( deltaRevolutionCount );

            if ( ( ( dataPage == "2" ) || ( dataPage == "3" ) ) && ( nbWords > 6 ) && ( inWords[ counter ].find_first_of( "=" ) != amString::npos ) )
            {
                amSplitString additionalWords;
                additionalWords.removeAllSeparators();
                additionalWords.addSeparator( '=' );
                unsigned int  nbAdditionalWords = additionalWords.split( inWords[ counter ] );
                for ( int addCounter = 0; addCounter < nbAdditionalWords; ++addCounter )
                {
                    outWords.push_back( additionalWords[ addCounter ] );
                }
                counter++;
            }

            while ( counter < nbWords )
            {
                outWords.push_back( inWords[ counter++ ] );   // ...
            }
        }
        nbWords = outWords.size();
    }

    return nbWords;
}

// ---------------------------------------------------------------------------------------------------
//
// processBikeSpeedSensorSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
//     "SPB7_<device_ID> <data_page> <bike_speed_event_time> <wheel_revolution_count> ..."
// Depending on the value of <data_page> & 127:
//    1: "SPB7_<device_ID> <data_page> <bike_speed_event_time> <wheel_revolution_count> <operating_time>"
//    2: "SPB7_<device_ID> <data_page> <bike_speed_event_time> <wheel_revolution_count> <manufacturer_ID> <serial_number>"
//    3: "SPB7_<device_ID> <data_page> <bike_speed_event_time> <wheel_revolution_count> <hardware_version> <software_version> <model_number>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antSpeedOnlyProcessing::processBikeSpeedSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType  result = OTHER_DEVICE;
    amString      curVersion                = getVersion();
    amString      semiCookedString;
    amString      sensorID;
    amString      timeStampBuffer;
    amSplitString words;
    unsigned int  nbWords                   = 0;
    unsigned int  counter                   = 0;
    unsigned int  startCounter              = 0;
    unsigned int  dataPage                  = 0;
    unsigned int  deltaBikeSpeedEventTime   = 0;
    unsigned int  deltaWheelRevolutionCount = 0;
    unsigned int  additionalData1           = 0;
    unsigned int  additionalData2           = 0;
    unsigned int  additionalData3           = 0;
    bool          commonPage                = false;
    bool          outputPageNo              = true;

    if ( isSemiCookedFormat137( inputBuffer ) )
    {
        nbWords = splitFormat137_SPB7( inputBuffer, words );
    }
    else
    {
        nbWords = words.split( inputBuffer );
    }

    if ( nbWords > 5 )
    {
        sensorID         = words[ counter++ ];                            // 0
        timeStampBuffer  = words[ counter++ ];                            // 1
        semiCookedString = words[ counter++ ];                            // 2
        replaceObsoleteHeader( sensorID );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "SensorID",   sensorID );
            appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
            appendDiagnosticsLine( "SemiCooked", semiCookedString );
        }
        if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isSpeedOnlySensor( sensorID ) )
        {
            startCounter = counter;
            dataPage     = words[ counter++ ].toUInt();                   // 3
            if ( words[ counter ] == C_UNSUPPORTED_DATA_PAGE )
            {
                result = UNKNOWN_DEVICE;
            }
            else
            {
                deltaBikeSpeedEventTime   = words[ counter++ ].toUInt();  // 4
                deltaWheelRevolutionCount = words[ counter++ ].toUInt();  // 5
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Data Page", dataPage );
                    appendDiagnosticsLine( "Delta Bike Speed Event Time", deltaBikeSpeedEventTime );
                    appendDiagnosticsLine( "Delta Cumulative Wheel Count", deltaWheelRevolutionCount );
                }

                int dataPageMod128 = dataPage & 0x0F;
                switch ( dataPageMod128 )
                {
                    case  0: // - - Page 0: No Additional Data - - - - - - - - - - - - - - -
                             result = SPEED_SENSOR;
                             break;

                    case  1: // - - Page 1: Operating Time - - - - - - - - - - - - - - - - -
                             if ( nbWords > 6 )
                             {
                                 result          = SPEED_SENSOR;
                                 additionalData1 = words[ counter++ ].toUInt();   // 6 -deltaOperatingTime
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Delta Cumulative Operating Time", additionalData1 );
                                 }
                             }
                             break;

                    case  2: // - - Page 2: Manufacturer Information - - - - - - - - - - - -
                             if ( nbWords > 7 )
                             {
                                 result          = SPEED_SENSOR;
                                 additionalData1 = words[ counter++ ].toUInt();   // 7 - manufacturerID
                                 additionalData2 = words[ counter++ ].toUInt();   // 8 - serialNumber
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
                                 result          = SPEED_SENSOR;
                                 additionalData1 = words[ counter++ ].toUInt();   //  9 - hwVersion
                                 additionalData2 = words[ counter++ ].toUInt();   // 10 - swVersion
                                 additionalData3 = words[ counter++ ].toUInt();   // 11 - modelNumber
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "Model Number", additionalData1 );
                                     appendDiagnosticsLine( "Software Version", additionalData2 );
                                     appendDiagnosticsLine( "Hardware Version", additionalData3 );
                                 }
                             }
                             break;

                    default: counter    = startCounter;
                             result     = SPEED_SENSOR;
                             commonPage = true;
                             break;
                }
            }
        }

        if ( result == SPEED_SENSOR )
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
                unsigned int nbMagnets          = ( unsigned int) round( getNbMagnets( sensorID ) );
                unsigned int zeroTime           = getZeroTimeCount( sensorID );
                double       wheelCircumference = getWheelCircumference( sensorID );
                double       speed              = getSpeed( sensorID );
                createSPB7ResultString
                (
                    speed,
                    dataPage,
                    deltaBikeSpeedEventTime,
                    deltaWheelRevolutionCount,
                    additionalData1,
                    additionalData2,
                    additionalData3,
                    wheelCircumference,
                    nbMagnets,
                    zeroTime
                );
                setZeroTimeCount( sensorID, zeroTime );
                setSpeed( sensorID, speed );
            }
            appendOutputFooter( curVersion );
        }
    }

    if ( result == UNKNOWN_DEVICE )
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

void antSpeedOnlyProcessing::createSPB7ResultString
(
    double       &speed,
    unsigned int  dataPage,
    unsigned int  deltaBikeSpeedEventTime,
    unsigned int  deltaWheelRevolutionCount,
    unsigned int  additionalData1,
    unsigned int  additionalData2,
    unsigned int  additionalData3,
    double        wheelCircumference,
    unsigned int  numberOfMagnets,
    unsigned int &zeroTime
)
{
    if ( !semiCookedOut )
    {
        speed = computeSpeed
                (
                    speed,
                    deltaWheelRevolutionCount,
                    deltaBikeSpeedEventTime,
                    wheelCircumference,
                    numberOfMagnets,
                    zeroTime,
                    maxZeroTime
                );
    }

    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "delta bike speed event time",  deltaBikeSpeedEventTime );
            appendJSONItem( "delta wheel revolution count", deltaWheelRevolutionCount );
        }
        else
        {
            appendJSONItem( C_SPEED_JSON,          speed,              getValuePrecision() );
            appendJSONItem( "wheel circumference", wheelCircumference, getValuePrecision() );
            appendJSONItem( "number of magnets",   numberOfMagnets );
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
        else
        {
            appendJSONItem( "additional data 1", additionalData1 );
            appendJSONItem( "additional data 1", additionalData2 );
            appendJSONItem( "additional data 3", additionalData3 );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( ( unsigned int ) dataPage );
            appendOutput( deltaBikeSpeedEventTime );
            appendOutput( deltaWheelRevolutionCount );
            if ( ( dataPage & 0x0F ) != 0 )
            {
                appendOutput( additionalData1 );             // deltaOperatingTime (1) / manufacturerID (2) / hwVersion (3)
                if ( ( dataPage & 0x0F ) != 1 )
                {
                    appendOutput( additionalData2 );         // serialNumber (2) / swVersion (3)
                    if ( ( dataPage & 0x0F ) != 2 )
                    {
                        appendOutput( additionalData3 );     // modelNumber (3)
                    }
                }
            }
        }
        else
        {
            appendOutput( speed,              getValuePrecision() );
            appendOutput( wheelCircumference, getValuePrecision() );
            appendOutput( numberOfMagnets );
            appendOutput( dataPage );
            if ( ( dataPage & 0x0F ) != 0 )
            {
                if ( ( dataPage & 0x0F ) == 1 )
                {
                    //                          operatingTimeSeconds
                    appendOutput( 2 * additionalData1 );
                }
                else
                {
                    appendOutput( additionalData1 );                             // manufacturerID (2) / hwVersion (3)
                    appendOutput( additionalData2 );                             // serialNumber (2)   / swVersion (3)
                    if ( ( dataPage & 0x0F ) != 2 )
                    {
                        appendOutput( additionalData3 );                         // modelNumber (3)
                    }
                }
            }
        }
    }
}

