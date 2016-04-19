#include "ant_constants.h"
#include "am_split_string.h"

amSplitString::amSplitString
(
    void
)
{
    init();
}

amSplitString::amSplitString
(
    const std::string &text
)
{
    init();
    split( text );
}

void amSplitString::init
(
    void
)
{
    curWordNo   = 0;
    separators  = '\t';
    separators += ' ';
    terminals   = '\0';
    terminals  += '\n';
    terminals  += '\r';
}

bool amSplitString::isInString
(
    char               testChar,
    const std::string &text
) const
{
    std::string::const_iterator tPtr = text.begin();
    for ( ; ( tPtr != text.end() ) && ( *tPtr != testChar ); ++tPtr );
    return ( tPtr != text.end() );
}

void amSplitString::removeFromString
(
    char         obsoleteChar,
    std::string &text
)
{
    for ( std::string::iterator tPtr = text.begin(); tPtr != text.end(); ++tPtr )
    {
        if ( *tPtr == obsoleteChar )
        {
            text.erase( tPtr );
        }
    }
}

bool amSplitString::isSeparator
(
    char testChar
) const
{
    return isInString( testChar, separators );
}

bool amSplitString::isTerminal
(
    char testChar
) const
{
    return isInString( testChar, terminals );
}

void amSplitString::addSeparator
(
    char newSeparator
)
{
    if ( !isSeparator( newSeparator ) )
    {
        separators += newSeparator;
    }
}

void amSplitString::addTerminal
(
    char newTerminal
)
{
    if ( !isTerminal( newTerminal ) )
    {
        terminals += newTerminal;
    }
}

void amSplitString::removeSeparator
(
    char obsoleteSeparator
)
{
    removeFromString( obsoleteSeparator, separators );
}

void amSplitString::removeTerminal
(
    char obsoleteTerminal
)
{
    removeFromString( obsoleteTerminal, terminals );
}


size_t amSplitString::split
(
    const std::string &text,
    const std::string &additionalTermnials,
    const std::string &additionalSeparators
)
{
    std::string                 word;
    std::string::const_iterator tPtr = text.begin();

    words.clear();
    curWordNo = 0;

    while ( !isTerminal( *tPtr ) && !isInString( *tPtr, additionalTermnials ) && ( tPtr != text.end() ) )
    {
        if ( isSeparator( *tPtr ) || isInString( *tPtr, additionalSeparators ) )
        {
            if ( !word.empty() )
            {
                words.push_back( word );
                word.clear();
            }
        }
        else
        {
            word += *tPtr;
        }
        ++tPtr;
    }
    if ( !word.empty() )
    {
        words.push_back( word );
    }

    return words.size();
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

std::ostream &operator<<
(
    std::ostream        &ostr,
    const amSplitString &splitString
)
{
    size_t nbWords = splitString.size();
    if ( nbWords == 0 )
    {
        ostr << "<empty>";
    }
    else
    {
        ostr << "\"" << splitString[ 0 ] << "\"";
        for ( size_t counter = 1; counter < nbWords; ++counter )
        {
            ostr << "\t\"" << splitString[ counter ] << "\"";
        }
    }
    return ostr;
}

