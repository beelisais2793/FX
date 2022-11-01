%module stk_asymp
%{
#include "Asymp.h"
%}

namespace stk 
{
    
    class Asymp 
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
        int getState( void ) const;
        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
   };
}