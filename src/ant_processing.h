#ifndef __ANT_PROCESSING_H__
#define __ANT_PROCESSING_H__

#include <map>
#include "am_split_string.h"

class antProcessing
{

    private:

        int readSemiCookedSingleLineFromStream( std::istream &inStream );
        int readAntSingleLineFromStream       ( std::istream &inStream,   int outSocketID, struct sockaddr_in &outGroupSocket );
        int readAntFromStream   ( std::istream &inStream,                 int outSocketID, struct sockaddr_in &outGroupSocket );
        int ant2txtLine         ( const unsigned char *line, int nbBytes, int outSocketID, struct sockaddr_in &outGroupSocket );
        int readAntFromMultiCast(                                         int outSocketID, struct sockaddr_in &outGroupSocket );
        int outputData          (                                         int outSocketID, struct sockaddr_in &outGroupSocket );
        int errorCode;

        std::string programName;
        std::string validOptions;

        void appendDiagnosticsItemName( const std::string &itemName );
        void appendDiagnosticsField( const std::string &fieldName);

        bool testMode;
        int  testCounter;


    protected:

        char doubleValueFormatString[ C_SMALL_BUFFER_SIZE ];
        char timeValueFormatString[ C_SMALL_BUFFER_SIZE ];
        char errorMessage[ C_BUFFER_SIZE ];
        std::string outBuffer;
        std::string rawBuffer;
        std::string diagnosticsBuffer;

        void resetRawBuffer        ( void ) { rawBuffer.clear(); }
        void resetOutBuffer        ( void ) { outBuffer.clear(); }
        void resetDiagnosticsBuffer( void ) { diagnosticsBuffer.clear(); }

        std::string deviceFileName;
        std::string mcAddressIn;
        std::string mcAddressOut;
        std::string interface;
        std::string inputFileName;
        std::string b2tVersion;

        int timePrecision;
        int valuePrecision;
        int mcPortNoIn;
        int mcPortNoOut;
        int timeOutSec;

        bool useLocalTime;
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

        std::map<std::string, bool> registeredDevices;
        std::vector<std::string>    supportedSensorTypes;

        std::map<std::string, unsigned int> zeroTimeCountTable;
        std::map<std::string, unsigned int> eventTimeTable;
        std::map<std::string, unsigned int> eventCountTable;
        std::map<std::string, unsigned int> totalTimeIntTable;
        std::map<std::string, unsigned int> totalCountTable;
        std::map<std::string, unsigned int> operatingTimeTable;
        std::map<std::string, unsigned int> sameEventCountTable;
        std::map<std::string, double>       totalTimeTable;
        std::map<std::string, double>       totalOperatingTimeTable;

        virtual void initializeSupportedDeviceTypes( void ) { }
        virtual int  readDeviceFileStream( std::ifstream &deviceFileStream );

        inline void setMaxZeroTime( unsigned int value ) { maxZeroTime = value; }
        void getUnixTimeAsString( std::string &timeStampBuffer );
        void getUnixTimeAsString( std::string &timeStampBuffer, double subSecondTimer );

        void setZeroTimeCount( const std::string &sensorID, unsigned int value );
        unsigned int getZeroTimeCount( const std::string &sensorID );

        bool createCommonResultStringPage67
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       value1,
                 unsigned int       value2,
                 unsigned int       value3,
                 unsigned int       value4,
                 unsigned int       value5
                 );
        bool createCommonResultStringPage68
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       value1,
                 unsigned int       value2,
                 unsigned int       value3,
                 unsigned int       value4
                 );

        bool createCommonResultStringPage70
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       descriptor1,
                 unsigned int       descriptor2,
                 unsigned int       requestedResponse,
                 unsigned int       requestedPageNo,
                 unsigned int       commandType
             );
        bool createCommonResultStringPage80
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       manufacturerID,
                 unsigned int       hardwareRevision,
                 unsigned int       modelNumber
             );
        bool createCommonResultStringPage81
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       serialNumber,
                 unsigned int       softwareRevision
             );

        bool createCommonResultStringPage82
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       voltage256,
                 unsigned int       status,
                 unsigned int       operatingTime,
                 unsigned int       resolution,
                 unsigned int       nbBatteries,
                 unsigned int       batteryID
             );
        bool createCommonResultStringPage83
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       seconds,
                 unsigned int       minutes,
                 unsigned int       hours,
                 unsigned int       weekDayNo,
                 unsigned int       monthDay,
                 unsigned int       month,
                 unsigned int       year
             );
        bool createCommonResultStringPage84
             (
                 const std::string &sensorID,
                 bool               outputPageNo,
                 unsigned int       subPage1,
                 unsigned int       subPage2,
                 unsigned int       dataField1,
                 unsigned int       dataField2
             );
        void createDataPage84SubPage( const std::string &sensorID, unsigned int subPageNo, unsigned int subPage, unsigned int dataField );

        amDeviceType processUndefinedSensorType( const char *inputBuffer );

        void convertAdditionalDataFromSemiCooked( char *additionalData );
        void convertAdditionalDataToSemiCooked( char *additionalData );

        unsigned int getDeltaInt
        (
            bool                                &rollOverHappened,
            const std::string                   &sensorID,
            unsigned int                         rollOver,
            std::map<std::string, unsigned int> &valueTable,
            unsigned int                         newValue
        );

        bool isRegisteredDevice( const std::string & );
        void registerDevice( const std::string & );

        void printDoubleValue( char *resultBuffer, const char *formatString, double value );

        bool isSupportedSensor( const std::string &deviceID );
        bool isSemiCookedFormat137( const char *inputBuffer );

        bool getBatteryStatus( char *buffer, int index, bool lowerCase = false );

        // Diagnostics
        void appendDiagnosticsLine( const std::string &message );
        void appendDiagnosticsLine( const std::string &itemName, const std::string &itemValue );
        void appendDiagnosticsLine( const std::string &itemName, unsigned int itemValue, const std::string &additionalInfo = "" );
        void appendDiagnosticsLine( const std::string &itemName, unsigned int index, unsigned char itemValue );
        void appendDiagnosticsLine( const std::string &itemName, const std::string &stringValue, unsigned int itemValue, const std::string &additionalInfo = "" );
        void appendDiagnosticsLine( const std::string &itemName, unsigned char byteValue, unsigned int itemValue, const std::string &additionalInfo = "" );
        void appendDiagnosticsLine( const std::string &itemName, unsigned char byteValue1, unsigned char byteValue2, unsigned int itemValue, const std::string &additionalInfo = "" );
        void appendDiagnosticsLine( const std::string &itemName, unsigned char byteValue1, unsigned char byteValue2, unsigned char byteValue3, unsigned int itemValue, const std::string &additionalInfo = "" );
        void appendDiagnosticsLine( const std::string &itemName, unsigned char byteValue1, unsigned char byteValue2, unsigned char byteValue3, unsigned char byteValue4, unsigned int itemValue, const std::string &additionalInfo = "" );


        // Auxilairy functions
        unsigned int uChar2UInt( unsigned char byte1 );
        unsigned int uChar2UInt( unsigned char byte1, unsigned char byte2 );
        unsigned int uChar2UInt( unsigned char byte1, unsigned char byte2, unsigned char byte3 );
        unsigned int uChar2UInt( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4 );
        void printDouble( char *resultBuffer, double value, int precision );
        void appendOutput( unsigned char itemValue );
        void appendOutput( int itemValue, const std::string &unit = "" );
        void appendOutput( double itemValue, unsigned int precision, const std::string &unit = "" );
        void appendOutput( unsigned int itemValue, const std::string &unit = "" );
        void appendOutput( const std::string &itemValue );
        void appendJSONItem( const char *itemName, unsigned char      itemValue );
        void appendJSONItem( const char *itemName, unsigned int       itemValue );
        void appendJSONItem( const char *itemName, int                itemValue );
        void appendJSONItem( const char *itemName, bool               itemValue );
        void appendJSONItem( const char *itemName, double             itemValue, int precision );
        void appendJSONItem( const char *itemName, const char        *itemValue );
        void appendJSONItem( const char *itemName, const std::string &itemValue );

        void createUnknownDeviceTypeString( int deviceType, int deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        void createOutputHeader( const std::string &sensorID, const std::string &timeStampBuffer );
        void appendOutputFooter( const std::string &versionString );


        bool processCommonPagesSemiCooked
             (
                 const amSplitString &words,
                 unsigned int        startCounter,
                 unsigned int        dataPage,
                 bool                outputPageNo
             );
        bool processCommonPages( const std::string &sensorID, unsigned char payLoad[], bool outputPageNo );

        void resetAll( void );


    public:

        antProcessing();
        ~antProcessing() {}

        bool processInput( const std::string &programNameIn, const std::string &validOptionsIn, const std::string &deviceType, int argc, char *argv[] );

        int getErrorCode( void ) const { return errorCode; }
        void setErrorCode( int value ) { errorCode = value; }

        void help( void );
        void outputFormats( const std::string &deviceType );
        void outputOptionF( std::stringstream &outputMessage, const std::string &indent );
        void outputOptionPOWER( std::stringstream &outputMessage, const std::string &indent );

        virtual void reset( void );

        inline std::string getDeviceFileName( void ) const        { return deviceFileName; }
        inline void        setDeviceFileName( std::string value ) { deviceFileName = value; }

        inline std::string getMCAddressIn( void ) const        { return mcAddressIn; }
        inline void        setMCAddressIn( std::string value ) { mcAddressIn = value; }

        inline std::string getMCAddressOut( void ) const        { return mcAddressOut; }
        inline void        setMCAddressOut( std::string value ) { mcAddressOut = value; }

        inline std::string getInterface( void ) const        { return interface; }
        inline void        setInterface( std::string value ) { interface = value; }

        inline std::string getInputFileName( void ) const        { return inputFileName; }
        inline void        setInputFileName( std::string value ) { inputFileName = value; }

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

        inline bool getUseLocalTime( void ) const { return useLocalTime; }
        inline void setUseLocalTime( bool value ) { useLocalTime = value; }

        inline void setOutputUnknown( bool value ) { outputUnknown = value; }
        inline bool getOutputUnknown( void ) const { return outputUnknown; }

        inline void setOutputBridge( bool value ) { outputBridge = value; }
        inline bool getOutputBridge( void ) const { return outputBridge; }

        inline void setTestMode( bool value ) { testMode = value; }
        inline bool getTestMode( void ) const { return testMode; }

        double getTotalOperationTime( const std::string &sensorID );
        void setTotalOperationTime( const std::string &sensorID, double value );

        inline const char *getErrorMessage( void ) const { return errorMessage; }

        int readDeviceFile( void );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );
        amDeviceType         updateSensorSemiCooked ( const char *inputBuffer );

        void createPacketSaverModeString( const std::string &timeStampBuffer, const unsigned char *payLoad, unsigned char nbBytes );

        void createBridgeString
             (
                 const std::string &bridgeDeviceID,
                 const std::string &timeStampBuffer,
                 const std::string &bridgeName,
                 const std::string &bridgeMACAddess,
                 const std::string &firmwareVersion,
                 int                voltageValue,
                 int                powerIndicator,
                 int                operatingMode,
                 int                connectionStatus
            );

        void createPacketSaverString( const unsigned char *line, int nbBytes, const std::string &timeStampBuffer );
        void createUnknownPacketString( const unsigned char *line, int nbBytes, const std::string &timeStampBuffer );
     
        int ant2txt( void );

};

#endif // __ANT_PROCESSING_H__

