#ifndef __ANT_CADENCE_PROCESSING_H__
#define __ANT_CADENCE_PROCESSING_H__

#include <map>

class amString;
class amSplitString;

class antCadenceProcessing
{

    private:

        std::map<amString, unsigned int> cadenceTable;
        std::map<amString, bool>         cadenceSensorTable;


    protected:

        unsigned int getCadence( const amString &sensorID );
        void setCadence( const amString &sensorID, unsigned int value );

        unsigned int computeCadence( unsigned int previousCadence, unsigned int deltaRevolutionCount, unsigned int deltaEventTime );

        virtual void reset( void );
        bool evaluateDeviceLine( const amSplitString &words );

    public:

        antCadenceProcessing();
        ~antCadenceProcessing() {}

        bool isCadenceSensor    ( const amString &deviceID );
        bool isRegisteredSensor ( const amString &deviceID );
        bool appendCadenceSensor( const amString &deviceID );
};

#endif // __ANT_CADENCE_PROCESSING_H__

