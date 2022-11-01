%{
#include "FileRead.h"
%}

namespace stk
{
    class FileRead : public Stk
    {
    public:

        FileRead( void );
        FileRead( std::string fileName, bool typeRaw = false, unsigned int nChannels = 1,
                    StkFormat format = STK_SINT16, StkFloat rate = 22050.0 );        
        ~FileRead( void );

        void open( std::string fileName, bool typeRaw = false, unsigned int nChannels = 1,
                    StkFormat format = STK_SINT16, StkFloat rate = 22050.0 );
        void close( void );
        bool isOpen( void );
        unsigned long fileSize( void ) const;
        unsigned int channels( void ) const;
        StkFormat format( void ) const;
        StkFloat fileRate( void ) const;
        void read( StkFrames& buffer, unsigned long startFrame = 0, bool doNormalize = true );
    };
}