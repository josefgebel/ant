#ifndef __ANT_SPEED_PROCESSING_H__
#define __ANT_SPEED_PROCESSING_H__

#include <map>
#include "ant_constants.h"

class amString;
class amSplitString;

class antSpeedProcessing
{

    private:

        void resetNbMagnetsDefault         ( void ) { nbMagnetsDefault          = C_NB_MAGNETS_DEFAULT; }
        void resetWheelCircumferenceDefault( void ) { wheelCircumferenceDefault = C_WHEEL_CIRCUMFERENCE_DEFAULT; }

        std::map<amString, double> nbMagnetsTable;
        std::map<amString, double> wheelCircumferenceTable;
        std::map<amString, double> speedTable;
        std::map<amString, bool>   usedAsSpeedSensorTable;


    protected:

        double nbMagnetsDefault;
        double wheelCircumferenceDefault;

        unsigned int maxZeroTime;

        double getSpeed             ( const amString &sensorID );
        double getWheelCircumference( const amString &sensorID );
        double getNbMagnets         ( const amString &sensorID );
        void setSpeed             ( const amString &sensorID, double value );
        void setWheelCircumference( const amString &sensorID, double value );
        void setNbMagnets         ( const amString &sensorID, double value );

        bool isPureSpeedSensor( const amString &sensorID );
        bool isMakeshiftSpeedSensor( const amString &sensorID );
        bool isSpeedSensor( const amString &sensorID );

        bool appendSpeedSensor( const amString &sensorID, double wheelCirumference, double nbMagnets );

        virtual int readDeviceFileLine1( const char *line, amString &errorMessage );

        inline void setMaxZeroTime( unsigned int value ) { maxZeroTime = value; }

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

        virtual void reset( void );

        void setUseAsSpeedSensor( const amString &sensorID, bool value );
        bool isUsedAsSpeedSensor( const amString &sensorID );

    public:

        antSpeedProcessing();
        ~antSpeedProcessing() {}

        void setNbMagnetsDefault         ( double value ) { nbMagnetsDefault          = value; }
        void setWheelCircumferenceDefault( double value ) { wheelCircumferenceDefault = value; }
};

#endif // __ANT_SPEED_PROCESSING_H__

