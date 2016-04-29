#ifndef __ANT_CONSTANTS_H__
#define __ANT_CONSTANTS_H__

#include <math.h>
#include <am_string.h>

#define C_VERSION        "1.0.0"
#define C_AUTO_INTERFACE "auto"

#define IS_WHITE_CHAR( _CCC_ ) ( ( ( _CCC_ ) == ' ' ) || ( ( _CCC_ ) == '\t' ) )
#define NEGATE_BINARY_INT( _NNN_, _PPP_ ) ( ( ( _NNN_ ) >= ( 1 << ( (_PPP_ ) - 1 ) ) ) ?  ( ( _NNN_ ) - ( 1 << ( _PPP_ ) ) ) : ( _NNN_ ) );
#define HEX_DIGIT_2_INT( _HHH_ ) ( ( ( ( _HHH_ ) >= '0' ) && ( ( _HHH_ ) <= '9' ) ) ? ( ( _HHH_ ) - '0' ) : \
                                   ( ( ( toupper( _HHH_ ) >= 'A' ) && ( toupper( _HHH_ ) <= 'F' ) ) ? ( toupper( _HHH_ ) - 'A' + 10 ) : 0 ) )

const char BUILD_NUMBER[] = C_VERSION;
const char C_COPYRIGHT[]  = "Copyright (c) Alphamantis Technologies Inc. 2016";

// ANT+ Device types
#define C_POWER_TYPE           11
#define C_AERO_TYPE            12
#define C_MSSDM_TYPE           15
#define C_AUDIO_TYPE           16
#define C_BLDPR_TYPE           18
#define C_ENV_TYPE             25
#define C_WEIGHT_TYPE         119
#define C_HRM_TYPE            120
#define C_SPCAD_TYPE          121
#define C_CAD_TYPE            122
#define C_SPEED_TYPE          123
#define C_SBSDM_TYPE          124

#define C_BUFFER_SIZE         ( 1 << 10 )
#define C_MEDIUM_BUFFER_SIZE  ( 1 <<  9 )
#define C_SMALL_BUFFER_SIZE   ( 1 <<  8 )
#define C_TINY_BUFFER_SIZE    ( 1 <<  6 )
#define C_ANT_PAYLOAD_LENGTH  8

enum amOperatingSystem
{
    MAC_OSX,
    LINUX,
    CYGWIN,
    UNKNOWN
};

enum amDeviceType
{
    AERO_SENSOR,             //  0
    SPEED_SENSOR,            //  1
    CADENCE_SENSOR,          //  2
    POWER_METER,             //  3
    HEART_RATE_METER,        //  4
    BRIDGE_ID,               //  5
    RHO,                     //  6
    ENVIRONMENT_SENSOR,      //  7
    MULTI_SPORT_SD_SENSOR,   //  8
    STRIDE_BASED_SD_SENSOR,  //  9
    BLOOD_PRESSURE_SENSOR,   // 10
    WEIGHT_SCALE,            // 11
    AUDIO_CONTROL,           // 12
    PACKET_SAVER_MODE,       // 13
    OTHER_DEVICE,            // 14
    UNKNOWN_DEVICE,          // 15
    DEVICE_ERROR             // 16
};


const int E_READ_TIMEOUT_C        =    35;

const int E_BAD_PARAMETER_VALUE   = 10001;

const int E_READ_ERROR            = 90001;
const int E_READ_FILE_NOT_OPEN    = 90002;
const int E_END_OF_FILE           = 90099;


const int E_MC_NO_INTERFACE       = 91001;
const int E_MC_NO_IP_ADDRESS      = 91002;
const int E_MC_NO_PORT_NUMBER     = 91003;
const int E_NO_IP_ADDRESS_IF      = 91004;
const int E_READ_TIMEOUT          = 91005;
const int E_MC_WRITE_FAIL         = 90006;
const int E_SOCKET_CREATE_FAIL    = 90101;
const int E_SOCKET_SET_OPT_FAIL   = 90102;
const int E_SOCKET_BIND_FAIL      = 90103;
const int E_LOOP_BACK_IP_ADDRESS  = 90104;
const int E_EMPTY_MESSAGE         = 91099;

const int E_BAD_OPTION            = 99998;
const int E_UNKNOWN_OPTION        = 99999;

const bool C_OUTPUT_UNKNOWN                                 = false;
const bool C_OUTPUT_BRIDGE                                  = true;
const bool C_DEFAULT_WRITE_STDOUT                           = true;
const bool C_DEFAULT_OUTPUT_UNKNOWN                         = false;
const bool C_DEFAULT_OUTPUT_BRIDGE                          = false;
const bool C_DEFAULT_USE_LOCAL_TIME                         = false;
const bool C_DEFAULT_SEMI_COOKED_IN                         = false;
const bool C_DEFAULT_SEMI_COOKED_OUT                        = false;
const bool C_DEFAULT_OUTPUT_AS_JSON                         = false;
const bool C_DEFAULT_OUTPUT_RAW                             = false;
const bool C_DEFAULT_DIAGNOSTICS                            = false;
const bool C_DEFAULT_ONLY_REGISTERED_DEVICES                = false;

const char C_UNSUPPORTED_DATA_PAGE[]                        = "UNSUPPORTED_DATA_PAGE";
const char C_UNSUPPORTED_DATA_PAGE_JSON[]                   = "unsupported data page";
const char C_N_A_JSON[]                                     = "n/a";
const char C_ON_JSON[]                                      = "on";
const char C_OFF_JSON[]                                     = "off";
const char C_INVALID_JSON[]                                 = "invalid";
const char C_UNUSED_JSON[]                                  = "unused";
const char C_UNKNOWN_JSON[]                                 = "unknown";
const char C_UNDEFINED_JSON[]                               = "undefined";
const char C_TRUE_JSON[]                                    = "true";
const char C_NONE_JSON[]                                    = "none";
const char C_FALSE_JSON[]                                   = "false";
const char C_SENSOR_JSON[]                                  = "sensor id";
const char C_TIMESTAMP_JSON[]                               = "timestamp";
const char C_SEMI_COOKED_JSON[]                             = "semi-cooked";
const char C_B2TXT_VERSION_JSON[]                           = "bridge2txt version";
const char C_HARDWARE_REVISION_JSON[]                       = "hardware revision";
const char C_SOFTWARE_REVISION_JSON[]                       = "software revision";
const char C_MODEL_NUMBER_JSON[]                            = "model number";
const char C_SERIAL_NUMBER_JSON[]                           = "serial number";
const char C_MANUFACTURER_JSON[]                            = "manufacturer id";
const char C_SPEED_JSON[]                                   = "speed";
const char C_POWER_JSON[]                                   = "power";
const char C_HEART_RATE_JSON[]                              = "heart rate";
const char C_TORQUE_JSON[]                                  = "torque";
const char C_CADENCE_JSON[]                                 = "cadence";
const char C_AIR_SPEED_JSON[]                               = "air speed";
const char C_YAW_ANGLE_JSON[]                               = "yaw angle";
const char C_CIRCUMFERENCE_JSON[]                           = "wheel circumference";
const char C_GEAR_RATIO_JSON[]                              = "gear ratio";
const char C_NB_MAGNETS_JSON[]                              = "number of magnets";
const char C_AIR_DENSITY_JSON[]                             = "current air density";
const char C_CALIB_AIR_DENSITY_JSON[]                       = "calibration air density";
const char C_AIR_SPEED_MULT_JSON[]                          = "air speed multiplier";
const char C_SLOPE_10_JSON[]                                = "slope times ten";
const char C_SLOPE_JSON[]                                   = "slope";
const char C_OFFSET_JSON[]                                  = "zero offset";
const char C_USER_PROFILE_JSON[]                            = "user profile";
const char C_DATA_PAGE_JSON[]                               = "data page";
const char C_OPERATING_TIME_JSON[]                          = "operating time";
const char C_TOTAL_TIME_JSON[]                              = "total time";
const char C_JSON_INDENT[]                                  = "   ";
const char C_JSON_OPEN[]                                    = "{";
const char C_JSON_CLOSE[]                                   = "}";
const char C_NO_EVENT[]                                     = "NO_EVENT";
const char C_ZERO_OFFSET_UPDATE[]                           = "ZERO_OFFSET_UPDATE";
const char C_SLOPE_UPDATE[]                                 = "SLOPE_UPDATE";
const char C_SERIAL_NUMBER_UPDATE[]                         = "SERIAL_NUMBER_UPDATE";
const char C_AUTO_ZERO_SUPPORT[]                            = "AUTO_ZERO_SUPPORT";
const char C_MANUAL_ZERO_REQUEST[]                          = "MANUAL_ZERO_REQUEST";
const char C_AUTO_ZERO_CONFIGURATION[]                      = "AUTO_ZERO_CONFIGURATION";
const char C_MANUAL_ZERO_SUCCESS[]                          = "MANUAL_ZEZO_SUCCESSFUL";
const char C_MANUAL_ZERO_FAIL[]                             = "MANUAL_ZEZO_FAILED";
const char C_CUSTOM_CALIBRATION_PARAMS_REQUEST[]            = "CUSTOM_CALIBRATION_PARAMS_REQUEST";
const char C_CUSTOM_CALIBRATION_PARAMS_RESPONSE[]           = "CUSTOM CALIBRATION PARAMS RESPONSE";
const char C_SET_CUSTOM_CALIBRATION_PARAMS_SUCCESS[]        = "SET_CUSTOM_CALIBRATION_PARAMS_SUCCESS";
const char C_SET_CUSTOM_CALIBRATION_PARAMS[]                = "SET_CUSTOM_CALIBRATION_PARAMS";
const char C_ZERO_OFFSET[]                                  = "ZERO_OFFSET";
const char C_SLOPE[]                                        = "SLOPE";
const char C_SERIAL_NUMBER[]                                = "SERIAL_NO";
const char C_ON[]                                           = "ON";
const char C_OFF[]                                          = "OFF";
const char C_UNKNOWN[]                                      = "UNKNOWN";
const char C_SUCCESS[]                                      = "SUCCESS";
const char C_FAIL[]                                         = "FAIL";
const char C_INVALID[]                                      = "INVALID";
const char C_NONE_ID[]                                      = "NONE";
const char C_UNUSED[]                                       = "UNUSED";
const char C_UNDEFINED[]                                    = "UNDEFINED";
const char C_SET_ID[]                                       = "SET";
const char C_NOT_SET_ID[]                                   = "NOT SET";
const char C_SUPPORTED_ID[]                                 = "SUPPORTED";
const char C_NOT_SUPPORTED_ID[]                             = "NOT_SUPPORTED";
const char C_SUPPORTED_AND_SET_ID[]                         = "SUPPORTED AND SET";
const char C_SUPPORTED_AND_NOT_SET_ID[]                     = "SUPPORTED AND NOT SET";
const char C_CTF_CALIBRATION_RESPONSE[]                     = "RESPONSE";                                      //  16 - 1,2,3;
const char C_CTF_CALIBRATION_REQUEST[]                      = "REQUEST";                                       //  16 - 172
const char C_AUTO_ZERO_SUPPORT_MESSAGE[]                    = "AUTO_ZERO_ENABLE";                              //  18 (1st part)
const char C_AUTO_ZERO_ENABLE[]                             = "AUTO_ZERO_ENABLE";                              //  18 (1st part)
const char C_AUTO_ZERO_STATUS[]                             = "AUTO_ZERO_STATUS";                              //  18 (2nd part)
const char C_AUTO_ZERO[]                                    = "AUTO_ZERO";                                     // 171
const char C_CALIBRATION_REQUEST_MANUAL_ZERO[]              = "MANUAL_ZERO_REQUEST";                           // 170
const char C_CALIBRATION_REQUEST_AUTO_ZERO[]                = "AUTO_ZERO_REQUEST";                             // 171
const char C_CALIBRATION_RESPONSE_MANUAL_ZERO_SUCCESS[]     = "MANUAL_ZERO_SUCCESS";                           // 172
const char C_CALIBRATION_RESPONSE_MANUAL_ZERO_FAILED[]      = "MANUAL_ZERO_FAILED";                            // 175
const char C_CUSTOM_CALIBRATION_PARAMETER_REQUEST[]         = "CUSTOM_CALIBRATION_PARAMETER_REQUEST";          // 186
const char C_CUSTOM_CALIBRATION_PARAMETER_RESPONSE[]        = "CUSTOM_CALIBRATION_PARAMETER_RESPONSE";         // 187
const char C_CUSTOM_CALIBRATION_PARAMETER_UPDATE[]          = "CUSTOM_CALIBRATION_PARAMETER_UPDATE";           // 188
const char C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_RESPONSE[] = "CUSTOM_CALIBRATION_PARAMETER_UPDATE_RESPONSE";  // 189
const char C_CRANK_LENGTH[]                                 = "CRANK_LENGTH";
const char C_CRANK_LENGTH_USED[]                            = "CRANK_LENGTH_USED";
const char C_SW_MISMATCH[]                                  = "SW_MISMATCH";
const char C_SENSOR_AVAILABILTY[]                           = "SENSOR_AVAILABILTY";
const char C_CUSTOM_CALIBRATION[]                           = "CUSTOM_CALIBRATION";

const char C_TRANSMIT_INVALID[]                             = "TRANSMIT_INVALID";
const char C_TRANSMIT_UNTIL_SUCCESS_ACKNOWLEDGED[]          = "TRANSMIT_UNTIL_SUCCESS";
const char C_TRANSMIT_NB_TIMES[]                            = "TRANSMIT_NB_TIMES";
const char C_REPLY_ACKNOWLEDGE[]                            = "REPLY_ACKNOWLEDGE";
const char C_NO_REPLY_ACKNOWLEDGE[]                         = "NO_REPLY_ACKNOWLEDGE";
const char C_REQUESTED_PAGE_NO[]                            = "REQUESTED_PAGE_NO";
const char C_DATA_PAGE[]                                    = "DATA_PAGE ";
const char C_ANT_FS_SESSION[]                               = "ANT_FS_SESSION";

const char C_DEFAULT_DEVICE_FILE[]                          = "";
const char C_DEFAULT_INPUT_FILE_NAME[]                      = "";
const char C_DEFAULT_INTERFACE[]                            = C_AUTO_INTERFACE;
const char C_DEFAULT_MC_ADDRESS_IN[]                        = "239.78.80.1";
const char C_DEFAULT_MC_ADDRESS_OUT[]                       = "";
const char C_DIAGNOSTICS_INDENT[]                           = "    ";

const char C_LEFT_PEDAL_SMOOTHNESS[]                        = "L_PDL_SMOOTH";
const char C_RIGHT_PEDAL_SMOOTHNESS[]                       = "R_PDL_SMOOTH";
const char C_COMMON_PEDAL_SMOOTHNESS[]                      = "C_PDL_SMOOTH";
const char C_RIGHT_TORQUE_EFFECTIVENESS[]                   = "R_TRQ_EFF";
const char C_LEFT_TORQUE_EFFECTIVENESS[]                    = "L_TRQ_EFF";
const char C_RIGHT_PEDAL[]                                  = "R_PEDAL";
const char C_UNKNOWN_PEDAL[]                                = "UNKNOWN_PEDAL";
const char C_OPERATING_TIME[]                               = "OPERATING_TIME";
const char C_AERO_DEVICE_HEAD[]                             = "AERO_";
const char C_AUDIO_DEVICE_HEAD[]                            = "AUDIO_";
const char C_CAD_DEVICE_HEAD[]                              = "CAD7A_";
const char C_BLDPR_DEVICE_HEAD[]                            = "BLDPR_";
const char C_ENV_DEVICE_HEAD[]                              = "ENV_";
const char C_WEIGHT_DEVICE_HEAD[]                           = "WEIGHT_";
const char C_HRM_DEVICE_HEAD[]                              = "HRM_";
const char C_MSSDM_DEVICE_HEAD[]                            = "MSSDM_";
const char C_POWER_DEVICE_HEAD[]                            = "PWRB";
const char C_PM_CALIBRATION_HEAD[]                          = "PWRB01_";
const char C_PM_GET_SET_PARAM_HEAD[]                        = "PWRB02_";
const char C_PM_MEASUREMENT_HEAD[]                          = "PWRB03_";
const char C_POWER_ONLY_DEVICE_HEAD[]                       = "PWRB10_";
const char C_WT_POWER_DEVICE_HEAD[]                         = "PWRB11_";
const char C_CT_POWER_DEVICE_HEAD[]                         = "PWRB12_";
const char C_PM_PEDAL_SMOOTH_HEAD[]                         = "PWRB13_";
const char C_CTF_POWER_DEVICE_HEAD[]                        = "PWRB20_";
const char C_PM_REQUEST_HEAD[]                              = "PWRB46_";
const char C_PM_MFR_INFO_HEAD[]                             = "PWRB50_";
const char C_PM_PROD_INFO_HEAD[]                            = "PWRB51_";
const char C_PM_STATUS_MSG_HEAD[]                           = "PWRB52_";
const char C_SPCAD_DEVICE_HEAD[]                            = "SPCAD790_";
const char C_GEAR_RATIO_DEFAULT_AS_RATIO[]                  = "51:15";
const char C_SPEED_DEVICE_HEAD[]                            = "SPB7_";
const char C_SPEED_OBSOLETE_HEAD[]                          = "SP7B_";
const char C_SBSDM_DEVICE_HEAD[]                            = "SBSDM_";
const char C_UNKNOWN_TYPE_HEAD[]                            = "TYPE";
const char C_PACKET_SAVER_HEAD[]                            = "PACKET_";
const char C_BRIDGE_MESSAGE_ID[]                            = "BDG_";
const char C_COMMENT_SYMBOL                                 = '#';
const char C_COMMENT_SYMBOL_AS_STRING[]                     = { C_COMMENT_SYMBOL, 0 };
const char C_INCLUDE_FILE[]                                 = "AM_INCLUDE";
const char C_RHO_ID[]                                       = "RHO";
const char C_SPEED_DEVICE_ID[]                              = "SPEED";
const char C_CADENCE_DEVICE_ID[]                            = "CADENCE";
const char C_POWER_DEVICE_ID[]                              = "POWER";
const char C_AERO_DEVICE_ID[]                               = "AERO";
const char C_AUDIO_DEVICE_ID[]                              = "AUDIO";
const char C_WEIGHT_DEVICE_ID[]                             = "SCALE";
const char C_BLOOD_PRESSURE_DEVICE_ID[]                     = "BLOOD";
const char C_MULTI_SPORT_DEVICE_ID[]                        = "MULTI";
const char C_STRIDE_DEVICE_ID[]                             = "STRIDE";
const char C_ENVIRONMENT_DEVICE_ID[]                        = "ENV";
const char C_HEART_RATE_DEVICE_ID[]                         = "HRM";
const char C_UNKNOWN_DEVICE_NAME[]                          = "TYPE";
const char C_PACKET_SAVER_NAME[]                            = "PACKET_SAVER_MODE";
const char C_UNKOWN_PACKET_NAME[]                           = "UNKNOWN_PACKET_COMMAND";
const char C_SEMI_COOKED_SYMBOL                             = 'S';
const char C_SEMI_COOKED_SYMBOL_AS_STRING[]                 = "S";
const char C_SPACE_REPLACEMENT                              = '-';
const char C_TAB_REPLACEMENT                                = '=';
const char C_MAC_OS_SYS_NAME[]                              = "Darwin";
const char C_LINUX_SYS_NAME[]                               = "Linux";
const char C_CYGWIN_SYS_NAME[]                              = "Cygwin";
const char C_INTERFACE_LINUX[]                              = "wlan0";
const char C_INTERFACE_MACOSX[]                             = "en0";
const char C_INTERFACE_CYGWIN[]                             = "eth0";
#if __APPLE__
const char C_INTERFACE_ROOT[]                               = "en";
#else
const char C_INTERFACE_ROOT[]                               = "wlan";
#endif

// Calibration Message ID
const int C_CTF_CALIBRATION_MESSAGE_ID                      =  16;
const int C_AUTO_ZERO_SUPPORT_MESSAGE_ID                    =  18;
const int C_CALIBRATION_REQUEST_MANUAL_ZERO_ID              = 170;
const int C_CALIBRATION_REQUEST_AUTO_ZERO_ID                = 171;
const int C_CALIBRATION_RESPONSE_MANUAL_ZERO_SUCCESS_ID     = 172;
const int C_CALIBRATION_RESPONSE_MANUAL_ZERO_FAIL_ID        = 175;
const int C_CUSTOM_CALIBRATION_PARAMETER_REQUEST_ID         = 186;
const int C_CUSTOM_CALIBRATION_PARAMETER_RESPONSE_ID        = 187;
const int C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_ID          = 188;
const int C_CUSTOM_CALIBRATION_PARAMETER_UPDATE_RESPONSE_ID = 189;

const int C_UNKNOWN_ID                                      = -1;
const int C_NO_EVENT_ID                                     =  0;
const int C_ZERO_OFFSET_UPDATE_ID                           =  1;
const int C_SLOPE_UPDATE_ID                                 =  2;
const int C_SERIAL_NUMBER_UPDATE_ID                         =  3;
const int C_AUTO_ZERO_SUPPORT_ID                            =  4;
const int C_MANUAL_ZERO_REQUEST_ID                          =  5;
const int C_AUTO_ZERO_CONFIGURATION_ID                      =  6;
const int C_MANUAL_ZERO_SUCCESS_ID                          =  7;
const int C_MANUAL_ZERO_FAIL_ID                             =  8;
const int C_CUSTOM_CALIBRATION_PARAMS_REQUEST_ID            =  9;
const int C_CUSTOM_CALIBRATION_PARAMS_RESPONSE_ID           = 10;
const int C_SET_CUSTOM_CALIBRATION_PARAMS_SUCCESS_ID        = 11;
const int C_SET_CUSTOM_CALIBRATION_PARAMS_ID                = 12;

const int    C_PORT_NO_BRIDGE_MULTICAST                     = 51113;
const int    C_ANT_PAY_LOAD_COUNT                           = C_ANT_PAYLOAD_LENGTH;
const int    C_MAC_ADDRESS_LENGTH                           =     8;
const int    C_WASP_NAME_LENGTH                             =    31;
const int    C_NB_MAGNETS_DEFAULT                           =     1;
const int    C_OFFSET_DEFAULT                               =   500;
const int    C_NB_BATTERY_STATUS                            =     8;
const int    C_TIME_OUT_SEC_DEFAULT                         =     0;
const int    C_DEFAULT_TIME_PRECISION_DEFAULT               =     6;
const int    C_DEFAULT_VALUE_PRECISION_DEFAULT              =     6;
const int    C_DEFAULT_MC_PORT_NO_IN                        = 51113;
const int    C_DEFAULT_MC_PORT_NO_OUT                       =     0;
const int    C_DEFAULT_TIME_OUT_SEC                         =     0;   // Time out after x seconds of inactivity. x = 0: Do not time out.

const double C_DBL_UNDEFINED                                = 1.0E13;
const double C_TWO_PI                                       = 4.0 * acos( 0 );         // 2 PI
const double C_GEAR_RATIO_DEFAULT                           =   3.4; // 51:15;
const double C_WHEEL_CIRCUMFERENCE_DEFAULT                  =   2.096;    // meters
const double C_AIR_SPEED_MULTIPLIER_DEFAULT                 =   1.0;
const double C_RHO_DEFAULT                                  =   1.18;     // kg/m^3
const double C_CALIBRATION_RHO_DEFAULT                      = C_RHO_DEFAULT;
const double C_MIN_POWER_VALUE                              =    0.0;
const double C_MAX_POWER_VALUE                              = 2500.0;
const double C_MIN_CADENCE                                  =    0.0;
const double C_MAX_CADENCE                                  =  256.0;
const double C_MIN_PM_SLOPE                                 =   10.0;
const double C_MAX_PM_SLOPE                                 =   50.0;
const double C_MIN_RHO                                      =    0.5;
const double C_MAX_RHO                                      =    2.0;
const double C_MIN_MULTIPLIER                               =    0.1;
const double C_MAX_MULTIPLIER                               =    2.0;
const double C_MIN_CIRCUMFERENCE                            =    1.0;
const double C_MAX_CIRCUMFERENCE                            =    3.0;
const double C_MIN_GEAR_RATIO                               =    1.0;
const double C_MAX_GEAR_RATIO                               =   10.0;

const unsigned int C_MIN_PRECISION                          =    0;
const unsigned int C_MAX_PRECISION                          =   20;
const unsigned int C_MIN_ZERO_OFFSET                        =   50;
const unsigned int C_MAX_ZERO_OFFSET                        = 1000;
const unsigned int C_MIN_NB_MAGNETS                         =    1;
const unsigned int C_MAX_NB_MAGNETS                         =    4;

const unsigned int C_SLOPE_DEFAULT                          = 0xFFFFFFFF;
const unsigned int C_NON_EXISTENT_DATA_PAGE                 = 256;
const unsigned int C_MAX_INTERFACE_COUNT                    =  10;
const unsigned int S_DIAGNOSTICS_BASE_LENGTH                =  28;
const unsigned int C_MAX_ZERO_TIME_POWER_B10                =   6;
const unsigned int C_MAX_ZERO_TIME_POWER_B11                =   4;
const unsigned int C_MAX_ZERO_TIME_POWER_B12                =   4;
const unsigned int C_MAX_ZERO_TIME_POWER_B20                =   4;
const unsigned int C_MAX_ZERO_TIME_SPEED                    =  12;
const unsigned int C_MAX_ZERO_TIME_CADENCE                  =  12;
const unsigned int C_MAX_ZERO_TIME_HRM                      =  12;
const unsigned int C_MAX_ZERO_TIME                          =  12;  // the largest of all C_MAX_ZERO_TIME_xyz

const BYTE C_ANT_ASYNC_MSG                         = 0x12;
const BYTE C_ANT_PACKET_SAVER                      = 0x13;
const BYTE C_QUERY_RESP                            = 0x43;

#endif // __ANT_CONSTANTS_H__

