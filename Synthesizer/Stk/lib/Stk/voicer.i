%{
#include "Voicer.h"
%}

namespace stk
{
    class Voicer : public Stk
    {
    public:

        Voicer( StkFloat decayTime = 0.2 );

        void addInstrument( Instrmnt *instrument, int group=0 );
        void removeInstrument( Instrmnt *instrument );
        long noteOn( StkFloat noteNumber, StkFloat amplitude, int group=0 );
        void noteOff( StkFloat noteNumber, StkFloat amplitude, int group=0 );
        void noteOff( long tag, StkFloat amplitude );
        void setFrequency( StkFloat noteNumber, int group=0 );
        void setFrequency( long tag, StkFloat noteNumber );
        void pitchBend( StkFloat value, int group=0 );
        void pitchBend( long tag, StkFloat value );
        void controlChange( int number, StkFloat value, int group=0 );
        void controlChange( long tag, int number, StkFloat value );
        void silence( void );
        unsigned int channelsOut( void ) const;
        const StkFrames& lastFrame( void ) const;
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}