#ifndef __ANT_WEIGHT_PROCESSING_H__
#define __ANT_WEIGHT_PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"

class amSplitString;

class antWeightProcessing : virtual public antProcessing
{

    private:

        void getWeightScaleCapabilities( int capabilities );


    protected:

        std::map<std::string, unsigned int> previousHeartRateTable;
        std::map<std::string, unsigned int> eventTimeTable;
        std::map<std::string, double>       heartBeatTimeTable;

        amDeviceType processWeightScaleSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processWeightScaleSensorSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendWeightSensor( const std::string & );


    public:

        antWeightProcessing();
        ~antWeightProcessing() {}

        bool isWeightScaleSensor( const std::string & );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        void createWeightScaleString
             (
                 unsigned int dataPage,
                 unsigned int additionalData1,
                 unsigned int additionalData2,
                 unsigned int additionalData3,
                 unsigned int additionalData4,
                 unsigned int additionalData5,
                 unsigned int additionalData6
             );

};

#endif // __ANT_WEIGHT_PROCESSING_H__

