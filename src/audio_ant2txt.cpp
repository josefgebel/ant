#include <iostream>

#include "b2t_utils.h"
#include "ant_audio_processing.h"

const char C_PROGRAM_NAME[]  = "audio_ant2txt";
const char C_DEVICE_NAME[]   = "AUDIO";
const char C_OPTION_STRING[] = "12BDd:f:HhI:JLl:M:m:P:p:RrsST:t:UvVx";

int main
(
    int   argc,
    char *argv[]
)
{
    bool               running   = true;
    int                errorCode = 0;
    antAudioProcessing antProcessor;

    running = antProcessor.processInput( C_PROGRAM_NAME, C_OPTION_STRING, C_DEVICE_NAME, argc, argv );

    errorCode = antProcessor.getErrorCode();
    if ( running )
    {
        errorCode = antProcessor.ant2txt();
    }
    if ( errorCode != 0 )
    {
        outputError( C_PROGRAM_NAME, errorCode, antProcessor.getErrorMessage() );
    }
    return errorCode;
}

