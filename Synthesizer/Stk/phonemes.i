%{
#include "Phonemes.h"
%}

namespace stk {
    class Phonemes : public Stk
    {
    public:

        Phonemes( void );
        ~Phonemes( void );


        static const char *name( unsigned int index );
        static StkFloat voiceGain( unsigned int index );
        static StkFloat noiseGain( unsigned int index );
        static StkFloat formantFrequency( unsigned int index, unsigned int partial );
        static StkFloat formantRadius( unsigned int index, unsigned int partial );
        static StkFloat formantGain( unsigned int index, unsigned int partial );
    };
}