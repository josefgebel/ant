#ifndef __ANT_AUDIO_PROCESSING_H__
#define __ANT_AUDIO_PROCESSING_H__

#include "ant_processing.h"

class antAudioProcessing : virtual public antProcessing
{

    protected:

        amDeviceType processAudioControl( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processAudioControlSemiCooked( const amString &inputBuffer );

        virtual void readDeviceFileLine( const char *line );

        bool appendAudioSensor( const amString & );


    public:

        antAudioProcessing();
        ~antAudioProcessing() {}

        bool isAudioSensor( const amString &deviceID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        amDeviceType createAudioControlString( unsigned int dataPage, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4 );

        virtual void reset( void );

};

#endif // __ANT_AUDIO_PROCESSING_H__

