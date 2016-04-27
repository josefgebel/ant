#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>

#include "am_string.h"
#include "ant_constants.h"
#include "am_multicast_read.h"

const amString C_LOOP_BACK_START( "127.0." );
const amString C_LOOP_BACK_ADDRESS( "127.0.0.1" );

amMulticastRead::amMulticastRead
(
    void
)
{
    connectionUp = false;
    socketID      = -1;
}


void amMulticastRead::close
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
    }
}

size_t amMulticastRead::read
(
    BYTE   *buffer,
    size_t  bufferSize
)
{
    size_t nbBytes = 0;
    if ( connectionUp && ( socketID > 0 ) && ( bufferSize > 0 ) && ( buffer != NULL ) )
    {
        nbBytes = ::read( socketID, ( void * ) buffer, bufferSize );
    }
    return nbBytes;
}

bool amMulticastRead::determineIPAddress
(
    amString       &ipAddress,
    const amString &interface
)
{
    bool success = false;
    char szBuffer[ C_BUFFER_SIZE ];

    if ( gethostname( szBuffer, sizeof( szBuffer ) ) == 0 )
    {
        struct hostent *host = gethostbyname( szBuffer );
        if ( host != NULL )
        {
            if ( interface.empty() )
            {
                ipAddress = C_LOOP_BACK_ADDRESS;
            }
            else
            {
                ipAddress = inet_ntoa( *( ( struct in_addr * ) host->h_addr ) );
                int ii = 0;
                while ( ipAddress.startsWith( C_LOOP_BACK_START ) && ( host->h_addr_list[ ii ] != NULL ) )
                {
                    ipAddress = inet_ntoa( *( ( struct in_addr * ) host->h_addr_list[ ii ] ) );
                    ++ii;
                }
            }

            if ( ipAddress.startsWith( C_LOOP_BACK_START ) )
            {
                if ( interface.empty() )
                {
                    sprintf( szBuffer, "ifconfig %s | grep \"inet \"", interface.c_str() );
                }
                else
                {
                    strcpy( szBuffer, "ifconfig | grep \"inet \"" );
                }
                FILE *pPtr = popen( szBuffer, "r" );

                if ( pPtr != NULL )
                {
                    while ( fgets( szBuffer, C_BUFFER_SIZE, pPtr ) )
                    {
                        char *bPtr = szBuffer;
                        while ( ( *bPtr != 0 ) && ( *bPtr != 'i' ) )
                        {
                            ++bPtr;
                        }
                        if ( *bPtr == 'i' )
                        {
                            ++bPtr;
                            while ( ( *bPtr != 0 ) && ( *bPtr != ' ' ) )
                            {
                                ++bPtr;
                            }
                            while ( *bPtr == ' ' )
                            {
                                ++bPtr;
                            }
                            while ( ( *bPtr != 0 ) && ( ( *bPtr < '1' ) || ( *bPtr > '9' ) ) )
                            {
                                ++bPtr;
                            }
                            ipAddress = bPtr;

                            size_t count = 0;
                            while ( ( ipAddress[ count ] != 0 ) && ( ipAddress[ count ] != ' ' ) )
                            {
                                ++count;
                            }
                            ipAddress.resize( count );
                            success = true;
                        }
                        if ( ipAddress.startsWith( C_LOOP_BACK_START ) )
                        {
                            break;
                        }
                    }
                    pclose( pPtr );
                }
            }
            else
            {
                success = true;
            }
        }
    }
    return success;
}

int amMulticastRead::connect
(
    const amString &interface,
    const amString &ipAddress,
    int             portNo,
    int             timeOutSec,
    amString       &errorMessage
)
{
    char     auxErrorMessage[ C_BUFFER_SIZE ];
    int      errorCode    = 0;
    struct   sockaddr_in localSock;
    struct   ip_mreq     group;
    amString localIPAddress;

    if ( interface.empty() )
    {
        errorCode     = E_MC_NO_INTERFACE;
        errorMessage += "Creating multicast connection for reading failed. No Interface.\n";
    }
    else if ( ipAddress.empty() )
    {
        errorCode     = E_MC_NO_IP_ADDRESS;
        errorMessage += "Creating multicast connection for reading failed. No IP Address.\n";
    }
    else if ( portNo <= 0 )
    {
        errorCode     = E_MC_NO_PORT_NUMBER;
        errorMessage += "Creating multicast connection for reading failed. No Port Number.\n";
    }
    else
    {
        // Create a datagram socket on which to receive.
        socketID = socket( AF_INET, SOCK_DGRAM, 0 );
        if( socketID < 0 )
        {
            errorCode     = E_SOCKET_CREATE_FAIL;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Creating socket option for multicast reading failed. Opening datagram socket error:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    if ( errorCode == 0 )
    {
        // Enable SO_REUSEPORT to allow multiple instances of this
        // application to receive copies of the multicast datagrams.
        long flags = fcntl( socketID, F_GETFL, 0 );
        fcntl( socketID, F_SETFL, flags );

        int reuse = 1;

        int resSetSockopt = setsockopt( socketID, SOL_SOCKET, SO_REUSEPORT, ( char * ) &reuse, sizeof( reuse ) );
        if ( resSetSockopt < 0 )
        {
            errorCode     = E_SOCKET_SET_OPT_FAIL;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Setting of socket option failed:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    if ( errorCode == 0 )
    {
        if ( timeOutSec > 0 )
        {
            struct timeval tv;

            tv.tv_sec  = timeOutSec;
            tv.tv_usec = 0;  // Not init'ing this can cause strange errors

            int resSetSockopt = setsockopt( socketID, SOL_SOCKET, SO_RCVTIMEO, ( char * ) &tv, sizeof( struct timeval ) );
            if ( resSetSockopt < 0 )
            {
                errorCode     = E_SOCKET_SET_OPT_FAIL;
                strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
                errorMessage += "Setting of socket option failed:\n    ";
                errorMessage += auxErrorMessage;
                errorMessage += ".\n";
            }
        }
    }

    if ( errorCode == 0 )
    {
        // Bind to the proper port number with the IP address
        // specified as INADDR_ANY.
        bzero( ( void * ) &localSock, sizeof( localSock ) );

        localSock.sin_family = AF_INET;
        localSock.sin_port = htons( portNo );
        localSock.sin_addr.s_addr = INADDR_ANY;

        int bindResult = bind( socketID, ( struct sockaddr * ) &localSock, sizeof( localSock ) );
        if ( bindResult )
        {
            errorCode     = E_SOCKET_BIND_FAIL;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Binding the socket failed:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    if ( errorCode == 0 )
    {
        // Join the multicast group 226.1.1.1 on the local 203.106.93.94
        // interface. Note that this IP_ADD_MEMBERSHIP option must be
        // called for each local interface over which the multicast
        // datagrams are to be received.

        int getIPResult = determineIPAddress( localIPAddress, interface );
        if ( !getIPResult )
        {
            errorCode     = E_NO_IP_ADDRESS_IF;
            errorMessage += "No IP Address found on interface \"";
            errorMessage += interface;
            errorMessage += "\".\n";
        }
    }

    if ( errorCode == 0 )
    {
        // Check if the IP Address is not the loopback address
        if ( localIPAddress == C_LOOP_BACK_ADDRESS )
        {
            errorCode     = E_LOOP_BACK_IP_ADDRESS;
            errorMessage += "IP Address on interface \"";
            errorMessage += interface;
            errorMessage += "\" is loopback address.\n";
        }
    }

    if ( errorCode == 0 )
    {
        group.imr_multiaddr.s_addr = inet_addr( ipAddress.c_str() );
        group.imr_interface.s_addr = inet_addr( localIPAddress.c_str() );

        int resSetSockopt = setsockopt( socketID, IPPROTO_IP, IP_ADD_MEMBERSHIP, ( char * ) &group, sizeof( group ) );
        if( resSetSockopt < 0 )
        {
            errorCode     = E_SOCKET_SET_OPT_FAIL;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Setting of socket option failed:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

