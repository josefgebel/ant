#ifndef __B2T_UTILS_H__
#define __B2T_UTILS_H__

#include <math.h>

#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#include "ant_constants.h"

extern void outputError( const char *programName, int errorCode, const char *argument );
extern void setInterface( char *interface, amOperatingSystem currentOS );

extern int  hex( unsigned char b4, unsigned char b3, unsigned char b2, unsigned char b1 );
extern int  hex( unsigned char b3, unsigned char b2, unsigned char b1 );
extern int  hex( unsigned char b2, unsigned char b1 );
extern int  hex( unsigned char b1 );
extern bool isWhiteChar( char ccc );
extern bool isInteger( const char *text );
extern bool isInteger( const std::string &text );
extern bool isPositiveInteger( const char *text );
extern bool isPositiveInteger( const std::string &text );
extern bool isDigit( char ccc );
extern void replaceChar( char *, char, char );

extern double scaleValue( int value, int scaleFactor );
extern double getUnixTime( void );

extern int connectMulticastWrite( const char *ipAddress, int portNo, struct sockaddr_in &groupSock, int &errorCode, char *errorMessage );
extern int sendMulticast( int socketID, struct sockaddr_in &groupSock, const char *message, int messageLength );
extern int connectMulticastRead( const char *interface, const char *ipAddress, int portNo, int timeOutSec, int &errorCode, char *errorMessage );
extern int negateBinaryInt( int number, int binaryPlaces );
extern int hexDigit2Int( char ccc );
extern void mSleep( unsigned int milliSec );
extern bool startsWith( const char        *text, const char        *pattern, bool ignoreCase = false );
extern bool startsWith( const char        *text, const std::string &pattern, bool ignoreCase = false );
extern bool startsWith( const std::string &text, const char        *pattern, bool ignoreCase = false );
extern bool startsWith( const std::string &text, const std::string &pattern, bool ignoreCase = false );
extern bool isInteger( const char *text );
extern bool isInteger( const std::string &text );
extern bool isPositiveInteger( const char *text );
extern bool isPositiveInteger( const std::string &text );
extern bool isDigit( char ccc );

extern std::string toUpper( const std::string input );
extern double strToDouble( const std::string buffer );
extern int strToInt( const std::string buffer );

// ----------
// Cygwin

#endif // __B2T_UTILS_H__

