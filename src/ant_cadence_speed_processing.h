#ifndef __ANT_CADENCE_SPEED_PROCESSING_H__
#define __ANT_CADENCE_SPEED_PROCESSING_H__

#include "ant_speed_processing.h"
#include "ant_cadence_processing.h"

class antCadenceSpeedProcessing : virtual public antSpeedProcessing,
                                  virtual public antCadenceProcessing
{

    protected:

        double gearRatioDefault;

        std::map<amString, bool> speedCadenceSensorTable;

        bool   appendCadenceSpeedSensor( const amString &sensorID, double wheelCirumference, double gearRatio );
        double computeSpeed( unsigned int cadence, double wheelCircumference, double gearRatio );
        bool   evaluateDeviceLine( const amSplitString &words );

        virtual void reset( void );


    public:

        antCadenceSpeedProcessing();
        ~antCadenceSpeedProcessing() {}

        void setGearRatioDefault  ( double value ) { gearRatioDefault = value; }
        void resetGearRatioDefault( void ) { gearRatioDefault = C_GEAR_RATIO_DEFAULT; }

        void setUseAsSpeedSensor( const amString &sensorID, bool value );
        bool isUsedAsSpeedSensor( const amString &sensorID );
};

#endif // __ANT_CADENCE_SPEED_PROCESSING_H__

