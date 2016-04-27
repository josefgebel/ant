#ifndef __ANT_CADENCE_SPEED_PROCESSING_H__
#define __ANT_CADENCE_SPEED_PROCESSING_H__

#include "ant_speed_processing.h"
#include "ant_cadence_processing.h"

class antCadenceSpeedProcessing : virtual public antSpeedProcessing,
                                  virtual public antCadenceProcessing
{

    private:

        double gearRatioDefault;
        std::map<amString, bool> speedCadenceSensorTable;


    protected:

        bool   isMakeshiftSpeedSensor( const amString &deviceID );
        bool   appendCadenceSpeedSensor( const amString &sensorID, double wheelCirumference, double gearRatio );
        double computeSpeed( unsigned int cadence, double wheelCircumference, double gearRatio );

        virtual int readDeviceFileLine1( const char *line, amString &errorMessage );

        virtual void reset( void );


    public:

        antCadenceSpeedProcessing();
        ~antCadenceSpeedProcessing() {}

        void setGearRatioDefault  ( double value ) { gearRatioDefault = value; }
        void resetGearRatioDefault( void ) { gearRatioDefault = C_GEAR_RATIO_DEFAULT; }

};

#endif // __ANT_CADENCE_SPEED_PROCESSING_H__

