#ifndef __ANT_SPEED_PROCESSING_H__
#define __ANT_SPEED_PROCESSING_H__

#include <map>
#include <string>

#include "ant_constants.h"

class amSplitString;

class antSpeedProcessing
{

    private:

        void resetNbMagnetsDefault         ( void ) { nbMagnetsDefault          = C_NB_MAGNETS_DEFAULT; }
        void resetWheelCircumferenceDefault( void ) { wheelCircumferenceDefault = C_WHEEL_CIRCUMFERENCE_DEFAULT; }


    protected:

        double nbMagnetsDefault;
        double wheelCircumferenceDefault;

        std::map<std::string, double> nbMagnetsTable;
        std::map<std::string, double> wheelCircumferenceTable;
        std::map<std::string, double> speedTable;

        double getSpeed             ( const std::string &sensorID );
        double getWheelCircumference( const std::string &sensorID );
        double getNbMagnets         ( const std::string &sensorID );
        void setSpeed             ( const std::string &sensorID, double value );
        void setWheelCircumference( const std::string &sensorID, double value );
        void setNbMagnets         ( const std::string &sensorID, double value );

        bool isPureSpeedSensor( const std::string &sensorID );
        bool isMakeshiftSpeedSensor( const std::string &sensorID );
        bool isSpeedSensor( const std::string &sensorID );

        bool appendSpeedSensor( const std::string &sensorID, double wheelCirumference, double nbMagnets );
        bool evaluateDeviceLine( const amSplitString &words );
        double computeSpeed
               (
                   double        previousSpeed,
                   unsigned int  deltaWheelRevolutionCount,
                   unsigned int  deltaBikeSpeedEventTime,
                   double        wheelCircumference,
                   unsigned int  numberOfMagnets,
                   unsigned int &zeroTime,
                   unsigned int  maxZeroTime
               );


    public:

        antSpeedProcessing();
        ~antSpeedProcessing() {}

        void setNbMagnetsDefault         ( double value ) { nbMagnetsDefault          = value; }
        void setWheelCircumferenceDefault( double value ) { wheelCircumferenceDefault = value; }

        void reset( void );
};

#endif // __ANT_SPEED_PROCESSING_H__

