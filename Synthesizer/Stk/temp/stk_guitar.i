%{
#include "Guitar.h"
%}

namespace stk {
    class Guitar : public Stk
    {
    public:
 
        Guitar( unsigned int nStrings = 6, std::string bodyfile = "" );
 
        void clear( void );
        void setBodyFile( std::string bodyfile = "" );
        void setPluckPosition( StkFloat position, int string = -1 );
        void setLoopGain( StkFloat gain, int string = -1 );
        void setFrequency( StkFloat frequency, unsigned int string = 0 );
        void noteOn( StkFloat frequency, StkFloat amplitude, unsigned int string = 0 );
        void noteOff( StkFloat amplitude, unsigned int string = 0 );
        void controlChange( int number, StkFloat value, int string = -1 );
        StkFloat lastOut( void );
        StkFloat tick( StkFloat input = 0.0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );
    };
}