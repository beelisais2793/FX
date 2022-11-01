%{
#include "Granulate.h"
%}

namespace stk 
{
    
    class Granulate: public Generator
    {
    public:


        Granulate( void );
        Granulate( unsigned int nVoices, std::string fileName, bool typeRaw = false );
        ~Granulate( void );

        void openFile( std::string fileName, bool typeRaw = false );
        void reset( void );
        void setVoices( unsigned int nVoices = 1 );
        void setStretch( unsigned int stretchFactor = 1 );
        void setGrainParameters( unsigned int duration = 30, unsigned int rampPercent = 50,
                                int offset = 0, unsigned int delay = 0 );
        void setRandomFactor( StkFloat randomness = 0.1 );
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

        enum GrainState {
            GRAIN_STOPPED,
            GRAIN_FADEIN,
            GRAIN_SUSTAIN,
            GRAIN_FADEOUT
        };

    };
}