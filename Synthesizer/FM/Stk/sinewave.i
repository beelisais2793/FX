%{
#include "SineWave.h"
%}    

namespace stk
{
    class SineWave : public Generator
    {
    public:

        SineWave( void );
        ~SineWave( void );

        void reset( void );
        void setRate( StkFloat rate );
        void setFrequency( StkFloat frequency );
        void addTime( StkFloat time );
        void addPhase( StkFloat phase );
        void addPhaseOffset( StkFloat phaseOffset );

        
        StkFloat lastOut( void ) const;
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}
