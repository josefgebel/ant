// -------------------------------------------------------------------------------------------------------------------------

// System C++ libraries
#include <iostream>
#include <fstream>
#include <iomanip>

// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "b2t_utils.h"
#include "am_split_string.h"
#include "ant_speed_processing.h"
#include "ant_cadence_processing.h"
#include "ant_spcad_processing.h"

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
antSpcadProcessing::antSpcadProcessing
(
    void
) : antProcessing(),
    antSpeedProcessing(),
    antCadenceProcessing()
{
    reset();
    setMaxZeroTime( C_MAX_ZERO_TIME_SPEED );
}

bool antSpcadProcessing::isSpeedAndCadenceSensor
(
    const std::string &deviceID
)
{
    bool result = startsWith( deviceID, C_SPCAD_DEVICE_HEAD );
    return result;
}

bool antSpcadProcessing::appendSpeedSensor
(
    const std::string &sensorID,
    double             wheelCircumference,
    double             nbMagnets
)
{
    bool result = isSpeedAndCadenceSensor( sensorID );
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
amDeviceType antSpcadProcessing::processSensor
(
    int                deviceType,
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_SPCAD_TYPE )
    {
        result = processSpeedAndCadenceSensor( deviceIDNo, timeStampBuffer, payLoad );
    }

    if ( deviceType == OTHER_DEVICE )
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

amDeviceType antSpcadProcessing::processSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        if ( isSpeedAndCadenceSensor( inputBuffer ) )
        {
            result = processSpeedAndCadenceSensorSemiCooked( inputBuffer );
        }
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

void antSpcadProcessing::reset
(
    void
)
{
    antProcessing::reset();
    antCadenceProcessing::reset();
    antSpeedProcessing::reset();
    operatingTimeTable.clear();
    cadenceTimeTable.clear();
    cadenceCountTable.clear();
}

int antSpcadProcessing::readDeviceFileStream
(
    std::ifstream &deviceFileStream
)
{
    int          errorCode  = 0;
    unsigned int nbWords    = 0;
    std::string  deviceType = "";
    std::string  deviceName = "";

    char line[ C_BUFFER_SIZE ];
    amSplitString words;

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
            // ----------------------------------------
            // All entries must have at least 2 words
            // ----------------------------------------

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
            else if ( ( deviceType == C_SPEED_DEVICE_ID ) && isSpeedAndCadenceSensor( deviceName ) )
            {
                antSpeedProcessing::evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// SPCAD790: Speed and Cadence Sensor
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antSpcadProcessing::processSpeedAndCadenceSensor
(
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    char         auxBuffer[ C_MEDIUM_BUFFER_SIZE ] = { 0 };
    unsigned int bikeCadenceEventTime              = 0;
    unsigned int cumCadenceRevCount                = 0;
    unsigned int bikeSpeedEventTime                = 0;
    unsigned int wheelRevolutionCount              = 0;
    unsigned int deltaSpeedEventTime               = 0;
    unsigned int deltaWheelRevolutionCount         = 0;
    unsigned int deltaCadenceEventTime             = 0;
    unsigned int deltaCrankRevolutionCount         = 0;
    unsigned int rollOver                          = 0;
    bool         rollOverHappened                  = false;
    amDeviceType result                            = OTHER_DEVICE;
    std::string  sensorID                          = std::string( C_SPCAD_DEVICE_HEAD ) + deviceIDNo;

    if ( isRegisteredDevice( sensorID ) )
    {
        // Note: SPCAD790 sensors have no data pages.
        result = SPEED_SENSOR;


        // - - - - - - - - - - - - - - - - - - - - -
        // Cadence Event Time
        bikeCadenceEventTime  = hex( payLoad[ 1 ], payLoad[ 0 ] );
        rollOver              = 65536;  // 256^2
        deltaCadenceEventTime = getDeltaInt( rollOverHappened, sensorID, rollOver, cadenceTimeTable, bikeCadenceEventTime );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "\"Cadence Event Time", payLoad[ 1 ], payLoad[ 0 ], bikeCadenceEventTime );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Cadence Event Time", deltaCadenceEventTime, auxBuffer );
        }


        // - - - - - - - - - - - - - - - - - - - - -
        // Cadence Revolution Count
        cumCadenceRevCount        = hex( payLoad[ 3 ], payLoad[ 2 ] );
        rollOver                  = 65536;  // 256^2
        deltaCrankRevolutionCount = getDeltaInt( rollOverHappened, sensorID, rollOver, cadenceCountTable, cumCadenceRevCount );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Cadence Revolution Count", payLoad[ 3 ], payLoad[ 2 ], cumCadenceRevCount );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Cadence Revolution Count", deltaCrankRevolutionCount, auxBuffer );
        }


        // - - - - - - - - - - - - - - - - - - - - -
        // Speed Event Time
        bikeSpeedEventTime  = hex( payLoad[ 5 ], payLoad[ 4 ] );
        rollOver            = 65536;  // 256^2
        deltaSpeedEventTime = getDeltaInt( rollOverHappened, sensorID, rollOver, eventTimeTable, bikeSpeedEventTime );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Bike Speed Event Time", payLoad[ 5 ], payLoad[ 4 ], bikeSpeedEventTime );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Bike Speed Event Time", deltaSpeedEventTime, auxBuffer );
        }


        // - - - - - - - - - - - - - - - - - - - - -
        // Cumulated Wheel Count
        wheelRevolutionCount      = hex( payLoad[ 7 ], payLoad[ 6 ] );
        rollOver                  = 65536;  // 256^2
        deltaWheelRevolutionCount = getDeltaInt( rollOverHappened, sensorID, rollOver, eventCountTable, wheelRevolutionCount );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Cumulative Wheel Revolution Count", payLoad[ 7 ], payLoad[ 6 ], deltaWheelRevolutionCount );
            *auxBuffer = 0;
            if ( rollOverHappened )
            {
                sprintf( auxBuffer, " (Rollover [%d] occurred)", rollOver );
            }
            appendDiagnosticsLine( "Delta Cumulative Wheel Revolution Count", deltaWheelRevolutionCount, auxBuffer );
        }
    }

    if ( result == SPEED_SENSOR )
    {
        unsigned int zeroTime           = getZeroTimeCount( sensorID );
        unsigned int nbMagnets          = ( unsigned int) round( getNbMagnets( sensorID ) );
        double       wheelCircumference = getWheelCircumference( sensorID );
        double       speed              = getSpeed( sensorID );
        unsigned int cadence            = getCadence( sensorID );
        createOutputHeader( sensorID, timeStampBuffer );
        createSPCADResultString
        (
            speed,
            cadence,
            deltaWheelRevolutionCount,
            deltaSpeedEventTime,
            deltaCrankRevolutionCount,
            deltaCadenceEventTime,
            wheelCircumference,
            nbMagnets,
            zeroTime
        );
        setSpeed( sensorID, speed );
        setCadence( sensorID, cadence );
        setZeroTimeCount( sensorID, zeroTime );
        appendOutputFooter( b2tVersion );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = strToInt( deviceIDNo );
            createUnknownDeviceTypeString( C_SPCAD_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------
//
// processSpeedAndCadenceSensorSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the outBuffer.
// The output string has the form
//     "SPCAD790_<device_ID> <cumulative_revolution_count> <bike_speed_event_time> <cumulative_cadence_revolution_count> <bike_cadence_event_time>"
//
// ---------------------------------------------------------------------------------------------------
amDeviceType antSpcadProcessing::processSpeedAndCadenceSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        std::string   curVersion                = b2tVersion;
        std::string   sensorID;
        std::string   semiCookedString;
        std::string   timeStampBuffer;
        amSplitString words;
        unsigned int  nbWords                   = words.split( inputBuffer );
        unsigned int  counter                   = 0;
        unsigned int  deltaSpeedEventTime       = 0;
        unsigned int  deltaWheelRevolutionCount = 0;
        unsigned int  deltaCadenceEventTime     = 0;
        unsigned int  deltaCrankRevolutionCount = 0;

        if ( nbWords > 6 )
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
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isSpeedAndCadenceSensor( sensorID ) )
            {
                result                    = SPEED_SENSOR;
                deltaSpeedEventTime       = ( unsigned int ) strToInt( words[ counter++ ] );      // 3
                deltaWheelRevolutionCount = ( unsigned int ) strToInt( words[ counter++ ] );      // 4
                deltaCadenceEventTime     = ( unsigned int ) strToInt( words[ counter++ ] );      // 5
                deltaCrankRevolutionCount = ( unsigned int ) strToInt( words[ counter++ ] );      // 6
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Delta Speed Event Time", deltaSpeedEventTime );
                    appendDiagnosticsLine( "Delta Cumulative Wheel Revolution Count", deltaWheelRevolutionCount );
                    appendDiagnosticsLine( "Delta Cadence Event Time", deltaCadenceEventTime );
                    appendDiagnosticsLine( "Delta Cadence Revolution Count", deltaCrankRevolutionCount );
                }
            }
        }

        if ( result == SPEED_SENSOR )
        {
            unsigned int zeroTime           = getZeroTimeCount( sensorID );
            unsigned int nbMagnets          = ( unsigned int) round( getNbMagnets( sensorID ) );
            double       wheelCircumference = getWheelCircumference( sensorID );
            double       speed              = getSpeed( sensorID );
            unsigned int cadence            = getCadence( sensorID );
            createOutputHeader( sensorID, timeStampBuffer );
            createSPCADResultString
            (
                speed,
                cadence,
                deltaWheelRevolutionCount,
                deltaSpeedEventTime,
                deltaCrankRevolutionCount,
                deltaCadenceEventTime,
                wheelCircumference,
                nbMagnets,
                zeroTime
            );

            setSpeed( sensorID, speed );
            setCadence( sensorID, cadence );
            setZeroTimeCount( sensorID, zeroTime );
            appendOutputFooter( curVersion );
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

void antSpcadProcessing::createSPCADResultString
(
    double       &speed,
    unsigned int &cadence,
    unsigned int  deltaWheelRevolutionCount,
    unsigned int  deltaBikeSpeedEventTime,
    unsigned int  deltaCrankRevolutionCount,
    unsigned int  deltaCadenceEventTime,
    double        wheelCircumference,
    unsigned int  numberOfMagnets,
    unsigned int &zeroTime
)
{
    if ( !semiCookedOut )
    {
        cadence = computeCadence( cadence, deltaCrankRevolutionCount, deltaCadenceEventTime );
        speed   = computeSpeed( speed, deltaWheelRevolutionCount, deltaBikeSpeedEventTime, wheelCircumference, numberOfMagnets, zeroTime, maxZeroTime );
    }

    if ( outputAsJSON )
    {
        if ( semiCookedOut )
        {
            appendJSONItem( "delta speed event time",       deltaBikeSpeedEventTime );
            appendJSONItem( "delta wheel revolution count", deltaWheelRevolutionCount );
            appendJSONItem( "delta cadence event time",     deltaCadenceEventTime );
            appendJSONItem( "delta crank revolution count", deltaCrankRevolutionCount );
        }
        else
        {
            appendJSONItem( C_SPEED_JSON,          speed,              getValuePrecision() );
            appendJSONItem( "wheel circumference", wheelCircumference, getValuePrecision() );
            appendJSONItem( "number of magnets",   numberOfMagnets );
            appendJSONItem( C_CADENCE_JSON,        cadence  );
        }
    }
    else
    {
        if ( semiCookedOut )
        {
            appendOutput( deltaBikeSpeedEventTime );
            appendOutput( deltaWheelRevolutionCount );
            appendOutput( deltaCadenceEventTime );
            appendOutput( deltaCrankRevolutionCount );
        }
        else
        {
            appendOutput( speed, getValuePrecision() );
            appendOutput( cadence );
            appendOutput( wheelCircumference, getValuePrecision() );
            appendOutput( numberOfMagnets );
        }
    }
}

