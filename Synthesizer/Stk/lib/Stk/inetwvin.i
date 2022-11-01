%{
#include "InetWvIn.h"
%}

namespace stk {
    class InetWvIn : public WvIn
    {
    public:

        InetWvIn( unsigned long bufferFrames = 1024, unsigned int nBuffers = 8 );
        ~InetWvIn();

        void listen( int port = 2006, unsigned int nChannels = 1,
                    Stk::StkFormat format = STK_SINT16,
                    Socket::ProtocolType protocol = Socket::PROTO_TCP );

        bool isConnected( void );
        StkFloat lastOut( unsigned int channel = 0 );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        void receive( void );
   };

}