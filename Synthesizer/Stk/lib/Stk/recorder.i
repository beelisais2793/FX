%{
#include "Recorder.h"
%}

namespace stk {
    class Recorder : public Instrmnt
    {
    public:

        Recorder( void );
        ~Recorder( void );

        void clear( void );
        void setFrequency( StkFloat val );
        void startBlowing( StkFloat amplitude, StkFloat rate );
        void stopBlowing( StkFloat rate );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        void setBlowPressure( StkFloat val );
        void setVibratoGain( StkFloat val );
        void setVibratoFrequency( StkFloat val );
        void setNoiseGain( StkFloat val );
        void setBreathCutoff( StkFloat val );
        void setSoftness( StkFloat val );
    };
}