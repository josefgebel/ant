#ifndef __ANT_WEIGHT_PROCESSING_H__
#define __ANT_WEIGHT_PROCESSING_H__

#include "ant_processing.h"

class antWeightProcessing : virtual public antProcessing
{

    private:

        void getWeightScaleCapabilities( int capabilities );


    protected:

        std::map<amString, unsigned int> previousHeartRateTable;
        std::map<amString, unsigned int> eventTimeTable;
        std::map<amString, double>       heartBeatTimeTable;

        amDeviceType processWeightScaleSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processWeightScaleSensorSemiCooked( const amString &inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendWeightSensor( const amString & );


    public:

        antWeightProcessing();
        ~antWeightProcessing() {}

        bool isWeightScaleSensor( const amString & );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

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

        virtual void reset( void );

};

#endif // __ANT_WEIGHT_PROCESSING_H__

