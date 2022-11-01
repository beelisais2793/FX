%{
#include "FormSwep.h"
%}

namespace stk
{
    class FormSwep : public Filter
    {
    public:

 
        FormSwep( void );
       ~FormSwep();
        void ignoreSampleRateChange( bool ignore = true );
        void setResonance( StkFloat frequency, StkFloat radius );
        void setStates( StkFloat frequency, StkFloat radius, StkFloat gain = 1.0 );
        void setTargets( StkFloat frequency, StkFloat radius, StkFloat gain = 1.0 );
        void setSweepRate( StkFloat rate );
        void setSweepTime( StkFloat time );
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}
