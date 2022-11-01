%{
#include "TcpServer.h"
%}

namespace stk {

    /***************************************************/
    /*! \class TcpServer
        \brief STK TCP socket server class.
        This class provides a uniform cross-platform TCP socket server
        interface.  Methods are provided for reading or writing data
        buffers to/from connections.
        TCP sockets are reliable and connection-oriented.  A TCP socket
        server must accept a connection from a TCP client before data can
        be sent or received.  Data delivery is guaranteed in order,
        without loss, error, or duplication.  That said, TCP transmissions
        tend to be slower than those using the UDP protocol and data sent
        with multiple \e write() calls can be arbitrarily combined by the
        underlying system.
        The user is responsible for checking the values
        returned by the read/write methods.  Values
        less than or equal to zero indicate a closed
        or lost connection or the occurence of an error.
        by Perry R. Cook and Gary P. Scavone, 1995--2021.
    */
    /***************************************************/

    class TcpServer : public Socket
    {
    public:

        TcpServer( int port = 2006 );
        ~TcpServer();

        int accept( void );
        int writeBuffer(const void *buffer, long bufferSize, int flags = 0);
        int readBuffer(void *buffer, long bufferSize, int flags = 0);
    };

} // stk namespace

