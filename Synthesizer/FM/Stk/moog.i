%{
#include "Moog.h"
%}

namespace stk
{
    class Moog : public Sampler
    {
    public:

        Moog( void );
        ~Moog( void );

        void setFrequency( StkFloat frequency );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void setModulationSpeed( StkFloat mSpeed );
        void setModulationDepth( StkFloat mDepth );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}