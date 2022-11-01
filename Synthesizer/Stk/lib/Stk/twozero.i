%{
#include "TwoZero.h"
%}

namespace stk
{
    class TwoZero : public Filter
    {
    public:
    
            TwoZero();
        ~TwoZero();

            void ignoreSampleRateChange( bool ignore = true );
            void setB0( StkFloat b0 );
            void setB1( StkFloat b1 );
            void setB2( StkFloat b2 );
            void setCoefficients( StkFloat b0, StkFloat b1, StkFloat b2, bool clearState = false );
            void setNotch( StkFloat frequency, StkFloat radius );
            StkFloat lastOut( void ) const;
            StkFloat tick( StkFloat input );
            StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
            StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
        };
}