#ifndef __AM_SPLIT_STRING_H__
#define __AM_SPLIT_STRING_H__

#include <vector>
#include <string>

class amSplitString
{
    public:

        amSplitString( void );
        amSplitString( const char * );

        inline void removeAllTerminals( void )  { terminals.clear(); }
        inline void removeAllSeparators( void ) { separators.clear(); }
        inline void clear( void ) { words.clear(); }

        void addTerminal( char );
        void addSeparator( char );
        void removeTerminal( char );
        void removeSeparator( char );

        size_t split( const char        *text, const char *additionalTermnials = "", const char *additionalSeparators = "" );
        size_t split( const std::string &text, const char *additionalTermnials = "", const char *additionalSeparators = "" );

        void push_back( const std::string &newWord );
        const std::string getWord( unsigned int ) const;
        const std::string operator[]( unsigned int ) const;
        const std::string getNextWord( void );
        const std::string front( void ) const;
        const std::string back( void ) const;

        inline size_t size( void ) const { return words.size(); }

    private:

        std::vector<std::string> words;
        std::vector<char>        terminals;
        std::vector<char>        separators;

        bool isTerminal( char ) const;
        bool isSeparator( char ) const;
        bool isAdditionalTerminal( char, const char * ) const;
        bool isAdditionalSeparator( char, const char * ) const;

        unsigned int curWordNo;
};

#endif // __AM_SPLIT_STRING_H__

