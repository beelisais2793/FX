%{
#include "Asymp.h"
%}

namespace stk 
{
    const StkFloat TARGET_THRESHOLD = 0.000001;
    class Asymp : public Generator
    {
    public:

        Asymp( void );
        ~Asymp( void );

        void keyOn( void );
        void keyOff( void );

        void setTau( StkFloat tau );
        void setTime( StkFloat time );
        void setT60( StkFloat t60 );
        void setTarget( StkFloat target );
        void setValue( StkFloat value );
        int getState( void ) const { return state_; };
        StkFloat lastOut( void ) const { return lastFrame_[0]; };
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
   };
}