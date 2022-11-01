%{
#include "TubeBell.h"
%}

namespace stk
{
    class TubeBell : public FM
    {
    public:
 
        TubeBell( void );
       ~TubeBell( void );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}