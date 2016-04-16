#include "ant_constants.h"
#include "am_split_string.h"

amSplitString::amSplitString
(
    void
) : curWordNo( 0 )
{
    separators.push_back( '\t' );
    separators.push_back( ' ' );
    terminals.push_back( '\0' );
    terminals.push_back( '\n' );
    terminals.push_back( '\r' );
}

amSplitString::amSplitString
(
    const char *text
) : curWordNo( 0 )
{
    separators.push_back( '\t' );
    separators.push_back( ' ' );
    terminals.push_back( '\0' );
    terminals.push_back( '\n' );
    terminals.push_back( '\r' );
    split( text );
}

bool amSplitString::isSeparator
(
    char testChar
) const
{
    for ( std::vector<char>::const_iterator sPtr = separators.begin(); sPtr != separators.end(); ++sPtr )
    {
        if ( *sPtr == testChar )
        {
            return true;
        }
    }
    return false;
}

bool amSplitString::isTerminal
(
    char testChar
) const
{
    for ( std::vector<char>::const_iterator tPtr = terminals.begin(); tPtr != terminals.end(); ++tPtr )
    {
        if ( *tPtr == testChar )
        {
            return true;
        }
    }
    return false;
}

bool amSplitString::isAdditionalTerminal
(
    char        testChar,
    const char *additionalTermnials
) const
{
    if ( additionalTermnials != NULL )
    {
        const char *aPtr = additionalTermnials;
        while ( ( *aPtr != 0 ) && ( *aPtr != testChar ) )
        {
            ++aPtr;
        }
        return ( *aPtr == testChar );
    }
    return false;
}

void amSplitString::addSeparator
(
    char newSeparator
)
{
    if ( !isSeparator( newSeparator ) )
    {
        separators.push_back( newSeparator );
    }
}

bool amSplitString::isAdditionalSeparator
(
    char        testChar,
    const char *additionalSeparators
) const
{
    if ( additionalSeparators != NULL )
    {
        const char *aPtr = additionalSeparators;
        while ( ( *aPtr != 0 ) && ( *aPtr != testChar ) )
        {
            ++aPtr;
        }
        return ( *aPtr == testChar );
    }
    return false;
}

void amSplitString::addTerminal
(
    char newTerminal
)
{
    if ( !isTerminal( newTerminal ) )
    {
        terminals.push_back( newTerminal );
    }
}

void amSplitString::removeSeparator
(
    char obsoleteSeparator
)
{
    for ( std::vector<char>::iterator sPtr = separators.begin(); sPtr != separators.end(); ++sPtr )
    {
        if ( *sPtr == obsoleteSeparator )
        {
            separators.erase( sPtr );
        }
    }
}

void amSplitString::removeTerminal
(
    char obsoleteTerminal
)
{
    for ( std::vector<char>::iterator tPtr = terminals.begin(); tPtr != terminals.end(); ++tPtr )
    {
        if ( *tPtr == obsoleteTerminal )
        {
            terminals.erase( tPtr );
        }
    }
}

size_t amSplitString::split
(
    const char *text,
    const char *additionalTermnials,
    const char *additionalSeparators
)
{
    char        auxBuffer[ C_BUFFER_SIZE ];
    char       *aPtr = auxBuffer;
    const char *tPtr = text;
    memset( auxBuffer, 0, C_BUFFER_SIZE );

    words.clear();
    curWordNo = 0;
    *aPtr     = 0;

    while ( !isTerminal( *tPtr ) && !isAdditionalTerminal( *tPtr, additionalTermnials ) )
    {
        if ( isSeparator( *tPtr ) || isAdditionalSeparator( *tPtr, additionalSeparators ) )
        {
            if ( strlen( auxBuffer ) )
            {
                words.push_back( auxBuffer );
                aPtr = auxBuffer;
                memset( auxBuffer, 0, C_BUFFER_SIZE );
            }
        }
        else
        {
            *aPtr = *tPtr;
            ++aPtr;
            *aPtr = 0;
        }
        ++tPtr;
    }
    if ( strlen( auxBuffer ) )
    {
        words.push_back( auxBuffer );
    }

    return words.size();
}

size_t amSplitString::split
(
    const std::string &text,
    const char        *additionalTermnials,
    const char        *additionalSeparators
)
{
    size_t result = split( text.c_str(), additionalTermnials, additionalSeparators );
    return result;
}

void amSplitString::push_back
(
    const std::string &newWord
)
{
    words.push_back( newWord );
}

const std::string amSplitString::getWord
(
    unsigned int wordNo
) const
{
    return wordNo < words.size() ? words[ wordNo ] : std::string( "" );
}

const std::string amSplitString::operator[]
(
    unsigned int wordNo
) const
{
    return wordNo < words.size() ? words[ wordNo ] : std::string( "" );
}

const std::string amSplitString::front
(
    void
) const
{
    return words.size() ? words.front() : std::string( "" );
}

const std::string amSplitString::back
(
    void
) const
{
    return words.size() ? words.back() : std::string( "" );
}

const std::string amSplitString::getNextWord
(
    void
)
{
    if ( curWordNo < words.size() )
    {
        return words[ curWordNo++ ];
    }
    return std::string( "" );
}

