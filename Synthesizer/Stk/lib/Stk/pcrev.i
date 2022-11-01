%{
#include "PRCRev.h"
%}

namespace stk {
    class PRCRev : public Effect
    {
    public:

        PRCRev( StkFloat T60 = 1.0 );

        void clear( void );
        void setT60( StkFloat T60 );
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( StkFloat input, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}