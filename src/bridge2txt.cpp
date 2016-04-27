#include "ant_all_processing.h"

const char C_PROGRAM_NAME[]  = "bridge2txt";
const char C_DEVICE_NAME[]   = "?";
const char C_OPTION_STRING[] = "12BDd:f:H:hI:Jl:M:m:P:p:RrsST:t:UvVwx";

int main
(
    int   argc,
    char *argv[]
)
{
    bool             running   = true;
    antAllProcessing antProcessor;

    running = antProcessor.processArguments( C_PROGRAM_NAME, C_OPTION_STRING, C_DEVICE_NAME, argc, argv );
    if ( running && ( antProcessor.getErrorCode() == 0 ) )
    {
        antProcessor.ant2txt();
    }
    antProcessor.outputError();
    return antProcessor.getErrorCode();
}

