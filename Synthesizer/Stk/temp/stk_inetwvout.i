%{
#include "InetWvOut.h"
%}

namespace stk
{
    class InetWvOut : public WvOut
    {
    public:

        InetWvOut( unsigned long packetFrames = 1024 );
        InetWvOut( int port, Socket::ProtocolType protocol = Socket::PROTO_TCP,
                    std::string hostname = "localhost", unsigned int nChannels = 1, Stk::StkFormat format = STK_SINT16,
                    unsigned long packetFrames = 1024 );
        ~InetWvOut();

        void connect( int port, Socket::ProtocolType protocol = Socket::PROTO_TCP,
                        std::string hostname = "localhost", unsigned int nChannels = 1, Stk::StkFormat format = STK_SINT16 );
        void disconnect( void );
        void tick( const StkFloat sample );
        void tick( const StkFrames& frames );
    };
}