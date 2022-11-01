%{
#include "Delay.h"
%}

namespace stk 
{
    class Delay : public Filter
    {
    public:

        Delay( unsigned long delay = 0, unsigned long maxDelay = 4095 );
       ~Delay();

        unsigned long getMaximumDelay( void );
        void setMaximumDelay( unsigned long delay );
        void setDelay( unsigned long delay );
        unsigned long getDelay( void ) const;
        StkFloat tapOut( unsigned long tapDelay );
        void tapIn( StkFloat value, unsigned long tapDelay );
        StkFloat addTo( StkFloat value, unsigned long tapDelay );
        StkFloat lastOut( void ) const;
        StkFloat nextOut( void );
        StkFloat energy( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}