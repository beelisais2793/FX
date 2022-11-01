%{
#include "Twang.h"
%}

namespace stk
{
    class Twang : public Stk
    {
    public:

        Twang( StkFloat lowestFrequency = 50.0 );

        void clear( void );
        void setLowestFrequency( StkFloat frequency );
        void setFrequency( StkFloat frequency );
        void setPluckPosition( StkFloat position );
        void setLoopGain( StkFloat loopGain );
        //void setLoopFilter( std::vector<StkFloat> coefficients );
        StkFloat lastOut( void );
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}