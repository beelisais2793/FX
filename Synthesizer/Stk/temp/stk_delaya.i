%{
#include "DelayA.h"
%}

namespace stk
{
    class DelayA : public Filter
    {
    public:
        DelayA( StkFloat delay = 0.5, unsigned long maxDelay = 4095 );
        ~DelayA();

        void clear( void );
        unsigned long getMaximumDelay( void );
        void setMaximumDelay( unsigned long delay );
        void setDelay( StkFloat delay );
        StkFloat getDelay( void ) const;
        StkFloat tapOut( unsigned long tapDelay );    
        StkFloat lastOut( void ) const;
        StkFloat nextOut( void );
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
   };

}