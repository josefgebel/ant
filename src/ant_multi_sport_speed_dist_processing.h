#ifndef __ANT_MULTI_SPORT_PROCESSING_H__
#define __ANT_MULTI_SPORT_PROCESSING_H__

#include "ant_processing.h"

class antMultiSportProcessing : virtual public antProcessing
{

    private:

        double convertTemp10( int temp10 );


    protected:

        std::map<amString, double>       totalDistTable;
        std::map<amString, unsigned int> eventDistTable;

        amDeviceType processMultiSportSpeedAndDistanceSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processMultiSportSpeedAndDistanceSensorSemiCooked( const amString &inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendMultiSportSensor( const amString & );


    public:

        antMultiSportProcessing();
        ~antMultiSportProcessing() {}

        bool isMultiSportSensor( const amString & );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

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

