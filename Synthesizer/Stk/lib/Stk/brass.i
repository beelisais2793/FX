%{
#include "Brass.h"
%}

namespace stk
{
    class Brass: public Instrmnt
    {
    public:

        Brass( StkFloat lowestFrequency = 8.0 );
        ~Brass(  );
        void clear(  );
        void setFrequency( StkFloat frequency );
        void setLip( StkFloat frequency );
        void startBlowing( StkFloat amplitude, StkFloat rate );
        void stopBlowing( StkFloat rate );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}