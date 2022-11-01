%{
    #include "Socket.h"
    #include "TcpClient.h"
    #include "TcpServer.h"
    #include "UdpSocket.h"
%}

namespace stk
{
    class Socket : public Stk
    {
    public:

        enum ProtocolType {
            PROTO_TCP,
            PROTO_UDP
        };


        Socket();
        virtual ~Socket();


        static void close( int socket );
        int id( void ) const;
        int port( void ) const;
        static bool isValid( int socket );
        static void setBlocking( int socket, bool enable );

        virtual int writeBuffer(const void *buffer, long bufferSize, int flags = 0) = 0;
        virtual int readBuffer(void *buffer, long bufferSize, int flags = 0) = 0;
        static int writeBuffer(int socket, const void *buffer, long bufferSize, int flags );
        static int readBuffer(int socket, void *buffer, long bufferSize, int flags );
    };

    class TcpClient : public Socket
    {
    public:

        TcpClient( int port, std::string hostname = "localhost" );
        ~TcpClient();

        int connect( int port, std::string hostname = "localhost" );
        int writeBuffer(const void *buffer, long bufferSize, int flags = 0);
        int readBuffer(void *buffer, long bufferSize, int flags = 0);

    };

    class TcpServer : public Socket
    {
    public:

        TcpServer( int port = 2006 );
        ~TcpServer();

        int accept( void );
        int writeBuffer(const void *buffer, long bufferSize, int flags = 0);
        int readBuffer(void *buffer, long bufferSize, int flags = 0);

    };

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
}