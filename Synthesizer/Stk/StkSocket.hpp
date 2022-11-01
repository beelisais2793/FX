#pragma once
#include "Stk.hpp"

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  #include <sys/socket.h>
  #include <sys/types.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>

#elif defined(__OS_WINDOWS__)

  #include <winsock.h>

#endif

namespace stk {

/***************************************************/
/*! \class Socket
    \brief STK internet socket abstract base class.

    This class provides common functionality for TCP and UDP internet
    socket server and client subclasses.  This class also provides a
    number of static functions for use with external socket
    descriptors.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

enum ProtocolType {
    PROTO_TCP,
    PROTO_UDP
  };


class Socket : public Stk
{
 public:

  
  //! Class constructor
  Socket();

  //! Class destructor.
  virtual ~Socket();

  //! Close the socket.
  static void close( int socket );

  //! Return the socket descriptor.
  int id( void ) const { return soket_; };

  //! Return the socket port number.
  int port( void ) const { return port_; };

  //! Returns true if the socket descriptor is valid.
  static bool isValid( int socket ) { return socket != -1; };

  //! If enable = false, the socket is set to non-blocking mode.  When first created, sockets are by default in blocking mode.
  static void setBlocking( int socket, bool enable );

  //! Write a buffer over the socket connection.  Returns the number of bytes written or -1 if an error occurs.
  virtual int writeBuffer(const void *buffer, long bufferSize, int flags = 0) = 0;

  //! Read an input buffer, up to length \e bufferSize.  Returns the number of bytes read or -1 if an error occurs.
  virtual int readBuffer(void *buffer, long bufferSize, int flags = 0) = 0;

  //! Write a buffer via the specified socket.  Returns the number of bytes written or -1 if an error occurs.
  static int writeBuffer(int socket, const void *buffer, long bufferSize, int flags );

  //! Read a buffer via the specified socket.  Returns the number of bytes read or -1 if an error occurs.
  static int readBuffer(int socket, void *buffer, long bufferSize, int flags );

 protected:

  int soket_;
  int port_;

};

/***************************************************/
/*! \class Socket
    \brief STK internet socket abstract base class.

    This class provides common functionality for TCP and UDP internet
    socket server and client subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



inline Socket::Socket()
{
  soket_ = -1;
  port_ = -1;
}


inline Socket::~Socket()
{
  this->close( soket_ );

#if defined(__OS_WINDOWS__)

  WSACleanup();

#endif
}


inline void Socket::close( int socket )
{
  if ( !isValid( socket ) ) return;

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  ::close( socket );

#elif defined(__OS_WINDOWS__)

  ::closesocket( socket );

#endif
}


inline void Socket::setBlocking( int socket, bool enable )
{
  if ( !isValid( socket ) ) return;

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  int tmp = ::fcntl( socket, F_GETFL, 0 );
  if ( tmp >= 0 )
    tmp = ::fcntl( socket, F_SETFL, enable ? (tmp &~ O_NONBLOCK) : (tmp | O_NONBLOCK) );

#elif defined(__OS_WINDOWS__)

  unsigned long non_block = !enable;
  ioctlsocket( socket, FIONBIO, &non_block );

#endif
}


inline int Socket::writeBuffer(int socket, const void *buffer, long bufferSize, int flags )
{
  if ( !isValid( socket ) ) return -1;
  return send( socket, (const char *)buffer, bufferSize, flags );
}


inline int Socket::readBuffer(int socket, void *buffer, long bufferSize, int flags )
{
  if ( !isValid( socket ) ) return -1;
  return recv( socket, (char *)buffer, bufferSize, flags );
}

} // stk namespace

