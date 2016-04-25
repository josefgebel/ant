#ifndef __ANT_HRM_PROCESSING_H__
#define __ANT_HRM_PROCESSING_H__

#include "ant_processing.h"

class antHRMProcessing : virtual public antProcessing
{
    private:

        std::map<amString, bool>         heartRateSensorTable;
        std::map<amString, unsigned int> previousHeartRateTable;
        std::map<amString, unsigned int> eventTimeTable;
        std::map<amString, double>       heartBeatTimeTable;


    protected:

        amDeviceType processHRMSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processHRMSensorSemiCooked( const amString &inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );


    public:

        antHRMProcessing();
        ~antHRMProcessing() {}

        bool isHeartRateSensor ( const amString &deviceID );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendHRMSensor   ( const amString &deviceName );
        bool isRegisteredSensor( const amString &deviceID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

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

