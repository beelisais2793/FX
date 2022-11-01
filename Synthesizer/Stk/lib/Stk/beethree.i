%{
#include "BeeThree.h"
%}

namespace stk 
{

    class BeeThree : public FM
    {
    public:
        BeeThree( void );
        ~BeeThree( void );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };

}