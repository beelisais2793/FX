%{
#include "OneZero.h"
%}

namespace stk
{
    class OneZero : public Filter
    {
    public:

        OneZero( StkFloat theZero = -1.0 );
        ~OneZero();

        void setB0( StkFloat b0 );
        void setB1( StkFloat b1 );
        void setCoefficients( StkFloat b0, StkFloat b1, bool clearState = false );
        void setZero( StkFloat theZero );
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}