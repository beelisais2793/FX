%{
#include "Blit.h"
%}

namespace stk
{
    class Blit: public Generator
    {
    public:
        Blit( StkFloat frequency = 220.0 );
        ~Blit();

        void reset();
        void setPhase( StkFloat phase );
        StkFloat getPhase() const;
        void setFrequency( StkFloat frequency );
        void setHarmonics( unsigned int nHarmonics = 0 );
        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}