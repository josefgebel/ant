#include <sstream>
#include <iomanip>
#include <iostream>

#include "am_string.h"

amString::amString
(
    void
) : std::string()
{
}

amString::amString
(
    const std::string &text
) : std::string( text )
{
}

amString::amString
(
    const char *text
) : std::string( text )
{
}

amString::amString
(
    int number
) : std::string()
{
    std::stringstream auxStream;
    auxStream << number;
    assign( auxStream.str() );
}

amString::amString
(
    unsigned int number
) : std::string()
{
    std::stringstream auxStream;
    auxStream << number;
    assign( auxStream.str() );
}

amString::amString
(
    unsigned char number
) : std::string()
{
    std::stringstream auxStream;
    auxStream << "0x" << std::uppercase << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number;
    assign( auxStream.str() );
}

amString::amString
(
    unsigned char number1,
    unsigned char number2
) : std::string()
{
    std::stringstream auxStream;
    auxStream << "0x" << std::uppercase << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number1 << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number2;
    assign( auxStream.str() );
}

amString::amString
(
    unsigned char number1,
    unsigned char number2,
    unsigned char number3
) : std::string()
{
    std::stringstream auxStream;
    auxStream << "0x" << std::uppercase << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number1;
    auxStream << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number2;
    auxStream << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number3;
    assign( auxStream.str() );
}

amString::amString
(
    unsigned char number1,
    unsigned char number2,
    unsigned char number3,
    unsigned char number4
) : std::string()
{
    std::stringstream auxStream;
    auxStream << "0x" << std::uppercase << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number1;
    auxStream << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number2;
    auxStream << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number3;
    auxStream << std::setfill( '0' ) << std::setw( 4 ) << std::hex << number4;
    assign( auxStream.str() );
}

amString::amString
(
    double number,
    size_t precision
) : std::string()
{
    std::stringstream auxStream;
    auxStream.precision( precision );
    auxStream << std::fixed << number;
    assign( auxStream.str() );
}

amString::amString
(
    char text
) : std::string()
{
    push_back( text );
}

bool amString::contains
(
    char testChar
) const
{
    std::string::const_iterator tPtr = begin();
    for ( ; ( tPtr != end() ) && ( *tPtr != testChar ); ++tPtr );
    return ( tPtr != end() );
}

bool amString::contains
(
    const amString &text
) const
{
    return ( find( text ) != std::string::npos );
}

bool amString::startsWith
(
    const std::string &prefix
) const
{
    return ( compare( 0, prefix.length(), prefix ) == 0 );
}

bool amString::startsWith
(
    char prefix
) const
{
    return ( at( 0 ) == prefix );
}

void amString::removeFromString
(
    char obsoleteChar
)
{
    for ( std::string::iterator tPtr = begin(); tPtr != end(); ++tPtr )
    {
        if ( *tPtr == obsoleteChar )
        {
            erase( tPtr );
        }
    }
}

unsigned int amString::toHex
(
    void
) const
{
    unsigned int result = 0;
    if ( isHex() )
    {
        std::string::const_iterator tPtr = begin();
        tPtr += 2;   // "0x"
        for ( ; tPtr != end(); ++tPtr )
        {
            result *= 16;
            if ( ( *tPtr >= '0' ) || ( *tPtr <= '9' ) )
            {
                result += ( *tPtr - '0' );
            }
            else if ( ( *tPtr >= 'A' ) || ( *tPtr <= 'F' ) )
            {
                result += ( *tPtr - 'A' + 10 );
            }
            else
            {
                result += ( *tPtr - 'a' + 10 );
            }
        }
    }
    return result;
}

int amString::toInt
(
    void
) const
{
    int result = 0;
    if ( isInteger() )
    {
        result = atoi( c_str() );
    }
    return result;
}

unsigned int amString::toUInt
(
    void
) const
{
    unsigned int result = 0;
    if ( isUnsignedInteger() )
    {
        result = ( unsigned int ) atoi( c_str() );
    }
    return result;
}

double amString::toDouble
(
    void
) const
{
    double result = 0;
    if ( isDouble() )
    {
        result = atof( c_str() );
    }
    return result;
}

bool amString::isUnsignedInteger
(
    void
) const
{
    std::string::const_iterator tPtr   = begin();
    bool                        result = isUnsignedInteger( tPtr );
    return result;
}
bool amString::isUnsignedInteger
(
    std::string::const_iterator &tPtr
) const
{
    bool result = false;
    if ( tPtr != end() )
    {
        for ( ; ( tPtr != end() ); ++tPtr )
        {
            if ( ( *tPtr >= '0' ) && ( *tPtr <= '9' ) )
            {
                result = true;
            }
            else
            {
                break;
            }
        }
    }
    return result;
}

bool amString::isInteger
(
    void
) const
{
    std::string::const_iterator tPtr   = begin();
    bool                        result = isInteger( tPtr );
    result &= ( tPtr == end() );
    return result;
}

bool amString::isInteger
(
    std::string::const_iterator &tPtr
) const
{
    bool result = false;
    if ( tPtr != end() )
    {
        if ( ( *tPtr == '-' ) || ( *tPtr == '+' ) )
        {
             ++tPtr;
        }
        if ( tPtr != end() )
        {
            result = isUnsignedInteger( tPtr );
        }
    }
    return result;
}

bool amString::isDouble
(
    void
) const
{
    bool                        result = false;
    std::string::const_iterator tPtr   = begin();
    result = isDouble( tPtr );
    if ( result && ( tPtr != end() ) && ( ( toupper( *tPtr ) == 'E' ) || ( toupper( *tPtr ) == 'G' ) ) )
    {
        ++tPtr;
        result = isInteger( tPtr );
    }
    result &= ( tPtr == end() );
    return result;
}

bool amString::isDouble
(
    std::string::const_iterator &tPtr
) const
{
    bool result = isInteger( tPtr );
    if ( result && ( tPtr != end() ) )
    {
        if ( *tPtr == '.' )
        {
            ++tPtr;
            if ( tPtr != end() )
            {
                result = isUnsignedInteger( tPtr );
            }
        }
    }
    return result;
}

bool amString::isHex
(
    void
) const
{
    bool                        result = false;
    std::string::const_iterator tPtr   = begin();
    if ( ( tPtr != end() ) && ( length() <= 10 ) ) // 0xFEDCBA98
    {
        if ( *tPtr == '0' )
        {
            ++tPtr;
            if ( tPtr != end() )
            {
                if ( ( *tPtr == 'X' ) || ( *tPtr == 'x' ) )
                {
                    ++tPtr;
                    if ( tPtr != end() )
                    {
                        result = true;
                        for ( ; result && ( tPtr != end() ); ++tPtr )
                        {
                            result = ( ( ( *tPtr >= '0' ) && ( *tPtr <= '9' ) ) || ( ( *tPtr >= 'A' ) && ( *tPtr <= 'F' ) ) || ( ( *tPtr >= 'a' ) && ( *tPtr <= 'f' ) ) );
                        }
                    }
                }
            }
        }
    }
    return result;
}

void amString::toUpper
(
    void
)
{
    std::transform( begin(), end(), begin(), ::toupper );
}

void amString::toLower
(
    void
)
{
    std::transform( begin(), end(), begin(), ::tolower );
}

amString toHex
(
    unsigned char value
)
{
    std::stringstream auxStream;
    auxStream << std::uppercase << std::setfill( '0' ) << std::setw( 4 ) << std::hex << value;
    return amString( auxStream.str() );
}

amString time2String
(
    int hour,
    int minute,
    int second
)
{
    std::stringstream auxStream;
    if ( hour < 10 )
    {
        auxStream << "0";
    }
    auxStream << hour;
    auxStream << ":";
    if ( minute < 10 )
    {
        auxStream << "0";
    }
    auxStream << minute;
    auxStream << ":";
    if ( second < 10 )
    {
        auxStream << "0";
    }
    auxStream << second;

    return amString( auxStream.str() );
}

amString date2String
(
    int year,
    int month,
    int day
)
{
    std::stringstream auxStream;
    if ( year < 10 )
    {
        auxStream << "0";
    }
    auxStream << year;
    auxStream << "-";
    if ( month < 10 )
    {
        auxStream << "0";
    }
    auxStream << month;
    auxStream << "-";
    if ( day < 10 )
    {
        auxStream << "0";
    }
    auxStream << day;

    return amString( auxStream.str() );
}

