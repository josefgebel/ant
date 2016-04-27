#ifndef __ANT_ENVIRONMENT_PROCESSING_H__
#define __ANT_ENVIRONMENT_PROCESSING_H__

#include "ant_processing.h"

class antEnvironmentProcessing : virtual public antProcessing
{

    private:

        double convertTemp10 ( int temp10 );
        double convertTemp100( int temp100 );

        std::map<amString, unsigned int> previousHeartRateTable;
        std::map<amString, unsigned int> eventTimeTable;
        std::map<amString, double>       heartBeatTimeTable;


    protected:

        amDeviceType processEnvironmentSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processEnvironmentSensorSemiCooked( const amString &inputBuffer );

        virtual void readDeviceFileLine( const char *line );
        bool appendEnvironmentSensor( const amString & );


    public:

        antEnvironmentProcessing();
        ~antEnvironmentProcessing() {}

        bool isEnvironmentSensor( const amString &deviceID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        void createENVResultString
             (
                 unsigned int dataPage,
                 unsigned int additionalData1,
                 unsigned int additionalData2,
                 unsigned int additionalData3,
                 unsigned int additionalData4
             );

        virtual void reset( void );

};

#endif // __ANT_ENVIRONMENT_PROCESSING_H__

