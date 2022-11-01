%{
#include "BlowHole.h"
%}

class stk::BlowHole : public Instrmnt
{
public:

    BlowHole( StkFloat lowestFrequency );
    ~BlowHole( void );
    void clear( void );
    void setFrequency( StkFloat frequency );
    void setTonehole( StkFloat newValue );
    void setVent( StkFloat newValue );
    void startBlowing( StkFloat amplitude, StkFloat rate );
    void stopBlowing( StkFloat rate );
    void noteOn( StkFloat frequency, StkFloat amplitude );
    void noteOff( StkFloat amplitude );
    void controlChange( int number, StkFloat value );
    StkFloat tick( unsigned int channel = 0 );
    StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
};
