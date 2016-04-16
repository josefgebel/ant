#ifndef __ANT_HRM_PROCESSING_H__
#define __ANT_HRM_PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"

class amSplitString;

class antHRMProcessing : virtual public antProcessing
{
    private:

        std::map<std::string, bool>         heartRateSensorTable;
        std::map<std::string, unsigned int> previousHeartRateTable;
        std::map<std::string, unsigned int> eventTimeTable;
        std::map<std::string, double>       heartBeatTimeTable;


    protected:

        amDeviceType processHRMSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processHRMSensorSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );


    public:

        antHRMProcessing();
        ~antHRMProcessing() {}

        bool isHeartRateSensor( const std::string &deviceID );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendHRMSensor( const std::string &deviceName );
        bool isRegisteredSensor( const std::string &deviceID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        virtual void reset( void );

        void createHRMResultString
             (
                 unsigned int dataPage,
                 unsigned int heartRate,
                 unsigned int deltaHeartBeatEventTime,
                 unsigned int deltaHeartBeatCount,
                 double       totalHeartBeatEventTime,
                 unsigned int totalHeartBeatCount,
                 unsigned int additionalData1,
                 unsigned int additionalData2,
                 unsigned int additionalData3,
                 double       additionalDoubleData1
             );

};

#endif // __ANT_HRM_PROCESSING_H__

