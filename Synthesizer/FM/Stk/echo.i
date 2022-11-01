%{
#include "Echo.h"
%}

namespace stk
{
    class Echo : public Effect
    {
    public:

        Echo( unsigned long maximumDelay = (unsigned long) Stk::sampleRate() );


        void clear();
        void setMaximumDelay( unsigned long delay );
        void setDelay( unsigned long delay );
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}