#ifndef __AM_MULTI_CAST_READ_H__
#define __AM_MULTI_CAST_READ_H__

class amString;

class amMulticastRead
{
    public:

        amMulticastRead( void );
        ~amMulticastRead( void ) {};

        inline bool isUp( void ) const { return connectionUp; }
        void close( void );

        size_t read( unsigned char *buffer, size_t bufferSize );
        int connect( const amString &interface, const amString &ipAddress, int portNo, int timeOutSec, amString &errorMessage );


    private:

        int  socketID;
        bool connectionUp;

        bool determineIPAddress( amString &ipAddress, const amString &interface );

};

#endif // __AM_MULTI_CAST_READ_H__

