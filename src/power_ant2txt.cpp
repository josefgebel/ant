#include "ant_power_processing.h"

const char C_PROGRAM_NAME[]  = "power_ant2txt";
const char C_DEVICE_NAME[]   = "POWER";
const char C_OPTION_STRING[] = "12BDd:f:H:hI:Jl:M:m:P:p:RrsST:t:UvVwx";

int main
(
    int   argc,
    char *argv[]
)
{
    bool               running   = true;
    antPowerProcessing antProcessor;

    running = antProcessor.processArguments( C_PROGRAM_NAME, C_OPTION_STRING, C_DEVICE_NAME, argc, argv );
    if ( running && ( antProcessor.getErrorCode() == 0 ) )
    {
        antProcessor.ant2txt();
    }
    antProcessor.outputError();
    return antProcessor.getErrorCode();
}

