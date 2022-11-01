%{
#include "DelayL.h"
%}

namespace stk
{
    class DelayL : public Filter
    {
    public:

        DelayL( StkFloat delay = 0.0, unsigned long maxDelay = 4095 );
        ~DelayL();

        unsigned long getMaximumDelay( void );
        void setMaximumDelay( unsigned long delay );
        void setDelay( StkFloat delay );
        StkFloat getDelay( void ) const;
        StkFloat tapOut( unsigned long tapDelay );
        void tapIn( StkFloat value, unsigned long tapDelay );
        StkFloat lastOut( void ) const;
        StkFloat nextOut( void );
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}