%{
#include "HevyMetl.h"
%}

namespace stk
{
    class HevyMetl : public FM
    {
    public:

        HevyMetl( void );
        ~HevyMetl( void );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}