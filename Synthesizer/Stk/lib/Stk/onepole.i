%{
#include "OnePole.h"
%}

namespace stk {
    class OnePole : public Filter
    {
    public:

        OnePole( StkFloat thePole = 0.9 );
        ~OnePole();

        void setB0( StkFloat b0 );
        void setA1( StkFloat a1 );
        void setCoefficients( StkFloat b0, StkFloat a1, bool clearState = false );
        void setPole( StkFloat thePole );
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

    };
}