#ifndef __ANT_AERO_PROCESSING_H__
#define __ANT_AERO_PROCESSING_H__

#include "ant_processing.h"

class antAeroProcessing : virtual public antProcessing
{

    private:

        double rho;
        double rhoDefault;
        double airSpeedMultiplierDefault;
        double calibrationRhoDefault;

        std::map<amString, double> airSpeedMultiplierTable;
        std::map<amString, double> rhoCalibrationTable;

        void   resetRhoDefault               ( void ) { rhoDefault                = C_RHO_DEFAULT; }
        void   resetCalibrationRhoDefault    ( void ) { calibrationRhoDefault     = C_CALIBRATION_RHO_DEFAULT; }
        void   resetAirSpeedMultiplierDefault( void ) { airSpeedMultiplierDefault = C_AIR_SPEED_MULTIPLIER_DEFAULT; }
        double computeYawAngle( unsigned int yawAngleUnsignedRaw );
        double computeYawAngle( int yawAngleSignedRaw );
        double computeAirSpeed( unsigned int airSpeedUnsignedRaw, double calibrationRho, double airSpeedMultiplier, double currentRho );
        double computeAirSpeed( int airSpeedSignedRaw, double calibrationRho, double airSpeedMultiplier, double currentRho );
        double getCorrectionFactor( const amString &sensorID );


    protected:

        double convertAirspeed( int );
        double convertYaw( int, int );

        amDeviceType processAeroSensor( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processAeroSensorSemiCooked( const amString &inputBuffer );

        double getCalibrationRho( const amString &sensorID );
        double getAirSpeedMultiplier( const amString &sensorID );

        virtual void readDeviceFileLine( const char *line );

        bool appendAeroSensor( const amString &, double = C_CALIBRATION_RHO_DEFAULT, double = C_AIR_SPEED_MULTIPLIER_DEFAULT );


    public:

        antAeroProcessing();
        ~antAeroProcessing() {}

        bool isAeroSensor( const amString &deviceID );

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        void setRho( double value );
        void setRhoDefault( double value ) { rhoDefault = value; }

        bool setCalibrationRho( const amString &sensorID, double value );
        void setCalibrationRhoDefault( double value ) { calibrationRhoDefault = value; }

        bool setAirSpeedMultiplier( const amString &sensorID, double value );
        void setAirSpeedMultiplierDefault( double value ) { airSpeedMultiplierDefault = value; }

        virtual void reset( void );

        void createAEROResultString( unsigned int airSpeedRaw, unsigned int yawAngleRaw, double calibrationRho, double airSpeedMultiplier, double currentRho );

};

#endif // __ANT_AERO_PROCESSING_H__

