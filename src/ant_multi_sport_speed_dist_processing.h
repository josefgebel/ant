#ifndef __ANT_MULTI_SPORT_PROCESSING_H__
#define __ANT_MULTI_SPORT_PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"

class amSplitString;

class antMultiSportProcessing : virtual public antProcessing
{

    private:

        double convertTemp10( int temp10 );


    protected:

        std::map<std::string, double>       totalDistTable;
        std::map<std::string, unsigned int> eventDistTable;

        amDeviceType processMultiSportSpeedAndDistanceSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processMultiSportSpeedAndDistanceSensorSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendMultiSportSensor( const std::string & );


    public:

        antMultiSportProcessing();
        ~antMultiSportProcessing() {}

        bool isMultiSportSensor( const std::string & );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        virtual void reset( void );

        void createMSSDMResultString
             (
                 unsigned int dataPage,
                 unsigned int additionalData1,
                 unsigned int additionalData2,
                 unsigned int additionalData3,
                 double       additionalDoubleData1,
                 double       additionalDoubleData2
             );

};

#endif // __ANT_MULTI_SPORT_PROCESSING_H__

