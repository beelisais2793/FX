%{
#include "Resonate.h"
%}

namespace stk
{
    class Resonate : public Instrmnt
    {
    public:

        Resonate( void );
        ~Resonate( void );

        void setResonance( StkFloat frequency, StkFloat radius );
        void setNotch( StkFloat frequency, StkFloat radius );
        void setEqualGainZeroes( void ) { filter_.setEqualGainZeroes(); };
        void keyOn( void );
        void keyOff( void );

        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );        
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
   };
}