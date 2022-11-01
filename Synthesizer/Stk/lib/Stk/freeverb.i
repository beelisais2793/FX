%{
#include "FreeVerb.h"
%}

namespace stk
{
    class FreeVerb : public Effect
    {   
    public:
        FreeVerb();
        ~FreeVerb();

        void setEffectMix( StkFloat mix );
        void setRoomSize( StkFloat value );
        StkFloat getRoomSize( void );
        void setDamping( StkFloat value );
        StkFloat getDamping( void );
        void setWidth( StkFloat value );
        StkFloat getWidth( void );
        void setMode( bool isFrozen );
        StkFloat getMode( void );
        void clear( void );
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( StkFloat inputL, StkFloat inputR = 0.0, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}