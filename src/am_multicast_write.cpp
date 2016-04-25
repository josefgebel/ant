#include <unistd.h>
#include <arpa/inet.h>

#include "am_string.h"
#include "ant_constants.h"
#include "am_multicast_write.h"

amMulticastWrite::amMulticastWrite
(
    void
)
{
    connectionUp = false;
    socketID      = -1;
    bzero( ( char * ) &groupSock, sizeof( groupSock ) );
}


void amMulticastWrite::close
(
    void
)
{
    if ( connectionUp )
    {
        if ( socketID > 0 )
        {
            ::close( socketID );
            socketID = -1;
        }
        connectionUp = false;
        bzero( ( char * ) &groupSock, sizeof( groupSock ) );
    }
}

size_t amMulticastWrite::write
(
    const amString &buffer,
    int             errorCode,
    amString       &errorMessage,
    int             length
)
{
    errorCode = E_EMPTY_MESSAGE;
    if ( connectionUp && ( socketID > 0 ) && !buffer.empty() )
    {
        size_t bufferSize = ( length < 0 ) ? buffer.size() : length;
        if ( sendto( socketID, buffer.c_str(), bufferSize, 0, ( struct sockaddr * ) &groupSock, sizeof( groupSock ) ) < 0 )
        {
            errorCode     = E_MC_WRITE_FAIL;
            errorMessage += "Sending message via multicast failed.\n";
        }
        else
        {
            errorCode = 0;
        }
    }
    return errorCode;
}

int amMulticastWrite::connect
(
    const amString &ipAddress,
    int             portNo,
    amString       &errorMessage
)
{
    int errorCode = 0;

    if ( !ipAddress.empty() )
    {   
        errorCode     = E_MC_NO_IP_ADDRESS;
        errorMessage += "Creating multicast connection for writing failed. No IP Address.\n";
    }   
    else if ( portNo <= 0 ) 
    {   
        errorCode     = E_MC_NO_PORT_NUMBER;
        errorMessage += "Creating multicast connection for writing failed. No Port Number.\n";
    }   
    else
    {   
        socketID = socket( AF_INET, SOCK_DGRAM, 0 );
        if ( socketID < 0 )
        {
            errorCode     = E_SOCKET_CREATE_FAIL;
            errorMessage += "Creating socket option for multicast writing failed. Opening datagram socket error.\n";
        }

        bzero( ( char * ) &groupSock, sizeof( groupSock ) );
        groupSock.sin_family = AF_INET;
        groupSock.sin_addr.s_addr = htonl( INADDR_ANY );
        groupSock.sin_port = htons( portNo );
        groupSock.sin_addr.s_addr = inet_addr( ipAddress.c_str() );
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}


