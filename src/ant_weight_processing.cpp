// -------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>

// -------------------------------------------------------------------------------------------------------------------------
// Local Libraries
#include "b2t_utils.h"
#include "ant_weight_processing.h"


// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
//
// WEIGHT SCALE ANT PROCESSOR class methods definition
//
// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------
//
// Constructor
//
// ------------------------------------------------------------------------------------------------------
antWeightProcessing::antWeightProcessing
(
    void
) : antProcessing()
{
}

bool antWeightProcessing::isWeightScaleSensor
(
    const std::string &deviceID
)
{
    bool result = startsWith( deviceID, C_WEIGHT_DEVICE_HEAD );
    return result;
}

bool antWeightProcessing::appendWeightSensor
(
    const std::string &sensorID
)
{
    bool result = isWeightScaleSensor( sensorID );
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
// WEIGHT SCALE
//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
amDeviceType antWeightProcessing::processWeightScaleSensor
(
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    amDeviceType result          = OTHER_DEVICE;
    std::string  sensorID;
    unsigned int dataPage        = 0;
    unsigned int auxInt          = 0;
    unsigned int additionalData1 = 0;
    unsigned int additionalData2 = 0;
    unsigned int additionalData3 = 0;
    unsigned int additionalData4 = 0;
    unsigned int additionalData5 = 0;
    unsigned int additionalData6 = 0;
    bool         commonPage      = false;
    bool         outputPageNo    = true;

    if ( isRegisteredDevice( sensorID ) )
    {
        dataPage = hex( payLoad[ 0 ] );
        if ( diagnostics )
        {
            appendDiagnosticsLine( "Data Page", payLoad[ 0 ], dataPage );
        }

        switch ( dataPage & 0x0F )
        {
            case  1: result          = WEIGHT_SCALE;
                     additionalData1 = hex( payLoad[ 2 ], payLoad[ 1 ] );     // User Profile
                     additionalData2 = hex( payLoad[ 3 ] );                   // Capabilities
                     additionalData3 = hex( payLoad[ 7 ], payLoad[ 6 ] );     // Body Weight
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "User Profile", payLoad[ 2 ], payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Capabilities", payLoad[ 3 ],               additionalData2 );
                         appendDiagnosticsLine( "Body Weight",  payLoad[ 7 ], payLoad[ 6 ], additionalData3 );
                     }
                     break;

            case  2: result          = WEIGHT_SCALE;
                     additionalData1 = hex( payLoad[ 2 ], payLoad[ 1 ] );     // User Profile
                     additionalData2 = hex( payLoad[ 5 ], payLoad[ 4 ] );     // Hydration
                     additionalData3 = hex( payLoad[ 7 ], payLoad[ 6 ] );     // Body Fat
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "User Profile", payLoad[ 2 ], payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Hydration",    payLoad[ 5 ], payLoad[ 4 ], additionalData2 );
                         appendDiagnosticsLine( "Body Fat",     payLoad[ 7 ], payLoad[ 6 ], additionalData3 );
                     }
                     break;

            case  3: result          = WEIGHT_SCALE;
                     additionalData1 = hex( payLoad[ 2 ], payLoad[ 1 ] );     // User Profile
                     additionalData2 = hex( payLoad[ 5 ], payLoad[ 4 ] );     // Active Metabolic Rate
                     additionalData3 = hex( payLoad[ 7 ], payLoad[ 6 ] );     // Basal Metabolic Rate
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "User Profile",          payLoad[ 2 ], payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Active Metabolic Rate", payLoad[ 5 ], payLoad[ 4 ], additionalData2 );
                         appendDiagnosticsLine( "Basal Metabolic Rate",  payLoad[ 7 ], payLoad[ 6 ], additionalData3 );
                     }
                     break;

            case  4: result          = WEIGHT_SCALE;
                     additionalData1 = hex( payLoad[ 2 ], payLoad[ 1 ] );     // User Profile
                     additionalData2 = hex( payLoad[ 6 ], payLoad[ 5 ] );     // Muscle Mass
                     additionalData3 = hex( payLoad[ 7 ] );                   // Bone Mass
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "User Profile", payLoad[ 2 ], payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Muscle Mass",  payLoad[ 6 ], payLoad[ 5 ], additionalData2 );
                         appendDiagnosticsLine( "Bone Mass",    payLoad[ 7 ],               additionalData3 );
                     }
                     break;

            case 58: result          = WEIGHT_SCALE;
                     additionalData1 = hex( payLoad[ 2 ], payLoad[ 1 ] );     //  User Profile
                     additionalData2 = hex( payLoad[ 3 ] );                   //  Capabilities
                     auxInt          = hex( payLoad[ 5 ] );
                     additionalData3 = auxInt >> 7;                           //  Gender
                     additionalData4 = auxInt & ( ( 1 << 7 ) - 1 );           //  Age
                     additionalData5 = hex( payLoad[ 6 ] );                   //  User Height
                     additionalData6 = hex( payLoad[ 7 ] );                   //  Descript Bit
                     if ( diagnostics )
                     {
                         appendDiagnosticsLine( "User Profile",  payLoad[ 2 ], payLoad[ 1 ], additionalData1 );
                         appendDiagnosticsLine( "Capabilities",  payLoad[ 3 ],               additionalData2 );
                         appendDiagnosticsLine( "Gender",        payLoad[ 5 ],               additionalData3, " (highest bit)" );
                         appendDiagnosticsLine( "Age",           payLoad[ 5 ],               additionalData4, " (lowest 3 bits)" );
                         appendDiagnosticsLine( "User Height",   payLoad[ 6 ],               additionalData5 );
                         appendDiagnosticsLine( "Descript Bit",  payLoad[ 7 ],               additionalData6 );
                     }
                     break;

            default: commonPage = true;
                     result     = WEIGHT_SCALE;
                     break;
        }
    }

    if ( result == WEIGHT_SCALE )
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
            createWeightScaleString( dataPage, additionalData1, additionalData2, additionalData3, additionalData4, additionalData5, additionalData6 );
        }
        appendOutputFooter( b2tVersion );
    }

    if ( result == OTHER_DEVICE )
    {
        resetOutBuffer();
        if ( outputUnknown )
        {
            int deviceIDNoAsInt = strToInt( deviceIDNo );
            createUnknownDeviceTypeString( C_WEIGHT_TYPE, deviceIDNoAsInt, timeStampBuffer, payLoad );
        }
    }

    return result;
}

amDeviceType antWeightProcessing::processWeightScaleSensorSemiCooked
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
        unsigned int  additionalData1 = 0;
        unsigned int  additionalData2 = 0;
        unsigned int  additionalData3 = 0;
        unsigned int  additionalData4 = 0;
        unsigned int  additionalData5 = 0;
        unsigned int  additionalData6 = 0;
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
            if ( isRegisteredDevice( sensorID ) && ( semiCookedString == C_SEMI_COOKED_SYMBOL_AS_STRING ) && isWeightScaleSensor( sensorID ) )
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
                    case  1: if ( nbWords > 6 )
                             {
                                 result          = WEIGHT_SCALE;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - User Profile
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );               //  5 - Capabilities
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  6 - Body Weight
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "User Profile", additionalData1 );
                                     appendDiagnosticsLine( "Capabilities", additionalData2 );
                                     appendDiagnosticsLine( "Body Weight",  additionalData3 );
                                 }
                             }
                             break;

                    case  2: if ( nbWords > 6 )
                             {
                                 result          = WEIGHT_SCALE;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - User Profile
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );               //  5 - Hydration
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  6 - Body Fat
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "User Profile", additionalData1 );
                                     appendDiagnosticsLine( "Hydration",    additionalData2 );
                                     appendDiagnosticsLine( "Body Fat",     additionalData3 );
                                 }
                             }
                             break;

                    case  3: if ( nbWords > 6 )
                             {
                                 result          = WEIGHT_SCALE;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - User Profile
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );               //  5 - Active Metabolic Rate
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  6 - Basal Metabolic Rate
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "User Profile",          additionalData1 );
                                     appendDiagnosticsLine( "Active Metabolic Rate", additionalData2 );
                                     appendDiagnosticsLine( "Basal Metabolic Rate",  additionalData3 );
                                 }
                             }
                             break;

                    case  4: if ( nbWords > 6 )
                             {
                                 result          = WEIGHT_SCALE;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - User Profile
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );               //  5 - Muscle Mass
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  6 - Bone Mass
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "User Profile", additionalData1 );
                                     appendDiagnosticsLine( "Muscle Mass",  additionalData2 );
                                     appendDiagnosticsLine( "Bone Mass",    additionalData3 );
                                 }
                             }
                             break;

                    case 58: if ( nbWords > 9 )
                             {
                                 result          = WEIGHT_SCALE;
                                 additionalData1 = ( unsigned int ) strToInt( words[ counter++ ] );               //  4 - User Profile
                                 additionalData2 = ( unsigned int ) strToInt( words[ counter++ ] );               //  5 - Capabilities
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  6 - Gender
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  7 - Age
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  8 - User Height
                                 additionalData3 = ( unsigned int ) strToInt( words[ counter++ ] );               //  9 - Descript Bit
                                 if ( diagnostics )
                                 {
                                     appendDiagnosticsLine( "User Profile",  additionalData1 );
                                     appendDiagnosticsLine( "Capabilities",  additionalData2 );
                                     appendDiagnosticsLine( "Gender",        additionalData3 );
                                     appendDiagnosticsLine( "Age",           additionalData4 );
                                     appendDiagnosticsLine( "User Height",   additionalData5 );
                                     appendDiagnosticsLine( "Descript Bit",  additionalData6 );
                                 }
                             }
                             break;

                    default: commonPage = true;
                             result     = WEIGHT_SCALE;
                             break;
                }
            }
        }

        if ( result == WEIGHT_SCALE )
        {
            if ( nbWords > counter )
            {
                curVersion = words[ counter ];
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
                createWeightScaleString( dataPage, additionalData1, additionalData2, additionalData3, additionalData4, additionalData5, additionalData6 );
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
amDeviceType antWeightProcessing::processSensor
(
    int                deviceType,
    const std::string &deviceIDNo,
    const std::string &timeStampBuffer,
    unsigned char      payLoad[]
)
{
    amDeviceType result = OTHER_DEVICE;

    if ( deviceType == C_WEIGHT_TYPE )
    {
        result = processWeightScaleSensor( deviceIDNo, timeStampBuffer, payLoad );
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

amDeviceType antWeightProcessing::processSensorSemiCooked
(
    const char *inputBuffer
)
{
    amDeviceType result = OTHER_DEVICE;
    if ( ( inputBuffer != NULL ) && ( *inputBuffer != 0 ) )
    {
        if ( isWeightScaleSensor( inputBuffer ) )
        {
            result = processWeightScaleSensorSemiCooked( inputBuffer );
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
bool antWeightProcessing::evaluateDeviceLine
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
        if ( ( deviceType == C_WEIGHT_DEVICE_ID ) && isWeightScaleSensor( deviceName ) )
        {   
            result = appendWeightSensor( deviceName );
        }   
    }   
    return result;
}

int antWeightProcessing::readDeviceFileStream
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
            else if ( ( deviceType == C_WEIGHT_DEVICE_ID ) && isWeightScaleSensor( deviceName ) )
            {
                evaluateDeviceLine( words );
            }
        }
    }

    return errorCode;
}

void antWeightProcessing::getWeightScaleCapabilities
(
    int capabilities
)
{
    if ( outputAsJSON )
    {
        appendJSONItem( "scale user profile selected",           ( capabilities &   1        ) ? true : false );
        appendJSONItem( "scale user profile exchange capable",   ( capabilities & ( 1 << 1 ) ) ? true : false );
        appendJSONItem( "ant fs channel avaliable",              ( capabilities & ( 1 << 2 ) ) ? true : false );
        appendJSONItem( "display user profile exchange capable", ( capabilities & ( 1 << 7 ) ) ? true : false );
    }
    else
    {
        appendOutput( ( capabilities & 1 )          ? "SCALE_USER_PROFILE_SELECTED"           : "SCALE_USER_PROFILE_NOT_SELECTED" );
        appendOutput( ( capabilities & ( 1 << 1 ) ) ? "SCALE_USER_PROFILE_CAPABLE"            : "SCALE_USER_PROFILE_NOT_CAPABLE" );
        appendOutput( ( capabilities & ( 1 << 2 ) ) ? "ANT_FS_CHANNEL_AVALIABLE"              : "ANT_FS_CHANNEL_NOT_AVALIABLE" );
        appendOutput( ( capabilities & ( 1 << 7 ) ) ? "DISPLAY_USER_PROFILE_EXHCANGE_CAPABLE" : "DISPLAY_USER_PROFILE_EXHCANGE_NOT_CAPABLE" );
    }
}

void antWeightProcessing::createWeightScaleString
(
    unsigned int dataPage,
    unsigned int additionalData1,
    unsigned int additionalData2,
    unsigned int additionalData3,
    unsigned int additionalData4,
    unsigned int additionalData5,
    unsigned int additionalData6
)
{
    if ( outputAsJSON )
    {
        appendJSONItem( "data page", dataPage );
        if ( semiCookedOut || ( additionalData1 != 0xFFFF ) )
        {
            appendJSONItem( "user profile", additionalData1 );
        }
        else
        {
            appendJSONItem( "user profile", C_INVALID_JSON );
        }
    }
    if ( semiCookedOut )
    {
        if ( outputAsJSON )
        {
            if ( ( dataPage == 1 ) || ( dataPage == 2 ) || ( dataPage == 3 ) || ( dataPage == 4 ) )
            {
                if ( dataPage == 1 )
                {
                    appendJSONItem( "capabilities", additionalData2 );
                    appendJSONItem( "body weight",  additionalData3 );
                }
                else if ( dataPage == 2 )
                {
                    appendJSONItem( "hydration", additionalData2 );
                    appendJSONItem( "body fat",  additionalData3 );
                }
                else if ( dataPage == 3 )
                {
                    appendJSONItem( "active metabolic rate", additionalData2 );
                    appendJSONItem( "basal metabolic rate",  additionalData3 );
                }
                else if ( dataPage == 4 )
                {
                    appendJSONItem( "muscle mass", additionalData2 );
                    appendJSONItem( "bone mass",   additionalData3 );
                }
            }
            else if ( dataPage == 58 )
            {
                appendJSONItem( "capabilities", additionalData2 );
                appendJSONItem( "gender",       additionalData3 );
                appendJSONItem( "age",          additionalData4 );
                appendJSONItem( "user height",  additionalData5 );
                appendJSONItem( "descript bit", additionalData6 );
            }
        }
        else
        {
            appendOutput( dataPage );
            appendOutput( additionalData1 );
            appendOutput( additionalData2 );
            appendOutput( additionalData3 );
            if ( dataPage == 58 )
            {
                appendOutput( additionalData4 );
                appendOutput( additionalData5 );
                appendOutput( additionalData6 );
            }
        }
    }
    else
    {
        if ( outputAsJSON )
        {
            if ( ( dataPage == 1 ) || ( dataPage == 58 ) )
            {
                getWeightScaleCapabilities( additionalData2 );
                if ( dataPage == 1 )
                {
                    if ( ( additionalData3 == 0xFFFF ) || ( additionalData3 == 0xFFFE ) )
                    {
                        appendJSONItem( "body weight", ( additionalData3 == 0xFFFF ) ? C_INVALID_JSON : "computing" );
                    }
                    else
                    {
                        double bodyWeight = ( double ) additionalData3 / 100.0;
                        appendJSONItem( "body weight", bodyWeight, 2 );
                    }
                }
                else if ( dataPage == 58 )
                {
                    if ( ( additionalData3 == 0 ) && ( additionalData4 == 0 ) )
                    {
                        appendJSONItem( "gender", "not set" );
                        appendJSONItem( "age",    "not set" );
                    }
                    else
                    {
                        appendJSONItem( "gender", ( additionalData2 == 0 ) ? "female" : "male" );
                        appendJSONItem( "age",    additionalData3 );
                    }

                    if ( additionalData5 == 0 )
                    {
                        appendJSONItem( "height", "not set" );
                    }
                    else
                    {
                        appendJSONItem( "height", additionalData5 );
                    }

                    bool isLifeTimeAthlete = ( ( additionalData6 & ( 1 << 7 ) ) != 0 );
                    appendJSONItem( "athlete type", isLifeTimeAthlete ? "lifetime" : "standard" );

                    unsigned int activityClass = ( additionalData6 >> 6 );
                    appendJSONItem( "activity class", activityClass );
                }
            }
            else if ( dataPage == 2 )
            {
                double hydrationPct = ( double ) additionalData2 / 100.0;
                if ( ( additionalData2 == 0xFFFF ) || ( additionalData2 == 0xFFFE ) )
                {
                    appendJSONItem( "hydration percentage", ( additionalData2 == 0xFFFF ) ? C_INVALID_JSON : "computing" );
                }
                else
                {
                    appendJSONItem( "hydration percentage", hydrationPct, 2 );
                }

                double bodyFatPct   = ( double ) additionalData3 / 100.0;
                if ( ( additionalData3 == 0xFFFF ) || ( additionalData3 == 0xFFFE ) )
                {
                    appendJSONItem( "body fat percentage", ( additionalData3 == 0xFFFF ) ? C_INVALID_JSON : "computing" );
                }
                else
                {
                    appendJSONItem( "body fat percentage", bodyFatPct, 2 );
                }
            }
            else if ( dataPage == 3 )
            {
                double activeMetabolicRate = ( double ) additionalData2 / 100.0;
                if ( ( additionalData2 == 0xFFFF ) || ( additionalData2 == 0xFFFE ) )
                {
                    appendJSONItem( "active metabolic rate", ( additionalData2 == 0xFFFF ) ? C_INVALID_JSON : "computing" );
                }
                else
                {
                    appendJSONItem( "active metabolic rate", activeMetabolicRate, 2 );
                }
    
                double basalMetabolicRate = ( double ) additionalData3 / 100.0;
                if ( ( additionalData3 == 0xFFFF ) || ( additionalData3 == 0xFFFE ) )
                {
                    appendJSONItem( "basal metabolic rate", ( additionalData3 == 0xFFFF ) ? C_INVALID_JSON : "computing" );
                }
                else
                {
                    appendJSONItem( "basal metabolic rate", basalMetabolicRate, 2 );
                }
            }
            else if ( dataPage == 4 )
            {
                double muscleMass = ( double ) additionalData2 / 100.0;
                if ( ( additionalData2 == 0xFFFF ) || ( additionalData2 == 0xFFFE ) )
                {
                    appendJSONItem( "muscle mass", ( additionalData2 == 0xFFFF ) ? C_INVALID_JSON : "computing" );
                }
                else
                {
                    appendJSONItem( "muscle mass", muscleMass, 2 );
                }
    
                double boneMass = ( double ) additionalData3 /  10.0;
                if ( ( additionalData3 == 0xFF ) || ( additionalData3 == 0xFE ) )
                {
                    appendJSONItem( "bone mass", ( additionalData3 == 0xFFFF ) ? C_INVALID_JSON : "computing" );
                }
                else
                {
                    appendJSONItem( "bone mass", boneMass, 1 );
                }
            }
        }
        else
        {
            appendOutput( dataPage );
            appendOutput( "USER_PROFILE" );
            if ( semiCookedOut || ( additionalData1 != 0xFFFF ) )
            {
                appendOutput( additionalData1 );
            }
            else
            {
                appendOutput( C_INVALID );
            }
            if ( ( dataPage == 1 ) || ( dataPage == 58 ) )
            {
                getWeightScaleCapabilities( additionalData2 );
                if ( dataPage == 1 )
                {
                    appendOutput( "BODY_WEIGHT" );
                    if ( ( additionalData3 == 0xFFFF ) || ( additionalData3 == 0xFFFE ) )
                    {
                        appendOutput( ( additionalData3 == 0xFFFF ) ? C_INVALID : "COMPUTING" );
                    }
                    else
                    {
                        double bodyWeight = ( double ) additionalData3 / 100.0;
                        appendOutput( bodyWeight, 2 );
                    }
                }
                else if ( dataPage == 58 )
                {
                    appendOutput( "GENDER" );
                    if ( ( additionalData3 == 0 ) && ( additionalData4 == 0 ) )
                    {
                        appendOutput( "NOT_SET" );
                        appendOutput( "AGE" );
                        appendOutput( "NOT_SET" );
                    }
                    else
                    {
                        appendOutput( ( additionalData2 == 0 ) ? "F" : "M" );
                        appendOutput( "AGE" );
                        appendOutput( additionalData3 );
                    }

                    appendOutput( "HEIGHT" );
                    if ( additionalData5 == 0 )
                    {
                        appendOutput( "NOT_SET" );
                    }
                    else
                    {
                        appendOutput( additionalData5, "cm" );
                    }

                    bool isLifeTimeAthlete = ( ( additionalData6 & ( 1 << 7 ) ) != 0 );
                    appendOutput( "ATHLETE_TYPE" );
                    appendOutput( isLifeTimeAthlete ? "LIFETIME" : "STANDARD" );

                    unsigned int activityClass = ( additionalData6 >> 6 );
                    appendOutput( "ACTIVITY_CLASS" );
                    appendOutput( activityClass );
                }
            }
            else if ( dataPage == 2 )
            {
                double hydrationPct = ( double ) additionalData2 / 100.0;
                appendOutput( "HYDRATION" );
                if ( ( additionalData2 == 0xFFFF ) || ( additionalData2 == 0xFFFE ) )
                {
                    appendOutput( ( additionalData2 == 0xFFFF ) ? C_INVALID : "COMPUTING" );
                }
                else
                {
                    appendOutput( hydrationPct, 2, "\%" );
                }

                double bodyFatPct   = ( double ) additionalData3 / 100.0;
                appendOutput( "BODY_FAT" );
                if ( ( additionalData3 == 0xFFFF ) || ( additionalData3 == 0xFFFE ) )
                {
                    appendOutput( ( additionalData3 == 0xFFFF ) ? C_INVALID : "COMPUTING" );
                }
                else
                {
                    appendOutput( bodyFatPct, 2, "\%" );
                }
            }
            else if ( dataPage == 3 )
            {
                double activeMetabolicRate = ( double ) additionalData2 / 100.0;
                appendOutput( "ACTIVE_METABOLIC_RATE" );
                if ( ( additionalData2 == 0xFFFF ) || ( additionalData2 == 0xFFFE ) )
                {
                    appendOutput( ( additionalData2 == 0xFFFF ) ? C_INVALID : "COMPUTING" );
                }
                else
                {
                    appendOutput( activeMetabolicRate, 2, "\%" );
                }

                double basalMetabolicRate  = ( double ) additionalData3 / 100.0;
                appendOutput( "BASAL_METABOLIC_RATE" );
                if ( ( additionalData3 == 0xFFFF ) || ( additionalData3 == 0xFFFE ) )
                {
                    appendOutput( ( additionalData3 == 0xFFFF ) ? C_INVALID : "COMPUTING" );
                }
                else
                {
                    appendOutput( basalMetabolicRate, 2, "\%" );
                }
            }
            else if ( dataPage == 4 )
            {
                double muscleMass = ( double ) additionalData2 / 100.0;
                appendOutput( "MUSCLE_MASS" );
                if ( ( additionalData2 == 0xFFFF ) || ( additionalData2 == 0xFFFE ) )
                {
                    appendOutput( ( additionalData2 == 0xFFFF ) ? C_INVALID : "COMPUTING" );
                }
                else
                {
                    appendOutput( muscleMass, 2, "kg" );
                }

                double boneMass = ( double ) additionalData3 /  10.0;
                appendOutput( "BONE_MASS" );
                if ( ( additionalData3 == 0xFF ) || ( additionalData3 == 0xFE ) )
                {
                    appendOutput( ( additionalData3 == 0xFFFF ) ? C_INVALID : "COMPUTING" );
                }
                else
                {
                    appendOutput( boneMass, 2, "kg" );
                }
            }
        }
    }
}

