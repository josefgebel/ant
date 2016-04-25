#ifndef __ANT_POWER_PROCESSING_H__
#define __ANT_POWER_PROCESSING_H__

#include "ant_processing.h"
#include "ant_cadence_speed_processing.h"

class antPowerProcessing : virtual public antProcessing,
                           virtual public antCadenceSpeedProcessing
{

    private:

        double       gearRatioDefault;
        unsigned int offsetDefault;
        unsigned int slopeDefault;

        std::map<amString, unsigned int> slope_Nm_10Hz_Table;
        std::map<amString, unsigned int> offsetTable;
        std::map<amString, unsigned int> accumulatedPowerTable;
        std::map<amString, unsigned int> accumulatedTorqueTable;
        std::map<amString, unsigned int> crankOrWheelPeriodTable;
        std::map<amString, unsigned int> otherEventCountTable;
        std::map<amString, unsigned int> otherAccumulatedPowerTable;
        std::map<amString, unsigned int> otherTotalCountTable;
        std::map<amString, unsigned int> otherAccumulatedTorqueTable;
        std::map<amString, unsigned int> otherCrankOrWheelPeriodTable;
        std::map<amString, double>       powerTable;
        std::map<amString, double>       torqueTable;
        std::map<amString, bool>         speedSensorTable;

        unsigned int maxZeroTimeB10;
        unsigned int maxZeroTimeB11;
        unsigned int maxZeroTimeB12;
        unsigned int maxZeroTimeB20;

        bool isLeftCrankEvent( const amString &sensorID, unsigned int newValue, unsigned int rollOver );

        unsigned int splitFormat137_B01( const amString &inputBuffer, amSplitString &outWords );

        bool createB01ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB01( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB01SemiCooked( const amString &inputBuffer );

        amDeviceType processPowerMeterB02( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB02SemiCooked( const amString &inputBuffer );
        bool createB02ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB03( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB03SemiCooked( const amString &inputBuffer );
        bool createB03ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB10( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB10SemiCooked( const amString &inputBuffer );

        amDeviceType processPowerMeterB11( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB11SemiCooked( const amString &inputBuffer );

        amDeviceType processPowerMeterB12( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB12SemiCooked( const amString &inputBuffer );

        amDeviceType processPowerMeterB13( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB13SemiCooked( const amString &inputBuffer );
        bool createB13ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB20( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB20SemiCooked( const amString &inputBuffer );

        amDeviceType processPowerMeterB46( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB46SemiCooked( const amString &inputBuffer );
        bool createB46ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB50( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB50SemiCooked( const amString &inputBuffer );
        bool createB50ResultString( const amSplitString &words );

        amDeviceType processPowerMeterB51( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB51SemiCooked( const amString &inputBuffer );
        bool createB51ResultString( const amSplitString &words );


        amDeviceType processPowerMeterB52( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterB52SemiCooked( const amString &inputBuffer );
        bool createB52ResultString( const amSplitString &words );
        unsigned int splitFormat137_B52( const amString &inputBuffer, amSplitString &outWords );

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

        double scaleValue( int value, int scaleFactor );


    protected:

        inline void resetGearRatioDefault( void ) { gearRatioDefault = C_GEAR_RATIO_DEFAULT; }
        inline void resetSlopeDefault    ( void ) { slopeDefault     = C_SLOPE_DEFAULT; }
        inline void resetOffsetDefault   ( void ) { offsetDefault    = C_OFFSET_DEFAULT; }

        double getPower ( const amString &sensorID );
        double getTorque( const amString &sensorID );
        void setPower ( const amString &sensorID, double value );
        void setTorque( const amString &sensorID, double value );

        unsigned int getOffset( const amString &sensorID );
        unsigned int getSlope_Nm_10Hz( const amString &sensorID );

        amDeviceType processPowerMeter( const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processPowerMeterSemiCooked( const amString &inputBuffer );

        virtual int readDeviceFileStream( std::ifstream &deviceFileStream );
        bool evaluateDeviceLine( const amSplitString &words );

        bool appendPowerSensor( const amString &sensorID );
        bool appendPowerSensor( const amString &sensorID, unsigned int offset, unsigned int slope );
        bool appendSpeedSensor( const amString &sensorID, double wheelCirumference );
        bool appendPowerSpeedSensor( const amString &sensorID, double wheelCirumference, double gearRatio );


    public:

        antPowerProcessing();
        ~antPowerProcessing() {}

        bool isPowerOnlySensor( const amString &deviceID );
        bool isWheelTorquePowerSensor( const amString &deviceID );
        bool isCrankTorquePowerSensor( const amString &deviceID );
        bool isCrankTorqueFrequencyPowerSensor( const amString &deviceID );
        bool isPowerSensor( const amString &deviceID );
        bool isPowerMeterRelated( const amString &deviceID );
        bool isPowerMeterCalibration( const amString &deviceID );
        bool isPowerMeterGetSetParameters( const amString &deviceID );
        bool isPowerMeterMeasurementOutput( const amString &deviceID );
        bool isPowerMeterPedalSmoothness( const amString &deviceID );
        bool isPowerMeterRequest( const amString &deviceID );
        bool isPowerMeterManufacturerInfo( const amString &deviceID );
        bool isPowerMeterProductInfo( const amString &deviceID );
        bool isPowerMeterStatusMessage( const amString &deviceID );

        inline void setMaxZeroTimeB10( unsigned int value ) { maxZeroTimeB10 = value; }
        inline void setMaxZeroTimeB11( unsigned int value ) { maxZeroTimeB11 = value; }
        inline void setMaxZeroTimeB12( unsigned int value ) { maxZeroTimeB12 = value; }
        inline void setMaxZeroTimeB20( unsigned int value ) { maxZeroTimeB20 = value; }

        virtual amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        virtual amDeviceType processSensorSemiCooked( const amString &inputBuffer );

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
                 unsigned int  pedalPower,
                 bool          isMakeshiftSpeedSensor,
                 double       &speed,
                 double        wheelCircumference,
                 double        gearRatio
             );

        void createPWRB11ResultString
             (
                 double       &power,
                 unsigned int &cadence,
                 double       &torque,
                 unsigned int &zeroTimeCount,
                 unsigned int  deltaEventCount,
                 unsigned int  deltaWheelPeriod,
                 unsigned int  deltaAccumulatedTorque,
                 unsigned int  instantaneousCadence,
                 unsigned int  wheelTicks,
                 bool          isSpeedSensor,
                 double       &speed,
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
                 unsigned int  crankTicks,
                 bool          isMakeshiftSpeedSensor,
                 double       &speed,
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
                 unsigned int  userDefinedSlope_Nm_10Hz,
                 bool          isMakeshiftSpeedSensor,
                 double       &speed,
                 double        wheelCircumference,
                 double        gearRatio
             );

        virtual void reset( void );
};

#endif // __ANT_POWER_PROCESSING_H__

