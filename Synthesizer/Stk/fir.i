%{
#include "Fir.h"
%}

namespace stk 
{
    class Fir : public Filter
    {
    public:

        Fir( void );
        Fir( std::vector<StkFloat> &coefficients );
        ~Fir( void );
        void setCoefficients( std::vector<StkFloat> &coefficients, bool clearState = false );
        StkFloat lastOut( void ) const { return lastFrame_[0]; };        
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}