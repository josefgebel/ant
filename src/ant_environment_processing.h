#ifndef __ANT_ENVIRONMENT_PROCESSING_H__
#define __ANT_ENVIRONMENT_PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"

class amSplitString;

class antEnvironmentProcessing : virtual public antProcessing
{

    private:

        double convertTemp10 ( int temp10 );
        double convertTemp100( int temp100 );


    protected:

        std::map<std::string, unsigned int> previousHeartRateTable;
        std::map<std::string, unsigned int> eventTimeTable;
        std::map<std::string, double>       heartBeatTimeTable;

        amDeviceType processEnvironmentSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processEnvironmentSensorSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendEnvironmentSensor( const std::string & );


    public:

        antEnvironmentProcessing();
        ~antEnvironmentProcessing() {}

        bool isEnvironmentSensor( const std::string &deviceID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        void createENVResultString
             (
                 unsigned int dataPage,
                 unsigned int additionalData1,
                 unsigned int additionalData2,
                 unsigned int additionalData3,
                 unsigned int additionalData4
             );

};

#endif // __ANT_ENVIRONMENT_PROCESSING_H__

