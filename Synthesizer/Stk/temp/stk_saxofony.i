%{
#include "Saxofony.h"
%}

namespace stk
{
    class Saxofony : public Instrmnt
    {
    public:

        Saxofony( StkFloat lowestFrequency );
        ~Saxofony( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        void setBlowPosition( StkFloat aPosition );
        void startBlowing( StkFloat amplitude, StkFloat rate );
        void stopBlowing( StkFloat rate );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}