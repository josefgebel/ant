#ifndef __ANT_ALL_PROCESSING_H__
#define __ANT_ALL_PROCESSING_H__

#include "ant_processing.h"
#include "ant_all_processing.h"
#include "ant_hrm_processing.h"
#include "ant_aero_processing.h"
#include "ant_audio_processing.h"
#include "ant_power_processing.h"
#include "ant_speed_processing.h"
#include "ant_spcad_processing.h"
#include "ant_weight_processing.h"
#include "ant_cadence_processing.h"
#include "ant_speed_only_processing.h"
#include "ant_environment_processing.h"
#include "ant_cadence_only_processing.h"
#include "ant_cadence_speed_processing.h"
#include "ant_blood_pressure_processing.h"
#include "ant_stride_speed_dist_processing.h"
#include "ant_multi_sport_speed_dist_processing.h"


class antAllProcessing : virtual public antAeroProcessing,
                         virtual public antMultiSportProcessing,
                         virtual public antPowerProcessing,
                         virtual public antStrideSpeedDistProcessing,
                         virtual public antAudioProcessing,
                         virtual public antWeightProcessing,
                         virtual public antBloodPressureProcessing,
                         virtual public antEnvironmentProcessing,
                         virtual public antSpcadProcessing,
                         virtual public antCadenceOnlyProcessing,
                         virtual public antHRMProcessing,
                         virtual public antSpeedOnlyProcessing
{
    private:

        amDeviceType processUndefinedSensorType( const amString &inputBuffer );

    public:

        antAllProcessing();
        virtual ~antAllProcessing() {}

        virtual int readDeviceFileStream( std::ifstream & );

        amDeviceType processSensor( int deviceType, const amString &deviceIDNo, const amString &timeStampBuffer, BYTE payLoad[] );
        amDeviceType processSensorSemiCooked( const amString &inputBuffer );

        amDeviceType updateSensorSemiCooked( const amString &inputBuffer );

        virtual void reset( void );

};

#endif // __ANT_ALL_PROCESSING_H__


