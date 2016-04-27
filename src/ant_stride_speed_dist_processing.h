#ifndef __ANT_STRIDE_SPEED_DIST__PROCESSING_H__
#define __ANT_STRIDE_SPEED_DIST__PROCESSING_H__

#include "ant_processing.h"

class antStrideSpeedDistProcessing : virtual public antProcessing
{
    protected:

        std::map<amString, double>       totalDistTable;
        std::map<amString, unsigned int> eventDistTable;
        std::map<amString, unsigned int> totalStrideCountTable;
        std::map<amString, unsigned int> strideCountTable;

        amDeviceType processStrideBasedSpeedAndDistanceSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processStrideBasedSpeedAndDistanceSensorSemiCooked( const amString &inputBuffer );

        void createSBSDMResultString
             (
                 unsigned int dataPage,
                 unsigned int additionalData1,
                 unsigned int additionalData2,
                 unsigned int additionalData3,
                 unsigned int additionalData4,
                 unsigned int additionalData5,
                 unsigned int additionalData6,
                 unsigned int additionalData7,
                 unsigned int additionalData8
             );

        virtual void readDeviceFileLine( const char *line );

        bool appendStrideSpeedDistSensor( const amString & );


    public:

        antStrideSpeedDistProcessing();
        ~antStrideSpeedDistProcessing() {}

        bool isStrideSpeedSensor( const amString & );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        virtual void reset( void );
};

#endif // __ANT_STRIDE_SPEED_DIST__PROCESSING_H__

