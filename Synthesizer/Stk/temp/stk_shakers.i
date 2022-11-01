%{
#include "Shakers.h"
%}


namespace stk
{
    class Shakers : public Instrmnt
    {
    public:

    
        Shakers( int type = 0 );

        void noteOn( StkFloat instrument, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );        
    };

    /* wont work
    struct Shakers::BiQuad 
    {
        StkFloat gain;
        StkFloat b[3];
        StkFloat a[3];
        StkFloat inputs[3];
        StkFloat outputs[3];        
        BiQuad();
    };
    */

}