#ifndef __ANT_SPCAD_PROCESSING_H__
#define __ANT_SPCAD_PROCESSING_H__

#include "ant_processing.h"
#include "ant_speed_processing.h"
#include "ant_cadence_processing.h"

class antSpcadProcessing : virtual public antProcessing,
                           virtual public antSpeedProcessing,
                           virtual public antCadenceProcessing
{

    private:

        std::map<amString, unsigned int> operatingTimeTable;
        std::map<amString, unsigned int> cadenceTimeTable;
        std::map<amString, unsigned int> cadenceCountTable;

        amDeviceType processSpeedAndCadenceSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processSpeedAndCadenceSensorSemiCooked( const amString &inputBuffer );

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
        bool appendSpeedSensor( const amString &sensorID, double wheelCircumference, double nbMagnets );


    public:

        antSpcadProcessing();
        ~antSpcadProcessing() {}

        bool isSpeedAndCadenceSensor( const amString &sensorID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        virtual void reset( void );

};

#endif // __ANT_SPCAD_PROCESSING_H__

