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
    const amString &text
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

    separators.clear();
    separators += '\t';
    separators += ' ';

    terminals.clear();
    terminals  += '\0';
    terminals  += '\n';
    terminals  += '\r';
}

bool amSplitString::isSeparator
(
    char testChar
) const
{
    return separators.contains( testChar );
}

bool amSplitString::isTerminal
(
    char testChar
) const
{
    return terminals.contains( testChar );
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
    separators.removeFromString( obsoleteSeparator );
}

void amSplitString::removeTerminal
(
    char obsoleteTerminal
)
{
    terminals.removeFromString( obsoleteTerminal );
}


size_t amSplitString::split
(
    const amString &text,
    const amString &additionalTermnials,
    const amString &additionalSeparators
)
{
    amString                 word;
    amString::const_iterator tPtr = text.begin();

    words.clear();
    curWordNo = 0;

    while ( !isTerminal( *tPtr ) && !additionalTermnials.contains( *tPtr ) && ( tPtr != text.end() ) )
    {
        if ( isSeparator( *tPtr ) || additionalSeparators.contains( *tPtr ) )
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

const amString amSplitString::getWord
(
    size_t index
) const
{
    return index < words.size() ? words[ index ] : amString( "" );
}

const amString amSplitString::operator[]
(
    size_t index
) const
{
    return index < words.size() ? words[ index ] : amString( "" );
}

const amString amSplitString::front
(
    void
) const
{
    return words.size() ? words.front() : amString( "" );
}

const amString amSplitString::back
(
    void
) const
{
    return words.size() ? words.back() : amString( "" );
}

const amString amSplitString::getNextWord
(
    void
)
{
    if ( curWordNo < words.size() )
    {
        return words[ curWordNo++ ];
    }
    return amString( "" );
}

void amSplitString::push_back
(
    const amString &word
)
{
    if ( !word.empty() )
    {
        words.push_back( word );
    }
}

amString amSplitString::concatenate
(
    int  start,
    int  end,
    char link
) const
{
    amString result;
    int endPos = ( end < 0 ) ? words.size() : end;
    for ( int pos = start; pos < endPos; ++pos )
    {
        if ( !result.empty() )
        {
            result += link;
        }
        result += words[ pos ];
    }
    return result;
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

