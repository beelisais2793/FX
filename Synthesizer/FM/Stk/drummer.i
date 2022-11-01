%{
#include "Drummer.h"
%}

namespace stk
{
    class Drummer : public Instrmnt
    {
    public:
 
        Drummer( void );
       ~Drummer( void );

        void noteOn( StkFloat instrument, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}