#ifndef __ANT_STRIDE_SPEED_DIST__PROCESSING_H__
#define __ANT_STRIDE_SPEED_DIST__PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"

class amSplitString;

class antStrideSpeedDistProcessing : virtual public antProcessing
{
    protected:

        std::map<std::string, double>       totalDistTable;
        std::map<std::string, unsigned int> eventDistTable;
        std::map<std::string, unsigned int> totalStrideCountTable;
        std::map<std::string, unsigned int> strideCountTable;

        amDeviceType processStrideBasedSpeedAndDistanceSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processStrideBasedSpeedAndDistanceSensorSemiCooked( const char *inputBuffer );

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

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendStrideSpeedDistSensor( const std::string & );


    public:

        antStrideSpeedDistProcessing();
        ~antStrideSpeedDistProcessing() {}

        bool isStrideSpeedSensor( const std::string & );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        virtual void reset( void );
};

#endif // __ANT_STRIDE_SPEED_DIST__PROCESSING_H__

