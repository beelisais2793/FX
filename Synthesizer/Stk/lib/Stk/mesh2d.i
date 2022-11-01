%{
#include "Mesh2D.h"
%}

namespace stk
{
 
    class Mesh2D : public Instrmnt
    {
    public:
 
        Mesh2D( unsigned short nX, unsigned short nY );
       ~Mesh2D( void );

        void clear( void );
        void setNX( unsigned short lenX );
        void setNY( unsigned short lenY );
        void setInputPosition( StkFloat xFactor, StkFloat yFactor );
        void setDecay( StkFloat decayFactor );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        StkFloat energy( void );
        StkFloat inputTick( StkFloat input );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}