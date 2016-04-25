#ifndef __AM_STRING_H__
#define __AM_STRING_H__

#include <string>

const size_t C_DBL_RECISION = 6;

class amString : public std::string
{
    public:

        amString( void );
        amString( const std::string & );
        amString( const char *string );
        explicit amString( char character );
        explicit amString( int number );
        explicit amString( unsigned int number );
        explicit amString( unsigned char number );
        explicit amString(unsigned char number1, unsigned char number2 ); 
        explicit amString(unsigned char number1, unsigned char number2, unsigned char number3 ); 
        explicit amString(unsigned char number1, unsigned char number2, unsigned char number3, unsigned char number4 );
        explicit amString( double number, size_t precision = C_DBL_RECISION );
        virtual ~amString( void ) {};

        bool contains        ( char testChar ) const;
        bool contains        ( const amString &text ) const;
        bool startsWith      ( const std::string &prefix ) const;
        bool startsWith      ( char testChar ) const;
        void removeFromString( char testChar );

        int          toInt( void ) const;
        unsigned int toUInt( void ) const;
        double       toDouble( void ) const;
        unsigned int toHex( void ) const;

        void toUpper( void );
        void toLower( void );

        bool isDouble( void ) const;
        bool isHex( void ) const;
        bool isInteger( void ) const;
        bool isUnsignedInteger( void ) const;

};

amString toHex( unsigned char value );
amString time2String( int hour, int minute, int second );
amString date2String( int year, int month,  int day );

#endif // __AM_STRING_H__

