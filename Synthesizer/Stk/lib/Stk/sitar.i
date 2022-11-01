%{
#include "Sitar.h"
%}

namespace stk
{
    class Sitar : public Instrmnt
    {
    public:

        Sitar( StkFloat lowestFrequency = 8.0 );
        ~Sitar( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        void pluck( StkFloat amplitude );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}