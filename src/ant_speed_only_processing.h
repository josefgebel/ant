#ifndef __ANT_SPEED_ONLY_PROCESSING_H__
#define __ANT_SPEED_ONLY_PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"
#include "ant_speed_processing.h"

class amSplitString;

class antSpeedOnlyProcessing : virtual public antProcessing,
                               virtual public antSpeedProcessing
{

    private:

        unsigned int splitFormat137_SPB7( const char *inputBuffer, amSplitString &outWords );
        void replaceObsoleteHeader( std::string &sensorID );


    protected:

        amDeviceType processBikeSpeedSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processBikeSpeedSensorSemiCooked( const char *inputBuffer );

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

        bool appendSpeedSensor( const std::string &sensorID, double wheelCircumference, double nbMagnets );
        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );


    public:

        antSpeedOnlyProcessing();
        ~antSpeedOnlyProcessing() {}

        bool isSpeedOnlySensor( const std::string &sensorID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        virtual void reset( void );

};

#endif // __ANT_SPEED_ONLY_PROCESSING_H__

