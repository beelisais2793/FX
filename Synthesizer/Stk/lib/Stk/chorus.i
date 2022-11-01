%{
#include "Chorus.h"
%}

namespace stk {
    class Chorus : public Effect
    {
    public:
        Chorus( StkFloat baseDelay = 6000 );

        void clear( void );
        void setModDepth( StkFloat depth );
        void setModFrequency( StkFloat frequency );
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( StkFloat input, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}