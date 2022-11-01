%{
#include "BiQuad.h"
%}    
namespace stk
{
    class BiQuad : public Filter
    {
    public:

        BiQuad();
        ~BiQuad();

        void ignoreSampleRateChange( bool ignore = true );
        void setCoefficients( StkFloat b0, StkFloat b1, StkFloat b2, StkFloat a1, StkFloat a2, bool clearState = false );

        void setB0( StkFloat b0 );
        void setB1( StkFloat b1 );
        void setB2( StkFloat b2 );
        void setA1( StkFloat a1 );
        void setA2( StkFloat a2 );
        void setResonance( StkFloat frequency, StkFloat radius, bool normalize = false );

        void setNotch( StkFloat frequency, StkFloat radius );
        void setEqualGainZeroes( void );
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}