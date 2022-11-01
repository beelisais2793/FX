%{
#include "FileWvOut.h"    
%}

namespace stk {
    class FileWvOut : public WvOut
    {
    public:

        FileWvOut( unsigned int bufferFrames = 1024 );
        FileWvOut( std::string fileName,
                    unsigned int nChannels = 1,
                    FileWrite::FILE_TYPE type = FileWrite::FILE_WAV,
                    Stk::StkFormat format = STK_SINT16,
                    unsigned int bufferFrames = 1024 );
        
        virtual ~FileWvOut();
        void openFile( std::string fileName,
                        unsigned int nChannels,
                        FileWrite::FILE_TYPE type,
                        Stk::StkFormat format );
        void closeFile( void );
        void tick( const StkFloat sample );
        void tick( const StkFrames& frames );
   };
}