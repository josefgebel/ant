#ifndef __ANT_CADENCE_ONLY_PROCESSING_H__
#define __ANT_CADENCE_ONLY_PROCESSING_H__

#include "ant_processing.h"
#include "ant_cadence_speed_processing.h"

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
                 unsigned int  additionalData3,
                 bool          isUsedAsSpeedSensor,
                 double       &speed,
                 double        wheelCircumference,
                 double        gearRatio
             );

        unsigned int splitFormat137_CAD7A( const amString &inputBuffer, amSplitString &outWords );


    protected:

        amDeviceType processCadenceOnlySensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processCadenceOnlySensorSemiCooked( const amString &inputBuffer );

        virtual void readDeviceFileLine( const char *line );

        bool appendCadenceOnlySensor( const amString &sensorID );


    public:

        antCadenceOnlyProcessing();
        ~antCadenceOnlyProcessing() {}

        bool isCadenceOnlySensor( const amString &deviceID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        virtual void reset( void );

};

#endif // __ANT_CADENCE_ONLY_PROCESSING_H__

