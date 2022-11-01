%{
#include "Cubic.h"
%}

namespace stk {
    class Cubic : public Function
    {
    public:
        Cubic( void );
        void setA1( StkFloat a1 );
        void setA2( StkFloat a2 );
        void setA3( StkFloat a3 );
        void setGain( StkFloat gain );
        void setThreshold( StkFloat threshold );
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}