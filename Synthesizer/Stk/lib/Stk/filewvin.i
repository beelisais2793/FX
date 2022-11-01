%{
#include "FileWvIn.h"
%}
namespace stk {
    class FileWvIn : public WvIn
    {
    public:


        FileWvIn( unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024 );
        FileWvIn( std::string fileName, bool raw = false, bool doNormalize = true,
                    unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024,
                    bool doInt2FloatScaling = true );
        ~FileWvIn( void );

        virtual void openFile( std::string fileName, bool raw = false, bool doNormalize = true, bool doInt2FloatScaling = true );
        virtual void closeFile( void );
        virtual void reset( void );
        virtual void normalize( void );
        virtual void normalize( StkFloat peak );
        virtual unsigned long getSize( void ) const;
        virtual StkFloat getFileRate( void ) const;
        bool isOpen( void );
        bool isFinished( void ) const;
        virtual void setRate( StkFloat rate );
        virtual void addTime( StkFloat time );
        void setInterpolate( bool doInterpolate );
        StkFloat lastOut( unsigned int channel = 0 );
        virtual StkFloat tick( unsigned int channel = 0 );
        virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
   };
}