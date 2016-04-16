#ifndef __ANT_POWER_PROCESSING_H__
#define __ANT_POWER_PROCESSING_H__

#include "ant_constants.h"
#include "ant_processing.h"
#include "ant_cadence_speed_processing.h"

class amSplitString;

class antPowerProcessing : virtual public antProcessing,
                           virtual public antCadenceSpeedProcessing
{

    private:

        double       gearRatioDefault;
        unsigned int offsetDefault;
        unsigned int slopeDefault;

        std::map<std::string, unsigned int> slope_Nm_10Hz_Table;
        std::map<std::string, unsigned int> offsetTable;
        std::map<std::string, unsigned int> accumulatedPowerTable;
        std::map<std::string, unsigned int> accumulatedTorqueTable;
        std::map<std::string, unsigned int> crankOrWheelPeriodTable;
        std::map<std::string, unsigned int> otherEventCountTable;
        std::map<std::string, unsigned int> otherAccumulatedPowerTable;
        std::map<std::string, unsigned int> otherTotalCountTable;
        std::map<std::string, unsigned int> otherAccumulatedTorqueTable;
        std::map<std::string, unsigned int> otherCrankOrWheelPeriodTable;
        std::map<std::string, double>       powerTable;
        std::map<std::string, double>       torqueTable;
        std::map<std::string, bool>         speedSensorTable;

        unsigned int maxZeroTimeB10;
        unsigned int maxZeroTimeB11;
        unsigned int maxZeroTimeB12;
        unsigned int maxZeroTimeB20;

        bool isLeftCrankEvent( const std::string &sensorID, unsigned int newValue, unsigned int rollOver );

        unsigned int splitFormat137_B01( const char *inputBuffer, amSplitString &outWords );

        bool createB01ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB01( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB01SemiCooked( const char *inputBuffer );

        amDeviceType processPowerMeterB02( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB02SemiCooked( const char *inputBuffer );
        bool createB02ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB03( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB03SemiCooked( const char *inputBuffer );
        bool createB03ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB10( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB10SemiCooked( const char *inputBuffer );

        amDeviceType processPowerMeterB11( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB11SemiCooked( const char *inputBuffer );

        amDeviceType processPowerMeterB12( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB12SemiCooked( const char *inputBuffer );

        amDeviceType processPowerMeterB13( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB13SemiCooked( const char *inputBuffer );
        bool createB13ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB20( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB20SemiCooked( const char *inputBuffer );

        amDeviceType processPowerMeterB46( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB46SemiCooked( const char *inputBuffer );
        bool createB46ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB50( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB50SemiCooked( const char *inputBuffer );
        bool createB50ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB51( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB51SemiCooked( const char *inputBuffer );
        bool createB51ResultString( const amSplitString &words );


        amDeviceType processPowerMeterB52( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterB52SemiCooked( const char *inputBuffer );
        bool createB52ResultString( const amSplitString &words );
        unsigned int splitFormat137_B52( const char *inputBuffer, amSplitString &outWords );

        void createPMCalibrationResultString016( unsigned int ctfDefinedID, unsigned int messageValue );
        void createPMCalibrationResultString018( int autoZeroEnable, int autoZeroStatus );
        void createPMCalibrationResultString170( void );
        void createPMCalibrationResultString171( unsigned int autoZeroStatus );
        void createPMCalibrationResultString172( int autoZeroStatus, int messageValue );
        void createPMCalibrationResultString175( int autoZeroStatus, int messageValue );
        void createPMCalibrationResultString186( void );
        void createPMCalibrationResultString187( void );
        void createPMCalibrationResultString188( void );
        void createPMCalibrationResultString189( void );


    protected:

        inline void resetGearRatioDefault( void ) { gearRatioDefault = C_GEAR_RATIO_DEFAULT; }
        inline void resetSlopeDefault    ( void ) { slopeDefault     = C_SLOPE_DEFAULT; }
        inline void resetOffsetDefault   ( void ) { offsetDefault    = C_OFFSET_DEFAULT; }

        double getPower( const std::string &sensorID );
        double getTorque( const std::string &sensorID );
        void setPower( const std::string &sensorID, double value );
        void setTorque( const std::string &sensorID, double value );

        unsigned int getOffset( const std::string &sensorID );
        unsigned int getSlope_Nm_10Hz( const std::string &sensorID );

        amDeviceType processPowerMeter( const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        amDeviceType processPowerMeterSemiCooked( const char *inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );

        bool appendPowerSensor( const std::string &sensorID );
        bool appendPowerSensor( const std::string &sensorID, unsigned int offset, unsigned int slope );
        bool appendSpeedSensor( const std::string &sensorID, double wheelCirumference );
        bool appendPowerSpeedSensor( const std::string &sensorID, double wheelCirumference, double gearRatio );


    public:

        antPowerProcessing();
        ~antPowerProcessing() {}

        bool isPowerOnlySensor( const std::string &deviceID );
        bool isWheelTorquePowerSensor( const std::string &deviceID );
        bool isCrankTorquePowerSensor( const std::string &deviceID );
        bool isCrankTorqueFrequencyPowerSensor( const std::string &deviceID );
        bool isPowerSensor( const std::string &deviceID );
        bool isPowerMeterRelated( const std::string &deviceID );
        bool isPowerMeterCalibration( const std::string &deviceID );
        bool isPowerMeterGetSetParameters( const std::string &deviceID );
        bool isPowerMeterMeasurementOutput( const std::string &deviceID );
        bool isPowerMeterPedalSmoothness( const std::string &deviceID );
        bool isPowerMeterRequest( const std::string &deviceID );
        bool isPowerMeterManufacturerInfo( const std::string &deviceID );
        bool isPowerMeterProductInfo( const std::string &deviceID );
        bool isPowerMeterStatusMessage( const std::string &deviceID );

        inline void setMaxZeroTimeB10( unsigned int value ) { maxZeroTimeB10 = value; }
        inline void setMaxZeroTimeB11( unsigned int value ) { maxZeroTimeB11 = value; }
        inline void setMaxZeroTimeB12( unsigned int value ) { maxZeroTimeB12 = value; }
        inline void setMaxZeroTimeB20( unsigned int value ) { maxZeroTimeB20 = value; }

        virtual amDeviceType processSensor( int deviceType, const std::string &deviceIDNo, const std::string &timeStampBuffer, unsigned char payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const char *inputBuffer );

        void reset( void );
        inline void setGearRatioDefault( unsigned int value ) { gearRatioDefault = value; }
        inline void setSlopeDefault    ( unsigned int value ) { slopeDefault     = value; }
        inline void setOffsetDefault   ( double       value ) { offsetDefault    = value; }

        void createPWRB01ResultString( unsigned int calibrationID, unsigned int additinalData1, unsigned int additinalData2 );
        void createPWRB02ResultString( unsigned int subPageNumber, unsigned int additionalData1, unsigned int additionalData2, unsigned int additionalData3 );
        void createPWRB03ResultString
             (
                 unsigned int additionalData1,
                 unsigned int additionalData2,
                 unsigned int additionalData3,
                 unsigned int additionalData4,
                 unsigned int additionalData5,
                 double       additionalDoubleData
             );

        void createPWRB10ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 unsigned int &zeroTimeCount,
                 unsigned int  eventCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaAccumulatedPower,
                 unsigned int  instantaneousPower,
                 unsigned int  instantaneousCadence,
                 unsigned int  pedalPower
             );
        void createPWRB10ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 double       &speed,
                 unsigned int &zeroTimeCount,
                 unsigned int  eventCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaAccumulatedPower,
                 unsigned int  instantaneousPower,
                 unsigned int  instantaneousCadence,
                 unsigned int  pedalPower,
                 double        wheelCircumference,
                 double        gearRatio
             );

        void createPWRB11ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 double       &torque,
                 double       &speed,
                 unsigned int &zeroTimeCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaWheelPeriod,
                 unsigned int  deltaAccumulatedTorque,
                 unsigned int  instantaneousCadence,
                 unsigned int  wheelTicks,
                 double        wheelCircumference
             );

        void createPWRB12ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 double       &torque,
                 unsigned int &zeroTimeCount,
                 unsigned int  eventCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaCrankPeriod,
                 unsigned int  deltaAccumulatedTorque,
                 unsigned int  instantaneousCadence,
                 unsigned int  crankTicks
             );
        void createPWRB12ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 double       &torque,
                 double       &speed,
                 unsigned int &zeroTimeCount,
                 unsigned int  eventCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaCrankPeriod,
                 unsigned int  deltaAccumulatedTorque,
                 unsigned int  instantaneousCadence,
                 unsigned int  crankTicks,
                 double        wheelCircumference,
                 double        gearRatio
             );

        void createPWRB13ResultString
             (
                 unsigned int rawLeftTorqueEffectiveness,
                 unsigned int rawRightTorqueEffectiveness,
                 unsigned int rawLeftPedalSmoothness,
                 unsigned int rawRightPedalSmoothness,
                 unsigned int deltaEventCount,
                 unsigned int totalEventCount
             );

        void createPWRB20ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 double       &torque,
                 unsigned int &zeroTimeCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaTimeStamp,
                 unsigned int  deltaTorqueTicks,
                 unsigned int  factorySlope_Nm_10Hz,
                 unsigned int  powerMeterOffset,
                 unsigned int  userDefinedSlope_Nm_10Hz
             );
        void createPWRB20ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 double       &torque,
                 double       &speed,
                 unsigned int &zeroTimeCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaTimeStamp,
                 unsigned int  deltaTorqueTicks,
                 unsigned int  factorySlope_Nm_10Hz,
                 unsigned int  powerMeterOffset,
                 unsigned int  userDefinedSlope_Nm_10Hz,
                 double        wheelCircumference,
                 double        gearRatio
             );

};

#endif // __ANT_POWER_PROCESSING_H__

