%{
#include "FMVoices.h"
%}

namespace stk
{
    class FMVoices : public FM
    {
    public:
 
        FMVoices( void );
        ~FMVoices( void );

        void setFrequency( StkFloat frequency );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}