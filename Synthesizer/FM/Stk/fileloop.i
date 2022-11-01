%{
#include "FileLoop.h"
%}

namespace stk
{
    // it's just a wave table or a sampler the name isn't very informative
    // one day I will just convert all the raw samples to big endian as nobody even probably has a next computer anymore
    class FileLoop : protected FileWvIn
    {
    public:
    
        FileLoop( unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024 );

        FileLoop( std::string fileName, bool raw = false, bool doNormalize = true,
                    unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024,
                    bool doInt2FloatScaling = true );


        ~FileLoop( void );

        void openFile( std::string fileName, bool raw = false, bool doNormalize = true, bool doInt2FloatScaling = true );
        void closeFile( void );
        void reset( void );
        unsigned int channelsOut( void ) const;
        void normalize( void );
        void normalize( StkFloat peak );
        unsigned long getSize( void ) const;
        StkFloat getFileRate( void ) const;
        void setRate( StkFloat rate );
        void setFrequency( StkFloat frequency );
        void addTime( StkFloat time );
        void addPhase( StkFloat angle );
        void addPhaseOffset( StkFloat angle );
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( unsigned int channel = 0 );
        virtual StkFrames& tick( StkFrames& frames,unsigned int channel = 0 );
    };

}