%{
#include "Plucked.h"
%}

namespace stk
{
    class Plucked : public Instrmnt
    {
    public:
 
        Plucked( StkFloat lowestFrequency = 10.0 );
       ~Plucked( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        void pluck( StkFloat amplitude );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}