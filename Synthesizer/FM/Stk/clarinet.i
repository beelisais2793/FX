%{
#include "Clarinet.h"    
%}

namespace stk
{
    class Clarinet : public Instrmnt
    {
    public:
 
        Clarinet( StkFloat lowestFrequency = 8.0 );
       ~Clarinet( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        void startBlowing( StkFloat amplitude, StkFloat rate );
        void stopBlowing( StkFloat rate );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}