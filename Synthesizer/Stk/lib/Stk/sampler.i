%{
#include "Sampler.h"
%}

namespace stk {
    class Sampler : public Instrmnt
    {
    public:

        Sampler( void );
        virtual ~Sampler( void );

        virtual void setFrequency( StkFloat frequency ) = 0;
        void keyOn( void );
        void keyOff( void );
        virtual void noteOff( StkFloat amplitude );
        virtual void controlChange( int number, StkFloat value ) = 0;
        virtual StkFloat tick( unsigned int channel = 0 ) = 0;
        virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ) = 0;
    };
}