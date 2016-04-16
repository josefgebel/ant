#ifndef __ANT_AERO_PROCESSING_H__
#define __ANT_AERO_PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"

class amSplitString;

class antAeroProcessing : virtual public antProcessing
{

    private:

        void   resetRhoDefault               ( void ) { rhoDefault                = C_RHO_DEFAULT; }
        void   resetCalibrationRhoDefault    ( void ) { calibrationRhoDefault     = C_CALIBRATION_RHO_DEFAULT; }
        void   resetAirSpeedMultiplierDefault( void ) { airSpeedMultiplierDefault = C_AIR_SPEED_MULTIPLIER_DEFAULT; }
        double computeYawAngle( unsigned int yawAngleUnsignedRaw );
        double computeYawAngle( int yawAngleSignedRaw );
        double computeAirSpeed( unsigned int airSpeedUnsignedRaw, double calibrationRho, double airSpeedMultiplier, double currentRho );
        double computeAirSpeed( int airSpeedSignedRaw, double calibrationRho, double airSpeedMultiplier, double currentRho );
        double getCorrectionFactor( const std::string &sensorID );


    protected:

        double rho;
        double rhoDefault;
        double airSpeedMultiplierDefault;
        double calibrationRhoDefault;

        std::map<std::string, double> airSpeedMultiplierTable;
        std::map<std::string, double> rhoCalibrationTable;

        double convertAirspeed( int );
        double convertYaw( int, int );

        amDeviceType processAeroSensor( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processAeroSensorSemiCooked( const char *inputBuffer );

        double getCalibrationRho( const std::string &sensorID );
        double getAirSpeedMultiplier( const std::string &sensorID );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );
        bool appendAeroSensor( const std::string &, double = C_CALIBRATION_RHO_DEFAULT, double = C_AIR_SPEED_MULTIPLIER_DEFAULT );


    public:

        antAeroProcessing();
        ~antAeroProcessing() {}

        bool isAeroSensor( const std::string &deviceID );

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        void setRho( double value );
        void setRhoDefault( double value ) { rhoDefault = value; }

        bool setCalibrationRho( const std::string &sensorID, double value );
        void setCalibrationRhoDefault( double value ) { calibrationRhoDefault = value; }

        bool setAirSpeedMultiplier( const std::string &sensorID, double value );
        void setAirSpeedMultiplierDefault( double value ) { airSpeedMultiplierDefault = value; }

        virtual void reset( void );

        void createAEROResultString( unsigned int airSpeedRaw, unsigned int yawAngleRaw, double calibrationRho, double airSpeedMultiplier, double currentRho );

};

#endif // __ANT_AERO_PROCESSING_H__

