#ifndef __AM_SPLIT_STRING_H__
#define __AM_SPLIT_STRING_H__

#include <fstream>
#include <ostream>
#include <vector>

#include "am_string.h"

class amSplitString
{

    private:

        std::vector<amString> words;
        amString              terminals;
        amString              separators;
        size_t                curWordNo;

        void init       ( void );
        bool isTerminal ( char ) const;
        bool isSeparator( char ) const;


    public:

        amSplitString( void );
        amSplitString( const amString & );
        ~amSplitString( void ) {};

        inline void removeAllTerminals( void )  { terminals.clear(); }
        inline void removeAllSeparators( void ) { separators.clear(); }
        inline void clear( void ) { words.clear(); }

        void addTerminal( char );
        void addSeparator( char );
        void removeTerminal( char );
        void removeSeparator( char );
        void push_back( const amString &word );

        size_t split( const amString &text, const amString &additionalTermnials = "", const amString &additionalSeparators = "" );

        const amString getWord( size_t index ) const;
        const amString operator[]( size_t index ) const;
        const amString getNextWord( void );
        const amString front( void ) const;
        const amString back( void ) const;

        amString concatenate( int start, int end = -1, char link = ' ' ) const;

        inline size_t size( void ) const { return words.size(); }

};

extern std::ostream &operator<<( std::ostream &, const amSplitString & );

#endif // __AM_SPLIT_STRING_H__

