#pragma once
#include "StkSocket.hpp"
#include <cstring>
#include <sstream>

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
  //! Default constructor creates a local UDP socket on port 2006 (or the specified port number).
  /*!
    An StkError will be thrown if a socket error occurs during instantiation.
  */
  UdpSocket( int port = 2006 );

  //! The class destructor closes the socket instance.
  ~UdpSocket();

  //! Set the address for subsequent outgoing data sent via the \e writeBuffer() function.
  /*!
    An StkError will be thrown if the host is unknown.
  */
  void setDestination( int port = 2006, std::string hostname = "localhost" );

  //! Send a buffer to the address specified with the \e setDestination() function.  Returns the number of bytes written or -1 if an error occurs.
  /*!
    This function will fail if the default address (set with \e setDestination()) is invalid or has not been specified.
   */
  int writeBuffer(const void *buffer, long bufferSize, int flags = 0);

  //! Read an input buffer, up to length \e bufferSize.  Returns the number of bytes read or -1 if an error occurs.
  int readBuffer(void *buffer, long bufferSize, int flags = 0);

  //! Write a buffer to the specified socket.  Returns the number of bytes written or -1 if an error occurs.
  int writeBufferTo(const void *buffer, long bufferSize, int port, std::string hostname = "localhost", int flags = 0 );

 protected:

  //! A protected function for use in writing a socket address structure.
  /*!
    An StkError will be thrown if the host is unknown.
  */
  void setAddress( struct sockaddr_in *address, int port = 2006, std::string hostname = "localhost" );

  struct sockaddr_in address_;
  bool validAddress_;

};

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



inline
UdpSocket::UdpSocket(int port )
{
  validAddress_ = false;

#if defined(__OS_WINDOWS__)  // windoze-only stuff
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested) {
    oStream_ << "UdpSocket: Incompatible Windows socket library version!";
    handleError( StkError::PROCESS_SOCKET );
  }
#endif

  // Create the UDP socket
  this->soket_ = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
  if ( this->soket_ < 0 ) {
    oStream_ << "UdpSocket: Couldn't create UDP socket!";
    handleError( StkError::PROCESS_SOCKET );
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( port );

  // Bind socket to the appropriate port and interface (INADDR_ANY)
  if ( bind(this->soket_, (struct sockaddr *)&address, sizeof(address)) < 0 ) {
    oStream_ << "UdpSocket: Couldn't bind socket in constructor!";
    handleError( StkError::PROCESS_SOCKET );
  }

  this->port_ = port;
}

inline
UdpSocket::~UdpSocket()
{
}

inline
void UdpSocket::setDestination( int port, std::string hostname )
{
  this->setAddress( &address_, port, hostname );
  validAddress_ = true;
}

inline
void UdpSocket::setAddress( struct sockaddr_in *address, int port, std::string hostname )
{
  struct hostent *hostp;
  if ( (hostp = gethostbyname( hostname.c_str() )) == 0 ) {
    oStream_ << "UdpSocket::setAddress: unknown host (" << hostname << ")!";
    handleError( StkError::PROCESS_SOCKET_IPADDR );
  }

  // Fill in the address structure
  address->sin_family = AF_INET;
  memcpy((void *)&address->sin_addr, hostp->h_addr, hostp->h_length);
  address->sin_port = htons( port );
}

inline
int UdpSocket::writeBuffer( const void *buffer, long bufferSize, int flags )
{
  if ( !isValid( this->soket_ ) || !validAddress_ ) return -1;
  return sendto( this->soket_, (const char *)buffer, bufferSize, flags, (struct sockaddr *)&address_, sizeof(address_) );
}

inline
int UdpSocket::readBuffer( void *buffer, long bufferSize, int flags )
{
  if ( !isValid( this->soket_ ) ) return -1;
  return recvfrom( this->soket_, (char *)buffer, bufferSize, flags, NULL, NULL );
}

inline
int UdpSocket::writeBufferTo( const void *buffer, long bufferSize, int port, std::string hostname, int flags )
{
  if ( !isValid( this->soket_ ) ) return -1;
  struct sockaddr_in address;
  this->setAddress( &address, port, hostname );
  return sendto( this->soket_, (const char *)buffer, bufferSize, flags, (struct sockaddr *)&address, sizeof(address) );
}

} // stk namespace
