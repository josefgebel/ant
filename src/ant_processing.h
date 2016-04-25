#ifndef __ANT_PROCESSING_H__
#define __ANT_PROCESSING_H__

#include <map>
#include <vector>

#include <am_multicast_read.h>
#include <am_multicast_write.h>

#include "ant_constants.h"
#include "am_string.h"

class amSplitString;

class antProcessing
{

    private:

        bool     testMode;
        int      testCounter;
        amString programName;
        amString validOptions;

        amMulticastRead  multicastRead;
        amMulticastWrite multicastWrite;

        int readSemiCookedSingleLineFromStream( std::istream &inStream );
        int readAntSingleLineFromStream       ( std::istream &inStream );
        int readAntFromStream   ( std::istream &inStream );
        int ant2txtLine         ( const BYTE *line, int nbBytes );
        int readAntFromMultiCast( void );
        int outputData          ( void );

        void appendDiagnosticsItemName( const amString &itemName );
        void appendDiagnosticsField   ( const amString &fieldName);


    protected:

        int      errorCode;

        amString errorMessage;
        amString outBuffer;
        amString rawBuffer;
        amString diagnosticsBuffer;
        amString deviceFileName;
        amString mcAddressIn;
        amString mcAddressOut;
        amString interface;
        amString inputFileName;
        amString b2tVersion;
        amString currentDeviceType;

        int timePrecision;
        int valuePrecision;
        int mcPortNoIn;
        int mcPortNoOut;
        int timeOutSec;

        bool outputUnknown;
        bool outputBridge;
        bool writeStdout;
        bool semiCookedIn;
        bool semiCookedOut;
        bool outputAsJSON;
        bool outputRaw;
        bool diagnostics;
        bool onlyRegisteredDevices;

        unsigned int maxZeroTime;

        std::vector<amString>            supportedSensorTypes;
        std::map<amString, bool>         registeredDevices;
        std::map<amString, unsigned int> zeroTimeCountTable;
        std::map<amString, unsigned int> eventTimeTable;
        std::map<amString, unsigned int> eventCountTable;
        std::map<amString, unsigned int> totalTimeIntTable;
        std::map<amString, unsigned int> totalCountTable;
        std::map<amString, unsigned int> operatingTimeTable;
        std::map<amString, unsigned int> sameEventCountTable;
        std::map<amString, double>       totalTimeTable;
        std::map<amString, double>       totalOperatingTimeTable;

        void resetRawBuffer        ( void ) { rawBuffer.clear(); }
        void resetOutBuffer        ( void ) { outBuffer.clear(); }
        void resetDiagnosticsBuffer( void ) { diagnosticsBuffer.clear(); }
        void clearErrors           ( void ) { errorMessage.clear(); errorCode = 0; }

        virtual void initializeSupportedDeviceTypes( void ) { }
        virtual int  readDeviceFileStream( std::ifstream &deviceFileStream );

        int hex2Int( BYTE b1 );
        int hex2Int( BYTE b2, BYTE b1 );
        int hex2Int( BYTE b3, BYTE b2, BYTE b1 );
        int hex2Int( BYTE b4, BYTE b3, BYTE b2, BYTE b1 );

        double getUnixTime( void );
        void getUnixTimeAsString( amString &timeStampBuffer );
        void getUnixTimeAsString( amString &timeStampBuffer, double subSecondTimer );
        void setZeroTimeCount( const amString &sensorID, unsigned int value );
        unsigned int getZeroTimeCount( const amString &sensorID );

        inline void setMaxZeroTime( unsigned int value ) { maxZeroTime = value; }

        bool createCommonResultStringPage67
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    value1,
                 unsigned int    value2,
                 unsigned int    value3,
                 unsigned int    value4,
                 unsigned int    value5
             );
        bool createCommonResultStringPage68
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    value1,
                 unsigned int    value2,
                 unsigned int    value3,
                 unsigned int    value4
             );
        bool createCommonResultStringPage70
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    descriptor1,
                 unsigned int    descriptor2,
                 unsigned int    requestedResponse,
                 unsigned int    requestedPageNo,
                 unsigned int    commandType
             );
        bool createCommonResultStringPage80
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    manufacturerID,
                 unsigned int    hardwareRevision,
                 unsigned int    modelNumber
             );
        bool createCommonResultStringPage81
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    serialNumber,
                 unsigned int    softwareRevision
             );
        bool createCommonResultStringPage82
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    voltage256,
                 unsigned int    status,
                 unsigned int    operatingTime,
                 unsigned int    resolution,
                 unsigned int    nbBatteries,
                 unsigned int    batteryID
             );
        bool createCommonResultStringPage83
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    seconds,
                 unsigned int    minutes,
                 unsigned int    hours,
                 unsigned int    weekDayNo,
                 unsigned int    monthDay,
                 unsigned int    month,
                 unsigned int    year
             );
        bool createCommonResultStringPage84
             (
                 const amString &sensorID,
                 bool            outputPageNo,
                 unsigned int    subPage1,
                 unsigned int    subPage2,
                 unsigned int    dataField1,
                 unsigned int    dataField2
             );
        void createDataPage84SubPage( const amString &sensorID, unsigned int subPageNo, unsigned int subPage, unsigned int dataField );
        bool getBatteryStatus( amString &status, int index, bool lowerCase = false );

        unsigned int getDeltaInt
                     (
                         bool                             &rollOverHappened,
                         const amString                   &sensorID,
                         unsigned int                      rollOver,
                         std::map<amString, unsigned int> &valueTable,
                         unsigned int                      newValue
                     );

        bool isRegisteredDevice( const amString & );
        void registerDevice    ( const amString & );

        bool isSupportedSensor    ( const amString &deviceID );
        bool isSemiCookedFormat137( const amString &inputBuffer );

        // Diagnostics
        void appendDiagnosticsLine( const amString &message );
        void appendDiagnosticsLine( const amString &itemName, const amString &itemValue );
        void appendDiagnosticsLine( const amString &itemName, unsigned int itemValue, const amString &additionalInfo = "" );
        void appendDiagnosticsLine( const amString &itemName, unsigned int index, BYTE itemValue );
        void appendDiagnosticsLine( const amString &itemName, const amString &stringValue, unsigned int itemValue, const amString &additionalInfo = "" );
        void appendDiagnosticsLine( const amString &itemName, BYTE byteValue, unsigned int itemValue, const amString &additionalInfo = "" );
        void appendDiagnosticsLine( const amString &itemName, BYTE byteValue1, BYTE byteValue2, unsigned int itemValue, const amString &additionalInfo = "" );
        void appendDiagnosticsLine( const amString &itemName, BYTE byteValue1, BYTE byteValue2, BYTE byteValue3, unsigned int itemValue, const amString &additionalInfo = "" );
        void appendDiagnosticsLine( const amString &itemName, BYTE byteValue1, BYTE byteValue2, BYTE byteValue3, BYTE byteValue4, unsigned int itemValue, const amString &additionalInfo = "" );


        // Auxilairy functions
        unsigned int uChar2UInt( BYTE byte1 );
        unsigned int uChar2UInt( BYTE byte1, BYTE byte2 );
        unsigned int uChar2UInt( BYTE byte1, BYTE byte2, BYTE byte3 );
        unsigned int uChar2UInt( BYTE byte1, BYTE byte2, BYTE byte3, BYTE byte4 );
        void appendOutput( BYTE itemValue );
        void appendOutput( int itemValue, const amString &unit = "" );
        void appendOutput( double itemValue, unsigned int precision, const amString &unit = "" );
        void appendOutput( unsigned int itemValue, const amString &unit = "" );
        void appendOutput( const amString &itemValue );
        void appendOutputConditional( bool condition, int             itemValueTrue, const amString &itemValueFalse );
        void appendOutputConditional( bool condition, unsigned int    itemValueTrue, const amString &itemValueFalse );
        void appendOutputConditional( bool condition, double          itemValueTrue, const amString &itemValueFalse, int precision );
        void appendOutputConditional( bool condition, const amString &itemValueTrue, const amString &itemValueFalse );
        void appendOutput4Way( int  condition, const amString &itemValue0, const amString &itemValue1, const amString &itemValue2, const amString &itemValue3 );
        void appendJSONItem( const amString &itemName, BYTE   itemValue );
        void appendJSONItem( const amString &itemName, unsigned int    itemValue );
        void appendJSONItem( const amString &itemName, int             itemValue );
        void appendJSONItem( const amString &itemName, double          itemValue, int precision );
        void appendJSONItem( const amString &itemName, const amString &itemValue );
        void appendJSONItemB( const amString &itemName, bool           itemValue );
        void appendJSONItemConditional( const amString &itemName, bool condition, int             itemValueTrue, const amString &itemValueFalse );
        void appendJSONItemConditional( const amString &itemName, bool condition, unsigned int    itemValueTrue, const amString &itemValueFalse );
        void appendJSONItemConditional( const amString &itemName, bool condition, double          itemValueTrue, const amString &itemValueFalse, int precision = 6 );
        void appendJSONItemConditional( const amString &itemName, bool condition, const amString &itemValueTrue, const amString &itemValueFalse );
        void appendJSONItem4Way( const amString &itemName, int condition, const amString &itemValue0, const amString &itemValue1, const amString &itemValue2, const amString &itemValue3 );

        void createUnknownDeviceTypeString( int deviceType, int deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        void createOutputHeader( const amString &sensorID, const amString &timeStampBuffer );
        void appendOutputFooter( const amString &versionString );

        amDeviceType processUndefinedSensorType( const amString &inputBuffer );

        bool processCommonPagesSemiCooked( const amSplitString &words, unsigned int startCounter, unsigned int dataPage, bool outputPageNo );
        bool processCommonPages( const amString &sensorID, BYTE payLoad[], bool outputPageNo );

        void resetAll( void );


    public:

        antProcessing();
        ~antProcessing() {}

        bool processArguments( const amString &programNameIn, const amString &validOptionsIn, const amString &deviceType, int argc, char *argv[] );

        void help( void );
        void outputFormats( const amString &deviceType );
        void outputOptionF( std::stringstream &outputMessage, const amString &indent );
        void outputOptionPOWER( std::stringstream &outputMessage, const amString &indent );

        int  getErrorCode( void ) const { return errorCode; }
        void outputError( void );

        virtual void reset( void );

        inline amString getDeviceFileName( void ) const     { return deviceFileName; }
        inline void     setDeviceFileName( amString value ) { deviceFileName = value; }

        inline amString getMCAddressIn( void ) const     { return mcAddressIn; }
        inline void     setMCAddressIn( amString value ) { mcAddressIn = value; }

        inline amString getMCAddressOut( void ) const     { return mcAddressOut; }
        inline void     setMCAddressOut( amString value ) { mcAddressOut = value; }

        inline amString getInterface( void ) const     { return interface; }
        inline void     setInterface( amString value ) { interface = value; }

        inline amString getInputFileName( void ) const     { return inputFileName; }
        inline void     setInputFileName( amString value ) { inputFileName = value; }

        inline int  getMCPortNoIn( void ) const { return mcPortNoIn; }
        inline void setMCPortNoIn( int value )  { mcPortNoIn = value; }

        inline int  getMCPortNoOut( void ) const { return mcPortNoOut; }
        inline void setMCPortNoOut( int value )  { mcPortNoOut = value; }

        inline int  getTimeOutSec( void ) const { return timeOutSec; }
        inline void setTimeOutSec( int value )  { timeOutSec = value; }

        inline int getTimePrecision( void ) const { return timePrecision; }
        void       setTimePrecision( int );

        inline int getValuePrecision( void ) const { return valuePrecision; }
        void       setValuePrecision( int );

        inline bool getWriteStdout( void ) const { return writeStdout; }
        inline void setWriteStdout( bool value ) { writeStdout = value; }

        inline bool getSemiCookedIn( void ) const { return semiCookedIn; }
        inline void setSemiCookedIn( bool value ) { semiCookedIn = value; }

        inline bool getSemiCookedOut( void ) const { return semiCookedOut; }
        inline void setSemiCookedOut( bool value ) { semiCookedOut = value; }

        inline bool getOutputAsJSON( void ) const { return outputAsJSON; }
        inline void setOutputAsJSON( bool value ) { outputAsJSON = value; }

        inline bool getOutputRaw( void ) const { return outputRaw; }
        inline void setOutputRaw( bool value ) { outputRaw = value; }

        inline bool getOnlyRegisteredDevices( void ) const { return onlyRegisteredDevices; }
        inline void setOnlyRegisteredDevices( bool value ) { onlyRegisteredDevices = value; }

        inline bool getDiagnostics( void ) const { return diagnostics; }
        inline void setDiagnostics( bool value ) { diagnostics = value; }

        inline void setOutputUnknown( bool value ) { outputUnknown = value; }
        inline bool getOutputUnknown( void ) const { return outputUnknown; }

        inline void setOutputBridge( bool value ) { outputBridge = value; }
        inline bool getOutputBridge( void ) const { return outputBridge; }

        inline void setTestMode( bool value ) { testMode = value; }
        inline bool getTestMode( void ) const { return testMode; }

        double getTotalOperationTime( const amString &sensorID );
        void setTotalOperationTime( const amString &sensorID, double value );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );
        amDeviceType         updateSensorSemiCooked ( const amString &inputBuffer );

        void createPacketSaverModeString( const amString &timeStampBuffer, const BYTE *payLoad, BYTE nbBytes );

        void createBridgeString
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
            );

        void createPacketSaverString( const BYTE *line, int nbBytes, const amString &timeStampBuffer );
        void createUnknownPacketString( const BYTE *line, int nbBytes, const amString &timeStampBuffer );

        int ant2txt( void );

};

#endif // __ANT_PROCESSING_H__

