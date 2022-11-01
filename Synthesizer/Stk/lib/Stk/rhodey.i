%{
#include "Rhodey.h"
%}

namespace stk {

    class Rhodey : public FM
    {
    public:

        Rhodey( void );
        ~Rhodey( void );

        void setFrequency( StkFloat frequency );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}
