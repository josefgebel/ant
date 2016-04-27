#ifndef __ANT_SPEED_ONLY_PROCESSING_H__
#define __ANT_SPEED_ONLY_PROCESSING_H__

#include "ant_processing.h"
#include "ant_speed_processing.h"


class antSpeedOnlyProcessing : virtual public antProcessing,
                               virtual public antSpeedProcessing
{

    private:

        unsigned int splitFormat137_SPB7( const amString &inputBuffer, amSplitString &outWords );
        void replaceObsoleteHeader( amString &sensorID );


    protected:

        amDeviceType processBikeSpeedSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processBikeSpeedSensorSemiCooked( const amString &inputBuffer );

        void createSPB7ResultString
             (
                 double       &speed,
                 unsigned int  dataPage,
                 unsigned int  deltaWheelRevolutionCount,
                 unsigned int  deltaBikeSpeedEventTime,
                 unsigned int  additionalData1,
                 unsigned int  additionalData2,
                 unsigned int  additionalData3,
                 double        wheelCircumference,
                 unsigned int  numberOfMagnets,
                 unsigned int &zeroTime
             );

        bool appendSpeedSensor( const amString &sensorID, double wheelCircumference, double nbMagnets );

        virtual void readDeviceFileLine( const char *line );


    public:

        antSpeedOnlyProcessing();
        ~antSpeedOnlyProcessing() {}

        bool isSpeedOnlySensor( const amString &sensorID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        virtual void reset( void );

};

#endif // __ANT_SPEED_ONLY_PROCESSING_H__

