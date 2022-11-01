%{
#include "RtWvOut.h"
%}

namespace stk
{
    class RtWvOut : public WvOut
    {
    public:

        RtWvOut( unsigned int nChannels = 1, StkFloat sampleRate = Stk::sampleRate(),
                int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 20 );
       ~RtWvOut();

        void start( void );
        void stop( void );
        void tick( const StkFloat sample );
        void tick( const StkFrames& frames );
        int readBuffer( void *buffer, unsigned int frameCount );
    };
}