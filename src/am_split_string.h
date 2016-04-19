#ifndef __AM_SPLIT_STRING_H__
#define __AM_SPLIT_STRING_H__

#include <fstream>
#include <ostream>
#include <vector>
#include <string>

class amSplitString
{
    public:

        amSplitString( void );
        amSplitString( const std::string & );

        inline void removeAllTerminals( void )  { terminals.clear(); }
        inline void removeAllSeparators( void ) { separators.clear(); }
        inline void clear( void ) { words.clear(); }

        void addTerminal( char );
        void addSeparator( char );
        void removeTerminal( char );
        void removeSeparator( char );

        size_t split( const std::string &text, const std::string &additionalTermnials = "", const std::string &additionalSeparators = "" );

        void push_back( const std::string &newWord );
        const std::string getWord( unsigned int ) const;
        const std::string operator[]( unsigned int ) const;
        const std::string getNextWord( void );
        const std::string front( void ) const;
        const std::string back( void ) const;

        inline size_t size( void ) const { return words.size(); }


    private:

        std::vector<std::string> words;
        std::string              terminals;
        std::string              separators;
        size_t                   curWordNo;

        void init            ( void );
        bool isTerminal      ( char ) const;
        bool isSeparator     ( char ) const;
        bool isInString      ( char, const std::string & ) const;
        void removeFromString( char, std::string & );

};

extern std::ostream &operator<<( std::ostream &, const amSplitString & );

#endif // __AM_SPLIT_STRING_H__

