#ifndef __ANT_CADENCE_ONLY_PROCESSING_H__
#define __ANT_CADENCE_ONLY_PROCESSING_H__

#include "ant_processing.h"
#include "ant_cadence_speed_processing.h"

class amSplitString;

class antCadenceOnlyProcessing : virtual public antProcessing,
                                 virtual public antCadenceSpeedProcessing
{

    private:

        void createCadenceResultString
             (
                 unsigned int &cadence,
                 unsigned int  dataPage,
                 unsigned int  deltaRevolutionCount,
                 unsigned int  deltaEventTime,
                 unsigned int  additionalData1,
                 unsigned int  additionalData2,
                 unsigned int  additionalData3
             );

        void createCadenceResultString
             (
                 unsigned int &cadence,
                 unsigned int  dataPage,
                 unsigned int  deltaRevolutionCount,
                 unsigned int  deltaEventTime,
                 unsigned int  additionalData1,
                 unsigned int  additionalData2,
                 unsigned int  additionalData3,
                 double       &speed,
                 double        wheelCircumference,
                 double        gearRatio
             );

        unsigned int splitFormat137_CAD7A( const char *inputBuffer, amSplitString &outWords );


    protected:

        amDeviceType processCadenceOnlySensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processCadenceOnlySensorSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );

        bool appendCadenceOnlySensor( const std::string &sensorID );

        void reset( void );


    public:

        antCadenceOnlyProcessing();
        ~antCadenceOnlyProcessing() {}

        bool isCadenceOnlySensor( const std::string &deviceID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

};

#endif // __ANT_CADENCE_ONLY_PROCESSING_H__

