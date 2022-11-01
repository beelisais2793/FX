%{
#include "Simple.h"
%}

namespace stk
{
    class Simple : public Instrmnt
    {
    public:

        Simple( void );
        ~Simple( void );

        void setFrequency( StkFloat frequency );
        void keyOn( void );
        void keyOff( void );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}