%{    
    #include "PoleZero.h"
%}

namespace stk
{
    class PoleZero : public Filter
    {
    public:

        PoleZero();
        ~PoleZero();

        void setB0( StkFloat b0 );
        void setB1( StkFloat b1 );
        void setA1( StkFloat a1 );
        void setCoefficients( StkFloat b0, StkFloat b1, StkFloat a1, bool clearState = false );
        void setAllpass( StkFloat coefficient );
        void setBlockZero( StkFloat thePole = 0.99 );
        StkFloat lastOut( void ) const;
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}