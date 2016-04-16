// Common C libraries

#include <sys/time.h>
#include <sys/utsname.h>

#ifdef __MINGW32__
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif


// Common C++ libraries
#include <iostream>
#include <fstream>
#include <iomanip>

#include "b2t_utils.h"


static bool determineIPAddress
(
    char       *ipAddress,
    const char *interface
);


static bool determineIPAddress
(
    char       *ipAddress,
    const char *interface
)
{
    bool success = false;
    char szBuffer[ C_BUFFER_SIZE ];

    if ( gethostname( szBuffer, sizeof( szBuffer ) ) == 0 )
    {
        struct hostent *host = gethostbyname( szBuffer );
        if ( host != NULL )
        {
            if ( ( interface == NULL ) || ( *interface == 0 ) )
            {
                sprintf( ipAddress, "%s", inet_ntoa( *( ( struct in_addr * ) host->h_addr ) ) );
                int ii = 0;
                while ( ( strncmp( ipAddress, "127.0.", strlen( "127.0." ) ) == 0 ) && ( host->h_addr_list[ ii ] != NULL ) )
                {
                    sprintf( ipAddress, "%s", inet_ntoa( *( ( struct in_addr * ) host->h_addr_list[ ii ] ) ) );
                    ++ii;
                }
            }
            else
            {
                strcpy( ipAddress, "127.0.0.1" );
            }

            if ( strncmp( ipAddress, "127.0.", strlen( "127.0." ) ) == 0 )
            {
                if ( ( interface == NULL ) || ( *interface == 0 ) )
                {
                    strcpy( szBuffer, "ifconfig | grep \"inet \"" );
                }
                else
                {
                    sprintf( szBuffer, "ifconfig %s | grep \"inet \"", interface );
                }
                FILE *pPtr = popen( szBuffer, "r" );
                success = false;
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
                            strcpy( ipAddress, bPtr );
                            bPtr = ipAddress;
                            while ( ( *bPtr != 0 ) && ( *bPtr != ' ' ) )
                            {
                                ++bPtr;
                            }
                            if ( *bPtr != 0 )
                            {
                                *bPtr = 0;
                            }
                            success = true;
                        }
                        if ( strncmp( ipAddress, "127.0.", strlen( "127.0." ) ) != 0 )
                        {
                            break;
                        }
                    }
                    pclose( pPtr );
                }
            }
        }
    }
    return success;
}


// ==================================================================================================================================

void outputError
(
    const char *programName,
    int         errorCode,
    const char *argument
)
{
    char errorMessage[ C_BUFFER_SIZE ];

    switch ( errorCode )
    {
        case 0:
             strcpy( errorMessage, "exited without error" );
             break;
        case E_MC_WRITE_FAIL:
             strcpy( errorMessage, "Sending message on multicast failed" );
             break;
        case E_UNKNOWN_MC_CONNECT_FAILURE:
             strcpy( errorMessage, "Unknown error while creating multicast write connection" );
             break;
        case E_MC_OUT_IP_ADD_BUT_NO_PORT:
             strcpy( errorMessage, "IP address for multicast output is present, port number is missing" );
             break;
        case E_BAD_OPTION:
             sprintf( errorMessage, "Bad option '%s'", argument );
             break;
        case E_UNKNOWN_OPTION:
             sprintf( errorMessage, "Unknown option '%s'", argument );
             break;
        case E_SOCKET_CREATE_FAIL:
             strcpy( errorMessage, "Creating socket option failed" );
             break;
        case E_SOCKET_SET_OPT_FAIL:
             strcpy( errorMessage, "Setting of socket option failed" );
             break;
        case E_NO_IP_ADDRESS_IF:
             strcpy( errorMessage, "No IP Address on Interface" );
             break;
        case E_LOOP_BACK_IP_ADDRESS:
             strcpy( errorMessage, "IP Address is loopback address" );
             break;
        case E_SOCKET_BIND_FAIL:
             strcpy( errorMessage, "Binding the socket failed" );
             break;
        case E_READ_ERROR:
             strcpy( errorMessage, "Error while reading" );
             break;
        case E_READ_FILE_NOT_OPEN:
             strcpy( errorMessage, "Could not open file for reading" );
             break;
        case E_NO_INTER_FACE:
             strcpy( errorMessage, "Could not find an interface" );
             break;
        case E_READ_TIMEOUT:
             strcpy( errorMessage, "Read source timed out" );
             break;
#       ifdef __MINGW32__
        case E_WSA_STARTUP:
             strcpy( errorMessage, "Can't WSAStartup" );
             break;
#       endif
        default:
             sprintf( errorMessage, "Unknown error code %d", errorCode );
             break;
    }

    std::cerr << programName;
    std::cerr << ": " << errorMessage;
    if ( errorCode != 0 )
    {
        std::cerr << " (Error Code = " << errorCode << ")";
    }
    std::cerr << "." << std::endl;
}

double getUnixTime
(
    void
)
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    double result = ( double ) tv.tv_sec + ( ( double ) tv.tv_usec ) / 1.0E6;

    return result;
}

bool isWhiteChar
(
    char ccc
)
{
    return ( ccc == ' ' ) || ( ccc == '\t' );
}

int hexDigit2Int
(
    char ccc
)
{
    int result = -1;
    switch ( ccc )
    {
        case 'F':
        case 'E':
        case 'D':
        case 'C':
        case 'B':
        case 'A': result = 10 + ( int ) ( ccc - 'A' );
                  break;
        case 'f':
        case 'e':
        case 'd':
        case 'c':
        case 'b':
        case 'a': result = 10 + ( int ) ( ccc - 'a' );
                  break;
        case '9':
        case '8':
        case '7':
        case '6':
        case '5':
        case '4':
        case '3':
        case '2':
        case '1':
        case '0': result = ( int ) ( ccc - '0' );
                  break;
        default : break;
    }
    return result;
}

int hex
(
    unsigned char b4,
    unsigned char b3,
    unsigned char b2,
    unsigned char b1
)
{
    int res = hex( b4 );
    res <<= 24;
    res += hex( b3, b2, b1 );
    return res;
}

int hex
(
    unsigned char b3,
    unsigned char b2,
    unsigned char b1
)
{
    int res = hex( b3 );
    res <<= 16;
    res += hex( b2, b1 );
    return res;
}

int hex
(
    unsigned char b2,
    unsigned char b1
)
{
    int res = hex( b2 );
    res <<= 8;
    res += hex( b1 );
    return res;
}

int hex
(
    unsigned char b1
)
{
    int res = ( unsigned int ) b1;
    return res;
}

double scaleValue
(
    int value,
    int scaleFactor
)
{
    double result = 0;

    if ( scaleFactor >= 0 )
    {
        result = ( double ) value;
        if ( scaleFactor > 0 )
        {
            result *= pow( 2.0, scaleFactor );
        }
    }
    else
    {
        double resultFraction = 0;
        int    nbBits         = -scaleFactor;
        int    intValue       = ( value >> nbBits );
        int    fractionValue  = ( value - ( intValue << nbBits ) );

        result = ( double ) intValue;
        while ( fractionValue )
        {
            if ( fractionValue & 1 )
            {
                ++resultFraction;
            }
            resultFraction /= 2.0;
            fractionValue >>= 1;
        }
        result += resultFraction;
    }

    return result;
}


// -------------------------------------------------------------------------------------------

void replaceChar
(
    char *theString,
    char  original,
    char  replacement
)
{
    char *sPtr = theString;
    while ( *sPtr != 0 )
    {
        if ( *sPtr == original )
        {
            *sPtr = replacement;
        }
        ++sPtr;
    }
}

void setInterface
(
    char              *interface,
    amOperatingSystem  currentOS
)
{
    switch ( currentOS )
    {
         case MAC_OSX:
              strcpy( interface, C_INTERFACE_MACOSX );
              break;
         case LINUX:
              strcpy( interface, C_INTERFACE_LINUX );
              break;
         case CYGWIN:
              strcpy( interface, C_INTERFACE_CYGWIN );
              break;
         default:
              *interface = 0;
              break;
    }
}

int sendMulticast
(
    int                 socketID,
    struct sockaddr_in &groupSock,
    const char         *message,
    int                 messageLength
)
{
    int errorCode = 0;
    if ( sendto( socketID, message, messageLength, 0, ( struct sockaddr * ) &groupSock, sizeof( groupSock ) ) < 0 )
    {
        errorCode = E_MC_WRITE_FAIL;
    }
    return errorCode;
}

void mSleep
(
    unsigned int milliSec
)
{
    struct timespec time;
    time.tv_sec = milliSec / 1000;
    time.tv_nsec = ( milliSec % 1000 ) * 1000000;
    nanosleep( &time, NULL );
}

bool startsWith
(
    const char *text,
    const char *pattern,
    bool        ignoreCase
)
{
    bool result = false;
    if ( ( text != NULL ) && ( pattern != NULL ) && strlen( pattern ) )
    {
        result = ignoreCase ? ( strncasecmp( text, pattern, strlen( pattern ) ) == 0 )
                            : ( strncmp    ( text, pattern, strlen( pattern ) ) == 0 );
    }
    return result;
}

bool startsWith
(
    const std::string &text,
    const char        *pattern,
    bool               ignoreCase
)
{
    bool result = startsWith( text.c_str(), pattern, ignoreCase );
    return result;
}

bool startsWith
(
    const char        *text,
    const std::string &pattern,
    bool               ignoreCase
)
{
    bool result = startsWith( text, pattern.c_str(), ignoreCase );
    return result;
}

bool startsWith
(
    const std::string &text,
    const std::string &pattern,
    bool               ignoreCase
)
{
    bool result = startsWith( text.c_str(), pattern.c_str(), ignoreCase );
    return result;
}

int negateBinaryInt
(
   int number,
   int binaryPlaces
)
{
    int result = number;
    if ( result >= ( 1 << ( binaryPlaces - 1 ) ) )
    {
        result -= ( 1 << binaryPlaces );
    }
    return result;
}

bool isInteger
(
    const char *text
)
{
    bool result = ( *text != 0 );

    if ( result )
    {
        if ( ( *text == '-' ) || ( *text == '+' ) )
        {
            result = isPositiveInteger( text + 1 );
        }
        else
        {
            result = isPositiveInteger( text );
        }
    }

    return result;
}

bool isInteger
(
    const std::string &text
)
{
    bool result = isInteger( text.c_str() );
    return result;
}

bool isPositiveInteger
(
    const char *text
)
{
    bool result = ( *text != 0 );

    if ( result )
    {
        const char *bPtr = text;
        while ( result && ( *bPtr != 0 ) )
        {
            result = isDigit( *bPtr );
            ++bPtr;
        }
    }
    return result;
}

bool isPositiveInteger
(
    const std::string &text
)
{
    bool result = isPositiveInteger( text.c_str() );
    return result;
}

bool isDigit
(
    char ccc
)
{
    return ( ccc >= '0' ) && ( ccc <= '9' );
}

std::string toUpper
(
    const std::string input
)
{
    std::string output( input );
    for ( int counter = 0; counter < input.length(); ++counter )
    {
        output[ counter ] = ::toupper( input[ counter ] );
    }
    return output;
}

double strToDouble
(
    const std::string buffer
)
{
    return atof( buffer.c_str() );
}

int strToInt
(
    const std::string buffer
)
{
    return atoi( buffer.c_str() );
}


// ----------
// Cygwin
#if defined __CYGWIN__

int connectMulticastRead
(
    const char *interface,
    const char *ipAddress,
    int         portNo,
    int         timeOutSec,
    int        &errorCode,
    char       *errorMessage
)
{
    struct sockaddr_in sender_addr;
    int                sender_addr_size = sizeof(sender_addr);
#   ifdef __MINGW32__
    WSADATA wsadata;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &wsadata ) == SOCKET_ERROR )
    {
        std::cerr << "connectMulticastRead: Can't WSAStartup." << std::endl;
        errorCode = E_WSA_STARTUP;
    }
#   endif

    // Create a datagram socket on which to receive.
    int sockedID = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( socketID < 0 )
    {
        errorCode = E_SOCKET_CREATE_FAIL;
        sprintf( errorMessage, "connectMulticastWrite: Opening datagram socket error." );
    }

    if ( errroCode == 0 )
    {
        // Enable SO_REUSEADDR to allow multiple instances of this
        // application to receive copies of the multicast datagrams.
        int reuse = 1;
        int resSetSockopt = setsockopt( socketID, SOL_SOCKET, SO_REUSEPORT, ( char * ) &reuse, sizeof( reuse ) );
        if ( resSetSockopt < 0 )
        {
            errorCode = E_SOCKET_SET_OPT_FAIL;
            strerror_r( errno, errorMessage, C_BUFFER_SIZE );

        }
    }

    if ( errroCode == 0 )
    {
        // Bind to the proper port number with the IP address specified as INADDR_ANY. */
        bzero( ( void * ) &localSock, sizeof( localSock ) );

        localSock.sin_family = AF_INET;
        localSock.sin_port = htons( portNo );
        localSock.sin_addr.s_addr = INADDR_ANY;

        int bindResult = bind( socketID, ( struct sockaddr * ) &localSock, sizeof( localSock ) );
        if ( bindResult )
        {
            errorCode = E_SOCKET_BIND_FAIL;
            strerror_r( errno, errorMessage, C_BUFFER_SIZE );
        }
    }

    if ( errorCode == 0 )
    {
        group.imr_multiaddr.s_addr = inet_addr( ipAddress );
        group.imr_interface.s_addr = INADDR_ANY;
        //group.imr_interface.s_addr = inet_addr( localIPAddress );

        int resSetSockopt = setsockopt( socketID, IPPROTO_IP, IP_ADD_MEMBERSHIP, ( char * ) &group, sizeof( group ) );
        if( resSetSockopt < 0 )
        {
            errorCode = E_SOCKET_SET_OPT_FAIL;
            strerror_r( errno, errorMessage, C_BUFFER_SIZE );
        }
    }

    if ( errorCode == 0 )
    {
        if ( setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &group, sizeof( group ) ) < 0 )
        {
            errorCode = E_ADD_MULTICAST_GROUP;
            close(sd);
            exit(1);
        }
    }
    return errorCode;
}


int connectMulticastWrite
(
    const char         *ipAddress,
    int                 portNo,
    struct sockaddr_in &groupSock,
    int                &errorCode,
    char               *errorMessage
)
{
;
}
//#elif defined __LINUX__
----------
// Linux
#else // __CYGWIN__
//----------
// Mac
int connectMulticastWrite
(
    const char         *ipAddress,
    int                 portNo,
    struct sockaddr_in &groupSock,
    int                &errorCode,
    char               *errorMessage
)
{
    int socketID = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( socketID < 0 )
    {
        errorCode = E_SOCKET_CREATE_FAIL;
        sprintf( errorMessage, "connectMulticastWrite: Opening datagram socket error." );
    }

    bzero( ( char * ) &groupSock, sizeof( groupSock ) );
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = htonl( INADDR_ANY );
    groupSock.sin_port = htons( portNo );
    groupSock.sin_addr.s_addr = inet_addr( ipAddress );

    return socketID;
}

int connectMulticastRead
(
    const char *interface,
    const char *ipAddress,
    int         portNo,
    int         timeOutSec,
    int        &errorCode,
    char       *errorMessage
)
{
    int socketID = -1;

    if ( ( interface != NULL ) && ( *interface != 0 ) && ( ipAddress != NULL ) && ( *ipAddress != 0 ) )
    {
        struct  sockaddr_in  localSock;
        struct  ip_mreq      group;
        char                 localIPAddress[ C_SMALL_BUFFER_SIZE ];

        // Create a datagram socket on which to receive.
        socketID = socket( AF_INET, SOCK_DGRAM, 0 );
        if( socketID < 0 )
        {
            errorCode = E_SOCKET_CREATE_FAIL;
            strerror_r( errno, errorMessage, C_BUFFER_SIZE );
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
                errorCode = E_SOCKET_SET_OPT_FAIL;
                strerror_r( errno, errorMessage, C_BUFFER_SIZE );
            }
        }

        if ( timeOutSec > 0 )
        {
            struct timeval tv;

            tv.tv_sec  = timeOutSec;
            tv.tv_usec = 0;  // Not init'ing this can cause strange errors

            int resSetSockopt = setsockopt( socketID, SOL_SOCKET, SO_RCVTIMEO, ( char * ) &tv, sizeof( struct timeval ) );
            if ( resSetSockopt < 0 )
            {
                errorCode = E_SOCKET_SET_OPT_FAIL;
                strerror_r( errno, errorMessage, C_BUFFER_SIZE );
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
                errorCode = E_SOCKET_BIND_FAIL;
                strerror_r( errno, errorMessage, C_BUFFER_SIZE );
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
                socketID = -socketID;
                errorCode = E_NO_IP_ADDRESS_IF;
                strcpy( errorMessage, "No IP Address on interface \"" );
                strcat( errorMessage, interface );
                strcat( errorMessage, "\"." );
            }
        }


        if ( errorCode == 0 )
        {
            // Check if the IP Address is not the loopback address
            if ( strcmp( localIPAddress, "127.0.0.1" ) == 0 )
            {
                errorCode = E_LOOP_BACK_IP_ADDRESS;
                strcpy( errorMessage, "IP Address on interface \"" );
                strcat( errorMessage, interface );
                strcat( errorMessage, "\" is loopback address." );
            }
        }


        if ( errorCode == 0 )
        {
            group.imr_multiaddr.s_addr = inet_addr( ipAddress );
            group.imr_interface.s_addr = inet_addr( localIPAddress );

            int resSetSockopt = setsockopt( socketID, IPPROTO_IP, IP_ADD_MEMBERSHIP, ( char * ) &group, sizeof( group ) );
            if( resSetSockopt < 0 )
            {
                errorCode = E_SOCKET_SET_OPT_FAIL;
                strerror_r( errno, errorMessage, C_BUFFER_SIZE );
            }
        }
    }

    return socketID;
}
#endif // __CYGWIN__

