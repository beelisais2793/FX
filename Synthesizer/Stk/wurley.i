%{
#include "Wurley.h"
%}    

namespace stk {
    class Wurley : public FM
    {
    public:
 
        Wurley( void );
       ~Wurley( void );

        void setFrequency( StkFloat frequency );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

    };
}