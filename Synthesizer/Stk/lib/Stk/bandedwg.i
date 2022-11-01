%{
#include "BandedWG.h"
%}

namespace stk 
{
    const int MAX_BANDED_MODES = 20;

    class BandedWG : public Instrmnt
    {
    public:
        BandedWG( void );
        ~BandedWG( void );

        void clear( void );
        void setStrikePosition( StkFloat position );
        void setPreset( int preset );
        void setFrequency( StkFloat frequency );
        void startBowing( StkFloat amplitude, StkFloat rate );
        void stopBowing( StkFloat rate );
        void pluck( StkFloat amp );

        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );

        void controlChange( int number, StkFloat value );

        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}