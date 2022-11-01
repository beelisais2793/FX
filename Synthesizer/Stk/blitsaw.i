%{
#include "BlitSaw.h"
%}

namespace stk
{

    class BlitSaw: public Generator
    {
    public:
        BlitSaw( StkFloat frequency = 220.0 );
        ~BlitSaw();

        void reset();
        void setFrequency( StkFloat frequency );
        void setHarmonics( unsigned int nHarmonics = 0 );
        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

    };

}