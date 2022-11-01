%{
#include "WvOut.h"
%}

namespace stk
{
    class WvOut : public Stk
    {
    public:

        WvOut( void );
        unsigned long getFrameCount( void ) const;
        StkFloat getTime( void ) const;
        bool clipStatus( void );
        void resetClipStatus( void );
        virtual void tick( const StkFloat sample ) = 0;
        virtual void tick( const StkFrames& frames ) = 0;
    };
}