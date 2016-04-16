#ifndef __ANT_CADENCE_PROCESSING_H__
#define __ANT_CADENCE_PROCESSING_H__

#include <map>
#include <string>

class amSplitString;

class antCadenceProcessing
{

    private:

        std::map<std::string, unsigned int> cadenceTable;
        std::map<std::string, bool>         cadenceSensorTable;


    protected:

        unsigned int getCadence( const std::string &sensorID );
        void setCadence( const std::string &sensorID, unsigned int value );

        unsigned int computeCadence( unsigned int previousCadence, unsigned int deltaRevolutionCount, unsigned int deltaEventTime );

        void reset( void );
        bool evaluateDeviceLine( const amSplitString &words );

    public:

        antCadenceProcessing();
        ~antCadenceProcessing() {}

        bool isCadenceSensor( const std::string &deviceID );
        bool isRegisteredSensor( const std::string &deviceID );
        bool appendCadenceSensor( const std::string &deviceID );
};

#endif // __ANT_CADENCE_PROCESSING_H__

