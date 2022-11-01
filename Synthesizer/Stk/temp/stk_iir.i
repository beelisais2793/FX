%{
#include "Iir.h"
%}

namespace stk 
{
    class Iir : public Filter
    {
    public:

        Iir( void );
        Iir( std::vector<StkFloat> &bCoefficients, std::vector<StkFloat> &aCoefficients );
        ~Iir( void );

        void setCoefficients( std::vector<StkFloat> &bCoefficients, std::vector<StkFloat> &aCoefficients, bool clearState = false );
        void setNumerator( std::vector<StkFloat> &bCoefficients, bool clearState = false );
        void setDenominator( std::vector<StkFloat> &aCoefficients, bool clearState = false );
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}