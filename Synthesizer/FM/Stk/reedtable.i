%{
#include "ReedTable.h"
%}

namespace stk
{
    class ReedTable : public Function
    {
    public:

        ReedTable( void );
        void setOffset( StkFloat offset );
        void setSlope( StkFloat slope );
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}