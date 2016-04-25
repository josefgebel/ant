#include "ant_aero_processing.h"

const char C_PROGRAM_NAME[] = "aero_ant2txt";
const char C_DEVICE_NAME[]  = "AERO";
const char C_OPTION_STRING[] = "12BDd:f:HhI:Jl:M:m:P:p:RrsST:t:UvVx";

int main
(
    int   argc,
    char *argv[]
)
{
    bool              running   = true;
    antAeroProcessing antProcessor;

    running = antProcessor.processArguments( C_PROGRAM_NAME, C_OPTION_STRING, C_DEVICE_NAME, argc, argv );
    if ( running && ( antProcessor.getErrorCode() == 0 ) )
    {
        antProcessor.ant2txt();
    }
    antProcessor.outputError();
    return antProcessor.getErrorCode();
}

