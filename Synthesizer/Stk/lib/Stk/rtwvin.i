%{
#include "RtWvIn.h"
%}

namespace stk
{
    class RtWvIn : public WvIn
    {
    public:

        RtWvIn( unsigned int nChannels = 1, StkFloat sampleRate = Stk::sampleRate(),
                int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 20 );
        ~RtWvIn();

        void start( void );
        void stop( void );
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        void fillBuffer( void *buffer, unsigned int nFrames );
    };
}