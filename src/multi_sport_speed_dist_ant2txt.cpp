#include "ant_multi_sport_speed_dist_processing.h"

const char C_PROGRAM_NAME[]  = "multi_sport_speed_dist_ant2txt";
const char C_DEVICE_NAME[]   = "MSSDM";
const char C_OPTION_STRING[] = "12BDd:f:HhI:Jl:M:m:P:p:RrsST:t:UvVwx";

int main
(
    int   argc,
    char *argv[]
)
{
    bool                    running   = true;
    antMultiSportProcessing antProcessor;

    running = antProcessor.processArguments( C_PROGRAM_NAME, C_OPTION_STRING, C_DEVICE_NAME, argc, argv );
    if ( running && ( antProcessor.getErrorCode() == 0 ) )
    {
        antProcessor.ant2txt();
    }
    antProcessor.outputError();
    return antProcessor.getErrorCode();
}

