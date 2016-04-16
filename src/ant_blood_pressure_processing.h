#ifndef __ANT_BLOOD_PRESSURE_PROCESSING_H__
#define __ANT_BLOOD_PRESSURE_PROCESSING_H__

#include "ant_processing.h"

class amSplitString;

class antBloodPressureProcessing : virtual public antProcessing
{
    protected:

        amDeviceType processBloodPressureSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processBloodPressureSensorSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendBloodPressureSensor( const std::string & );


    public:

        antBloodPressureProcessing();
        ~antBloodPressureProcessing() {}

        bool isBloodPressureSensor( const std::string &deviceID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        void createBLDPRResultString( unsigned int dataPage );
};

#endif // __ANT_BLOOD_PRESSURE_PROCESSING_H__

