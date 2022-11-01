%{
#include "PitShift.h"
%}

namespace stk
{
    class PitShift : public Effect
    {
    public:

        PitShift( void );

        void clear( void );
        void setShift( StkFloat shift );
        StkFloat lastOut( void );
        StkFloat tick( StkFloat input );

        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}