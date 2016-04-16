#ifndef __ANT_CADENCE_SPEED_PROCESSING_H__
#define __ANT_CADENCE_SPEED_PROCESSING_H__

#include "ant_speed_processing.h"
#include "ant_cadence_processing.h"

class amSplitString;

class antCadenceSpeedProcessing : virtual public antSpeedProcessing,
                                  virtual public antCadenceProcessing
{

    protected:

        double gearRatioDefault;

        std::map<std::string, bool> speedCadenceSensorTable;

        bool   appendCadenceSpeedSensor( const std::string &sensorID, double wheelCirumference, double gearRatio );
        double computeSpeed( unsigned int cadence, double wheelCircumference, double gearRatio );
        bool   evaluateDeviceLine( const amSplitString &words );


    public:

        antCadenceSpeedProcessing();
        ~antCadenceSpeedProcessing() {}

        void setGearRatioDefault  ( double value ) { gearRatioDefault = value; }
        void resetGearRatioDefault( void ) { gearRatioDefault = C_GEAR_RATIO_DEFAULT; }

        void setUseAsSpeedSensor( const std::string &sensorID, bool value );
        bool isUsedAsSpeedSensor( const std::string &sensorID );

        void reset( void );
};

#endif // __ANT_CADENCE_SPEED_PROCESSING_H__

