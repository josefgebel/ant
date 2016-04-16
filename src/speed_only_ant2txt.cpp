#include <iostream>

#include "b2t_utils.h"
#include "ant_speed_only_processing.h"

const char C_PROGRAM_NAME[]  = "speed_only_ant2txt";
const char C_DEVICE_NAME[]   = "SPB7";
const char C_OPTION_STRING[] = "12BDd:f:HhI:JLl:M:m:P:p:RrsST:t:UvVx";

int main
(
    int   argc,
    char *argv[]
)
{
    bool                   running   = true;
    int                    errorCode = 0;
    antSpeedOnlyProcessing antProcessor;

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

