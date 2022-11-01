%{
#include "Mandolin.h"
%}

namespace stk 
{
    class Mandolin : public Instrmnt
    {
    public:

        Mandolin( StkFloat lowestFrequency );
        ~Mandolin( void );

        void clear( void );
        void setDetune( StkFloat detune );
        void setBodySize( StkFloat size );
        void setPluckPosition( StkFloat position );
        void setFrequency( StkFloat frequency );
        void pluck( StkFloat amplitude );
        void pluck( StkFloat amplitude,StkFloat position );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
   };
}