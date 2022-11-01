%{
#include "Flute.h"
%}

namespace stk
{
    class Flute : public Instrmnt
    {
    public:

        Flute( StkFloat lowestFrequency );
        ~Flute( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        void setJetReflection( StkFloat coefficient );
        void setEndReflection( StkFloat coefficient );
        void setJetDelay( StkFloat aRatio );
        void startBlowing( StkFloat amplitude, StkFloat rate );
        void stopBlowing( StkFloat rate );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}