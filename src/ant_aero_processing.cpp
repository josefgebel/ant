// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "am_split_string.h"
#include "ant_aero_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// AERO ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antAeroProcessing::antAeroProcessing
(
    void
) : antProcessing()
{
    currentDeviceType = "AERO";
    rho               = rhoDefault;
    reset();
}

bool antAeroProcessing::isAeroSensor
(
    const amString &deviceID
)
{
    bool result = deviceID.startsWith( C_AERO_DEVICE_HEAD );
    return result;
}

bool antAeroProcessing::appendAeroSensor
(
    const amString &sensorID,
    double          calibrationRho,
    double          airSpeedMultiplier
)
{
    bool result = isAeroSensor( sensorID );
    if ( result )
    {
        if ( !isRegisteredDevice( sensorID ) )
        {
            registerDevice( sensorID );
        }

        if ( rhoCalibrationTable.count( sensorID ) == 0 )
        {
            rhoCalibrationTable.insert( std::pair<amString, double>( sensorID, 0 ) );
        }
        rhoCalibrationTable[ sensorID ] = calibrationRho;

        if ( airSpeedMultiplierTable.count( sensorID ) == 0 )
        {
            airSpeedMultiplierTable.insert( std::pair<amString, double>( sensorID, 0 ) );
        }
        airSpeedMultiplierTable[ sensorID ] = airSpeedMultiplier;
    }
    return result;
}

double antAeroProcessing::getCalibrationRho
(
    const amString &sensorID
)
{
    if ( rhoCalibrationTable.count( sensorID ) == 0 )
    {
        rhoCalibrationTable.insert( std::pair<amString, double>( sensorID, calibrationRhoDefault ) );
    }
    double calibrationRho = rhoCalibrationTable[ sensorID ];
    return calibrationRho;
}

double antAeroProcessing::getAirSpeedMultiplier
(
    const amString &sensorID
)
{
    if ( airSpeedMultiplierTable.count( sensorID ) == 0 )
    {
        airSpeedMultiplierTable.insert( std::pair<amString, double>( sensorID, airSpeedMultiplierDefault ) );
    }
    double airSpeedMultiplier = airSpeedMultiplierTable[ sensorID ];
    return airSpeedMultiplier;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//
// AERO
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antAeroProcessing::processAeroSensor
(
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result      = OTHER_DEVICE;
    amString  sensorID       = amString( C_AERO_DEVICE_HEAD ) + deviceIDNo;
    unsigned int dataPage    = 0;
    unsigned int airSpeedRaw = 0;
    unsigned int yawAngleRaw = 0;

    if ( isRegisteredDevice( sensorID ) )
    {
        result      = AERO_SENSOR;
        dataPage    = hex2Int( payLoad[ 0 ] );
        yawAngleRaw = hex2Int( payLoad[ 5 ], payLoad[ 4 ] );
        airSpeedRaw = hex2Int( payLoad[ 7 ], payLoad[ 6 ] );

        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
            appendDiagnosticsLine( "Raw Yaw", payLoad[ 5 ], payLoad[ 4 ], yawAngleRaw );
            appendDiagnosticsLine( "Raw Air Speed", payLoad[ 7 ], payLoad[ 6 ], airSpeedRaw );
        }
    }

    if ( result == AERO_SENSOR )
    {
        double calibrationRho     = getCalibrationRho( sensorID );
        double airSpeedMultiplier = getAirSpeedMultiplier( sensorID );

        createOutputHeader( sensorID, timeStampBuffer );
        createAEROResultString( airSpeedRaw, yawAngleRaw, calibrationRho, airSpeedMultiplier, rho );
        appendOutputFooter( b2tVersion );
    }

    if ( result == OTHER_DEVICE )
    {
        bool commonPage   = false;
        bool outputPageNo = true;
        createOutputHeader( sensorID, timeStampBuffer );
        if ( commonPage )
        {
            commonPage = processCommonPages( sensorID, payLoad, outputPageNo );
            if ( commonPage )
            {
                appendOutputFooter( b2tVersion );
            }
            else
            {
                result = OTHER_DEVICE;
            }
        }
        else
        {
            resetOutBuffer();
            if ( outputUnknown )
            {
                int deviceIDNoAsInt = deviceIDNo.toInt();
                createUnknownDeviceTypeString( C_AERO_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
            }
        }
    }

    return result;
}

//---------------------------------------------------------------------------------------------------
//
// processAeroSensorSemiCooked
//
// Convert the raw ant data into semi-cooked text data and put the result string into the resultBuffer.
// The output string has the form
//     "AERO_<device_ID> <air_speed_ad> <yaw_ad>"
//
//---------------------------------------------------------------------------------------------------
amDeviceType antAeroProcessing::processAeroSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( !inputBuffer.empty() )
    {
        amSplitString words;
        unsigned int  nbWords            = words.split( inputBuffer );
        unsigned int  airSpeedRaw        = 0;
        unsigned int  yawAngleRaw        = 0;
        unsigned int  counter            = 0;
        unsigned int  dataPage           = 0;
        unsigned int  startCounter       = 0;
        bool          commonPage         = false;
        bool          outputPageNo       = true;
        amString   curVersion         = b2tVersion;
        amString   sensorID;
        amString   timeStampBuffer;
        amString   semiCookedString;

        if ( nbWords > 4 )
        {
            sensorID         = words[ counter++ ];                // 0
            timeStampBuffer  = words[ counter++ ];                // 1
            semiCookedString = words[ counter++ ];                // 2
            if ( diagnostics )
            {
                appendDiagnosticsLine( "SensorID",   sensorID );
                appendDiagnosticsLine( "Timestamp",  timeStampBuffer );
                appendDiagnosticsLine( "SemiCooked", semiCookedString );
            }
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isAeroSensor( sensorID ) )
            {
                startCounter = counter;
                result       = AERO_SENSOR;
                airSpeedRaw  = ( unsigned int ) words[ counter++ ].toInt();          // 3
                yawAngleRaw  = ( unsigned int ) words[ counter++ ].toInt();          // 4
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Raw Air Speed", airSpeedRaw );
                    appendDiagnosticsLine( "Raw Yaw Angle (signed)", yawAngleRaw );
                }
            }
        }

        if ( result == AERO_SENSOR )
        {
            if ( nbWords > counter )
            {
                curVersion = words.back();
                if ( diagnostics )
                {
                    appendDiagnosticsLine( "Version", curVersion );
                }
            }
            double calibrationRho     = getCalibrationRho( sensorID );
            double airSpeedMultiplier = getAirSpeedMultiplier( sensorID );

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
                createAEROResultString( airSpeedRaw, yawAngleRaw, calibrationRho, airSpeedMultiplier, rho );
            }
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

//---------------------------------------------------------------------------------------------------
//
// processSensor
//
// Depending on the device type call a subroutine to process the payload data and put
// the result string into the resultBuffer.
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
amDeviceType antAeroProcessing::processSensor
(
    int             deviceType,
    const amString &deviceIDNo,
    const amString &timeStampBuffer,
    BYTE            payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_AERO_TYPE )
    {
        result = processAeroSensor( deviceIDNo, timeStampBuffer, payLoad );
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

amDeviceType antAeroProcessing::processSensorSemiCooked
(
    const amString &inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( !inputBuffer.empty() )
    {
        if ( isAeroSensor( inputBuffer ) )
        {
            result = processAeroSensorSemiCooked( inputBuffer );
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

void antAeroProcessing::reset
(
    void
)
{
    antProcessing::reset();

    currentDeviceType = "AERO";
    airSpeedMultiplierTable.clear();
    rhoCalibrationTable.clear();

    resetRhoDefault();
    resetCalibrationRhoDefault();
    resetAirSpeedMultiplierDefault();
}

void antAeroProcessing::setRho
(
    double value
)
{
    rho = value;
}

bool antAeroProcessing::setCalibrationRho
(
    const amString &sensorID,
    double             value
)
{
    bool result = ( rhoCalibrationTable.count( sensorID ) > 0 );
    if ( result )
    {
        rhoCalibrationTable[ sensorID ] = value;
    }
    return result;
}

bool antAeroProcessing::setAirSpeedMultiplier
(
    const amString &sensorID,
    double             value
)
{
    bool result = ( airSpeedMultiplierTable.count( sensorID ) > 0 );
    if ( result )
    {
        airSpeedMultiplierTable[ sensorID ] = value;
    }
    return result;
}

bool antAeroProcessing::evaluateDeviceLine
(
    const amSplitString &words
)
{
    bool         result  = false;
    unsigned int nbWords = words.size();
    if ( nbWords > 1 )
    {
        amString deviceType = words[ 0 ];
        amString deviceName = words[ 1 ];
        double      dArg1      = 0;
        double      dArg2      = 0;
        if ( ( deviceType == C_AERO_DEVICE_ID ) && isAeroSensor( deviceName ) )
        {
            dArg1 = C_CALIBRATION_RHO_DEFAULT;
            dArg2 = C_AIR_SPEED_MULTIPLIER_DEFAULT;
            if ( nbWords > 2 )
            {
                dArg1 = words[ 2 ].toDouble();
                if ( nbWords > 3 )
                {
                    dArg2 = words[ 3 ].toDouble();
                }
            }
            appendAeroSensor( deviceName, dArg1, dArg2 );
        }
        else if ( deviceType == C_RHO_ID )
        {
            dArg1 = words[ 1 ].toDouble();
            if ( dArg1 > 0 )
            {
                setRho( dArg1 );
            }
        }
    }
    return result;
}

int antAeroProcessing::readDeviceFileStream
(
    std::ifstream &deviceFileStream
)
{
    amString  deviceType = "";
    amString  deviceName = "";
    unsigned int nbWords    = 0;

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
            else if ( ( deviceType == C_AERO_DEVICE_ID ) || ( deviceType == C_RHO_ID ) )
            {
                evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
}

void antAeroProcessing::createAEROResultString
(
    unsigned int airSpeedRaw,
    unsigned int yawAngleRaw,
    double       calibrationRho,
    double       airSpeedMultiplier,
    double       currentRho
)
{
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            appendJSONItem( "raw air speed", airSpeedRaw );
            appendJSONItem( "raw yaw angle", yawAngleRaw );
        }
        else
        {
            appendOutput( airSpeedRaw );
            appendOutput( yawAngleRaw );
        }
    }
    else
    {
        double airSpeed = computeAirSpeed( airSpeedRaw, calibrationRho, airSpeedMultiplier, currentRho );
        double yawAngle = computeYawAngle( yawAngleRaw );
        if ( outputAsJSON )
        {
            appendJSONItem( "air speed",               airSpeed,           getValuePrecision() );
            appendJSONItem( "yaw angle",               yawAngle,           getValuePrecision() );
            appendJSONItem( "calibration air density", calibrationRho,     getValuePrecision() );
            appendJSONItem( "air speed multiplier",    airSpeedMultiplier, getValuePrecision() );
            appendJSONItem( "current air density",     currentRho,         getValuePrecision() );
        }
        else
        {
            appendOutput( airSpeed,           getValuePrecision() );
            appendOutput( yawAngle,           getValuePrecision() );
            appendOutput( calibrationRho,     getValuePrecision() );
            appendOutput( airSpeedMultiplier, getValuePrecision() );
            appendOutput( currentRho,         getValuePrecision() );
        }
    }
}

double antAeroProcessing::computeYawAngle
(
    unsigned int yawAngleUnsignedRaw
)
{
    int yawAngleSignedRaw = 0;
    if ( yawAngleUnsignedRaw > ( 1 << 15 ) )
    {
        // Negative value
        yawAngleSignedRaw  = ( int ) ( yawAngleUnsignedRaw & 0xFFFF );
        yawAngleSignedRaw -= ( 1 << 16 );
        ++yawAngleSignedRaw;
    }
    else
    {
        yawAngleSignedRaw  = ( int ) yawAngleUnsignedRaw;
    }
    double yawAngle = computeYawAngle( yawAngleSignedRaw );
    return yawAngle;
}

double antAeroProcessing::computeYawAngle
(
    int yawAngleSignedRaw
)
{
    double yawAngle  = ( double ) yawAngleSignedRaw;
    yawAngle        /= 200.0;
    return yawAngle;
}

double antAeroProcessing::computeAirSpeed
(
    unsigned int airSpeedUnsignedRaw,
    double       calibrationRho,
    double       airSpeedMultiplier,
    double       currentRho
)
{
    int airSpeedSignedRaw = 0;
    if ( airSpeedUnsignedRaw > ( 1 << 15 ) )
    {
        // Negative value
        airSpeedSignedRaw  = ( int ) ( airSpeedUnsignedRaw & 0xFFFF );
        airSpeedSignedRaw -= ( 1 << 16 );
        ++airSpeedSignedRaw;
    }
    else
    {
        airSpeedSignedRaw  = ( int ) airSpeedUnsignedRaw;
    }
    double airSpeed = computeAirSpeed( airSpeedSignedRaw, calibrationRho, airSpeedMultiplier, currentRho );
    return airSpeed;
}

double antAeroProcessing::computeAirSpeed
(
    int    airSpeedSignedRaw,
    double calibrationRho,
    double airSpeedMultiplier,
    double currentRho
)
{
    double airSpeed  = ( double ) airSpeedSignedRaw;
    airSpeed        *= sqrt( calibrationRho / currentRho );
    airSpeed        *= airSpeedMultiplier;
    airSpeed        /= 100.0;
    return airSpeed;
}

double antAeroProcessing::getCorrectionFactor
(
    const amString &sensorID
)
{
    double correctionFactor = 1.0;

    if ( rhoCalibrationTable.count( sensorID ) > 0 )
    {
        double calibrationRho = getCalibrationRho( sensorID );
        correctionFactor      = sqrt( calibrationRho / rho );
    }

    if ( airSpeedMultiplierTable.count( sensorID ) > 0 )
    {
        double airSpeedMultiplier  = getAirSpeedMultiplier( sensorID );
        correctionFactor          *= airSpeedMultiplier;
    }

    return correctionFactor;
}

