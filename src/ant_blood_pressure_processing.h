#ifndef __ANT_BLOOD_PRESSURE_PROCESSING_H__
#define __ANT_BLOOD_PRESSURE_PROCESSING_H__

#include "ant_processing.h"

class antBloodPressureProcessing : virtual public antProcessing
{
    protected:

        amDeviceType processBloodPressureSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processBloodPressureSensorSemiCooked( const amString &inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendBloodPressureSensor( const amString & );


    public:

        antBloodPressureProcessing();
        ~antBloodPressureProcessing() {}

        bool isBloodPressureSensor( const amString &deviceID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        void createBLDPRResultString( unsigned int dataPage );
        virtual void reset( void );

};

#endif // __ANT_BLOOD_PRESSURE_PROCESSING_H__

