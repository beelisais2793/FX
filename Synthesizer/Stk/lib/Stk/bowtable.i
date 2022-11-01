%{
#include "BowTable.h"
%}

namespace stk{
    class BowTable : public Function
    {
    public:

        BowTable( void );
        void setOffset( StkFloat offset );
        void setSlope( StkFloat slope );
        void setMinOutput( StkFloat minimum );
        void setMaxOutput( StkFloat maximum );
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}