%{
#include "LentPitShift.h"
%}

namespace stk {
    class LentPitShift : public Effect
    {
    public:
        
        LentPitShift( StkFloat periodRatio = 1.0, int tMax = RT_BUFFER_SIZE );
        ~LentPitShift( void ) {
            delete window;
            delete dt;
            delete dpt;
            delete cumDt;
        }

        void clear( void );
        void setShift( StkFloat shift );
        StkFloat tick( StkFloat input );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

    };
}