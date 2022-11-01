%{
#include "UdpSocket.h"
%}

namespace stk {

    /***************************************************/
    /*! \class UdpSocket
        \brief STK UDP socket server/client class.
        This class provides a uniform cross-platform UDP socket
        server/client interface.  Methods are provided for reading or
        writing data buffers.  The constructor creates a UDP socket and
        binds it to the specified port.  Note that only one socket can be
        bound to a given port on the same machine.
        UDP sockets provide unreliable, connection-less service.  Messages
        can be lost, duplicated, or received out of order.  That said,
        data transmission tends to be faster than with TCP connections and
        datagrams are not potentially combined by the underlying system.
        The user is responsible for checking the values returned by the
        read/write methods.  Values less than or equal to zero indicate
        the occurence of an error.
        by Perry R. Cook and Gary P. Scavone, 1995--2021.
    */
    /***************************************************/

    class UdpSocket : public Socket
    {
    public:

        UdpSocket( int port = 2006 );
        ~UdpSocket();

        void setDestination( int port = 2006, std::string hostname = "localhost" );
        int writeBuffer(const void *buffer, long bufferSize, int flags = 0);
        int readBuffer(void *buffer, long bufferSize, int flags = 0);
        int writeBufferTo(const void *buffer, long bufferSize, int port, std::string hostname = "localhost", int flags = 0 );
    };
} // stk namespace