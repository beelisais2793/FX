%{
#include "SingWave.h"
%}

namespace stk
{

    class SingWave : public Generator
    {
    public:
 
        SingWave( std::string fileName, bool raw = false );
       ~SingWave( void );

        void reset( void );
        void normalize( void );
        void normalize( StkFloat peak );
        void setFrequency( StkFloat frequency );
        void setVibratoRate( StkFloat rate );
        void setVibratoGain( StkFloat gain );
        void setRandomGain( StkFloat gain );
        void setSweepRate( StkFloat rate );
        void setGainRate( StkFloat rate );
        void setGainTarget( StkFloat target );
        void noteOn( void );
        void noteOff( void );
        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}