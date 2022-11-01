%{
#include "WvIn.h"
%}

namespace stk
{
    class WvIn : public Stk
    {
    public:    
        unsigned int channelsOut( void ) const;
        const StkFrames& lastFrame( void ) const;
        virtual StkFloat tick( unsigned int channel = 0 ) = 0;
        virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ) = 0;
    };
}