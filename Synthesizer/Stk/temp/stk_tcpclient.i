%{
#include "TcpClient.h"
%}
namespace stk {

    /***************************************************/
    /*! \class TcpClient
        \brief STK TCP socket client class.
        This class provides a uniform cross-platform TCP socket client
        interface.  Methods are provided for reading or writing data
        buffers to/from connections.
        TCP sockets are reliable and connection-oriented.  A TCP socket
        client must be connected to a TCP server before data can be sent
        or received.  Data delivery is guaranteed in order, without loss,
        error, or duplication.  That said, TCP transmissions tend to be
        slower than those using the UDP protocol and data sent with
        multiple \e write() calls can be arbitrarily combined by the
        underlying system.
        The user is responsible for checking the values
        returned by the read/write methods.  Values
        less than or equal to zero indicate a closed
        or lost connection or the occurence of an error.
        by Perry R. Cook and Gary P. Scavone, 1995--2021.
    */
    /***************************************************/

    class TcpClient : public Socket
    {
    public:

        TcpClient( int port, std::string hostname = "localhost" );
        ~TcpClient();

        int connect( int port, std::string hostname = "localhost" );
        int writeBuffer(const void *buffer, long bufferSize, int flags = 0);
        int readBuffer(void *buffer, long bufferSize, int flags = 0);

    };

} // stk namespace