%{
#include "TwoPole.h"
%}

namespace stk
{
    class TwoPole : public Filter
    {
    public:

        TwoPole( void );
        ~TwoPole();
        void ignoreSampleRateChange( bool ignore = true );
        void setB0( StkFloat b0 );
        void setA1( StkFloat a1 );
        void setA2( StkFloat a2 );
        void setCoefficients( StkFloat b0, StkFloat a1, StkFloat a2, bool clearState = false );
        void setResonance(StkFloat frequency, StkFloat radius, bool normalize = false);
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}