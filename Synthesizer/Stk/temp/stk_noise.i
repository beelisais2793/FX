%{
#include "Noise.h"
%}

namespace stk 
{
    class Noise : public Generator
    {
    public:

        Noise( unsigned int seed = 0 );
        void setSeed( unsigned int seed = 0 );

        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}