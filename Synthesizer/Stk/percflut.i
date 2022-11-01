%{
#include "PercFlut.h"
%}

namespace stk {

    class PercFlut : public FM
    {
    public:

        PercFlut( void );
        ~PercFlut( void );

        void setFrequency( StkFloat frequency );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}