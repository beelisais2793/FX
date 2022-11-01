%{
#include "Bowed.h"
%}

namespace stk{
    class Bowed : public Instrmnt
    {
    public:
        Bowed( StkFloat lowestFrequency = 8.0 );
        ~Bowed( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        void setVibrato( StkFloat gain );
        void startBowing( StkFloat amplitude, StkFloat rate );
        void stopBowing( StkFloat rate );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
    };
}