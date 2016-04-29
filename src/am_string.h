#ifndef __AM_STRING_H__
#define __AM_STRING_H__

#define BYTE unsigned char

#include <string>

const size_t C_DBL_RECISION = 6;

class amString : public std::string
{

    private:

        bool isDouble( std::string::const_iterator &tPtr ) const;
        bool isInteger( std::string::const_iterator &tPtr ) const;
        bool isUnsignedInteger( std::string::const_iterator &tPtr ) const;


    public:

        amString( void );
        amString( const std::string & );
        amString( const char *string );
        explicit amString( char character );
        explicit amString( int number );
        explicit amString( unsigned int number );
        explicit amString( BYTE number );
        explicit amString(BYTE number1, BYTE number2 ); 
        explicit amString(BYTE number1, BYTE number2, BYTE number3 ); 
        explicit amString(BYTE number1, BYTE number2, BYTE number3, BYTE number4 );
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

