#ifndef __ANT_SPCAD_PROCESSING_H__
#define __ANT_SPCAD_PROCESSING_H__

#include <string>
#include <vector>

#include "ant_constants.h"
#include "ant_processing.h"
#include "ant_speed_processing.h"
#include "ant_cadence_processing.h"

class antSpcadProcessing : virtual public antProcessing,
                           virtual public antSpeedProcessing,
                           virtual public antCadenceProcessing
{

    private:

        std::map<std::string, unsigned int> operatingTimeTable;
        std::map<std::string, unsigned int> cadenceTimeTable;
        std::map<std::string, unsigned int> cadenceCountTable;

        amDeviceType processSpeedAndCadenceSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processSpeedAndCadenceSensorSemiCooked( const char *inputBuffer );

        void createSPCADResultString
             (
                 double       &speed,
                 unsigned int &cadence,
                 unsigned int  deltaSpeedEventTime,
                 unsigned int  deltaWheelRevolutionCount,
                 unsigned int  deltaCadenceEventTime,
                 unsigned int  deltaCrankRevolutionCount,
                 double        wheelCircumference,
                 unsigned int  numberOfMagnets,
                 unsigned int &zeroCountTime
             );

    protected:

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool appendSpeedSensor( const std::string &sensorID, double wheelCircumference, double nbMagnets );


    public:

        antSpcadProcessing();
        ~antSpcadProcessing() {}

        bool isSpeedAndCadenceSensor( const std::string &sensorID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        virtual void reset( void );

};

#endif // __ANT_SPCAD_PROCESSING_H__

