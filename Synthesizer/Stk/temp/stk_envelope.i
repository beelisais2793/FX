%{
#include "Envelope.h"
%}

namespace stk
{
    class Envelope : public Generator
    {
    public:

        Envelope( void );
        ~Envelope( void );

        Envelope& operator= ( const Envelope& e );
        void keyOn( StkFloat target = 1.0 );
        void keyOff(  StkFloat target = 0.0 );
        void setRate( StkFloat rate );
        void setTime( StkFloat time );
        void setTarget( StkFloat target );
        void setValue( StkFloat value );
        int getState( void ) const;
        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };

}