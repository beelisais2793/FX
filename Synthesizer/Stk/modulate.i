%{
#include "Modulate.h"
%}


namespace stk
{
    class Modulate : public Generator
    {
    public:

        Modulate( void );
        ~Modulate( void );

        void reset( void );
        void setVibratoRate( StkFloat rate );
        void setVibratoGain( StkFloat gain );
        void setRandomRate( StkFloat rate );
        void setRandomGain( StkFloat gain );
        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}
