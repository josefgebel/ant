#ifndef __ANT_AUDIO_PROCESSING_H__
#define __ANT_AUDIO_PROCESSING_H__

#include "ant_processing.h"

class amSplitString;

class antAudioProcessing : virtual public antProcessing
{
    protected:

        amDeviceType processAudioControl( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processAudioControlSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendAudioSensor( const std::string & );


    public:

        antAudioProcessing();
        ~antAudioProcessing() {}

        bool isAudioSensor( const std::string &deviceID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        amDeviceType createAudioControlString( unsigned int dataPage, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4 );

};

#endif // __ANT_AUDIO_PROCESSING_H__

