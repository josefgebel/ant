#include "ant_spcad_processing.h"

const char C_PROGRAM_NAME[]  = "spcad_ant2txt";
const char C_DEVICE_NAME[]   = "SPCAD";
const char C_OPTION_STRING[] = "12BDd:f:HhI:Jl:M:m:P:p:RrsST:t:UvVx";

int main
(
    int   argc,
    char *argv[]
)
{
    bool               running   = true;
    antSpcadProcessing antProcessor;

    running = antProcessor.processArguments( C_PROGRAM_NAME, C_OPTION_STRING, C_DEVICE_NAME, argc, argv );
    if ( running && ( antProcessor.getErrorCode() == 0 ) )
    {
        antProcessor.ant2txt();
    }
    antProcessor.outputError();
    return antProcessor.getErrorCode();
}

