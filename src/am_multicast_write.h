#ifndef __AM_MULTI_CAST_WRITE_H__
#define __AM_MULTI_CAST_WRITE_H__

#include <netdb.h>

class amString;

class amMulticastWrite
{

    private:

        int  socketID;
        bool connectionUp;

        bool determineIPAddress( amString &ipAddress, const amString &interface );

        struct sockaddr_in groupSock;


    public:

        amMulticastWrite( void );
        ~amMulticastWrite( void ) {};

        inline bool isUp( void ) const { return connectionUp; }

        void   close( void );
        size_t write( const amString &buffer, int errorCode, amString &errorMessage, int length = -1 );
        int connect( const amString &ipAddress, int portNo, amString &errorMessage );

};

#endif // __AM_MULTI_CAST_WRITE_H__

