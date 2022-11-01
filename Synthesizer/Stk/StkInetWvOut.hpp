#pragma once
#include "StkWvOut.hpp"
#include "StkSocket.hpp"
#include "StkTcpClient.hpp"
#include "StkUdpSocket.hpp"
#include <sstream>


namespace stk {

/***************************************************/
/*! \class InetWvOut
    \brief STK internet streaming output class.

    This WvOut subclass can stream data over a network via a TCP or
    UDP socket connection.  The data is converted to big-endian byte
    order, if necessary, before being transmitted.

    InetWvOut supports multi-channel data.  It is important to
    distinguish the tick() method that outputs a single sample to all
    channels in a sample frame from the overloaded one that takes a
    reference to an StkFrames<T> object for multi-channel and/or
    multi-frame data.

    This class connects to a socket server, the port and IP address of
    which must be specified as constructor arguments.  The default
    data type is signed 16-bit integers but any of the defined
    StkFormats are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class InetWvOut : public WvOut<T>
{
 public:
  //! Default constructor ... the socket is not instantiated.
  InetWvOut( unsigned long packetFrames = 1024 );

  //! Overloaded constructor which opens a network connection during instantiation.
  /*!
    An StkError is thrown if a socket error occurs or an invalid argument is specified.
  */
  InetWvOut( int port, ProtocolType protocol = PROTO_TCP,
             std::string hostname = "localhost", unsigned int nChannels = 1, StkFormat format = STK_SINT16,
             unsigned long packetFrames = 1024 );

  //! Class destructor.
  ~InetWvOut();

  //! Connect to the specified host and port and prepare to stream \e nChannels of data in the given data format.
  /*!
    An StkError is thrown if a socket error occurs or an invalid argument is specified.
  */
  void connect( int port, ProtocolType protocol = PROTO_TCP,
                std::string hostname = "localhost", unsigned int nChannels = 1, StkFormat format = STK_SINT16 );

  //! If a connection is open, write out remaining samples in the queue and then disconnect.
  void disconnect( void );

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if an output error occurs.  If a socket
    connection does not exist, the function does nothing (a warning
    will be issued if _STK_DEBUG_ is defined during compilation).
  */
  void tick( const T sample );

  //! Output the StkFrames<T> data.
  /*!
    An StkError will be thrown if an output error occurs.  An
    StkError will also be thrown if _STK_DEBUG_ is defined during
    compilation and there is an incompatability between the number of
    channels in the FileWvOut object and that in the StkFrames<T> object.
    If a socket connection does not exist, the function does nothing
    (a warning will be issued if _STK_DEBUG_ is defined during
    compilation).
  */
  void tick( const StkFrames<T>& frames );

 protected:

  void incrementFrame( void );

  // Write a buffer of length frames via the socket connection.
  void writeData( unsigned long frames );

  char *buffer_;
  Socket *soket_;
  unsigned long bufferFrames_;
  unsigned long bufferBytes_;
  unsigned long bufferIndex_;
  unsigned long iData_;
  unsigned int dataBytes_;
  StkFormat dataType_;
};

/***************************************************/
/*! \class InetWvOut
    \brief STK internet streaming output class.

    This WvOut subclass can stream data over a network via a TCP or
    UDP socket connection.  The data is converted to big-endian byte
    order, if necessary, before being transmitted.

    InetWvOut supports multi-channel data.  It is important to
    distinguish the tick() method that outputs a single sample to all
    channels in a sample frame from the overloaded one that takes a
    reference to an StkFrames<T> object for multi-channel and/or
    multi-frame data.

    This class connects to a socket server, the port and IP address of
    which must be specified as constructor arguments.  The default
    data type is signed 16-bit integers but any of the defined
    StkFormats are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
InetWvOut<T>::InetWvOut( unsigned long packetFrames )
  : buffer_(0), soket_(0), bufferFrames_(packetFrames), bufferBytes_(0)
{
}

template<typename T>
InetWvOut<T>::InetWvOut( int port, ProtocolType protocol, std::string hostname,
                        unsigned int nChannels, StkFormat format, unsigned long packetFrames )
  : buffer_(0), soket_(0), bufferFrames_(packetFrames), bufferBytes_(0)
{
  connect( port, protocol, hostname, nChannels, format );
}

template<typename T>
InetWvOut<T>::~InetWvOut()
{
  disconnect();
  if ( soket_ ) delete soket_;
  if ( buffer_ ) delete [] buffer_;
}


template<typename T>
void InetWvOut<T>::connect( int port, ProtocolType protocol, std::string hostname,
                           unsigned int nChannels, StkFormat format )
{
  if ( soket_ && soket_->isValid( soket_->id() ) )
    disconnect();

  if ( nChannels == 0 ) {
    oStream_ << "InetWvOut::connect: the channel argument must be greater than zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( format == STK_SINT8 ) dataBytes_ = 1;
  else if ( format == STK_SINT16 ) dataBytes_ = 2;
  else if ( format == STK_SINT32 || format == STK_FLOAT32 ) dataBytes_ = 4;
  else if ( format == STK_FLOAT64 ) dataBytes_ = 8;
  else {
    oStream_ << "InetWvOut::connect: unknown data type specified.";
    handleError( StkError::FUNCTION_ARGUMENT );
  } 
  dataType_ = format;

  if ( protocol == PROTO_TCP ) {
    soket_ = new TcpClient( port, hostname );
  }
  else {
    // For UDP sockets, the sending and receiving sockets cannot have
    // the same port number.  Since the port argument corresponds to
    // the destination port, we will associate this socket instance
    // with a different port number (arbitrarily determined as port -
    // 1).
    UdpSocket *socket = new UdpSocket( port - 1 );
    socket->setDestination( port, hostname );
    soket_ = (Socket*) socket;
  }

  // Allocate new memory if necessary.
  this->data_.resize( bufferFrames_, nChannels );
  unsigned long bufferBytes = dataBytes_ * bufferFrames_ * nChannels;
  if ( bufferBytes > bufferBytes_ ) {
    if ( buffer_) delete [] buffer_;
    buffer_ = (char *) new char[ bufferBytes ];
    bufferBytes_ = bufferBytes;
  }
  this->frameCounter_ = 0;
  bufferIndex_ = 0;
  iData_ = 0;
}

template<typename T>
void InetWvOut<T>::disconnect(void)
{
  if ( soket_ ) {
    writeData( bufferIndex_ );
    soket_->close( soket_->id() );
    delete soket_;
    soket_ = 0;
  }
}

template<typename T>
void InetWvOut<T>::writeData( unsigned long frames )
{
  unsigned long samples = frames * this->data_.channels();
  if ( dataType_ == STK_SINT8 ) {
    signed char *ptr = (signed char *) buffer_;
    for ( unsigned long k=0; k<samples; k++ ) {
      this->clipTest( this->data_[k] );
      *ptr++ = (signed char) (this->data_[k] * 127.0);
    }
  }
  else if ( dataType_ == STK_SINT16 ) {
    SINT16 *ptr = (SINT16 *) buffer_;
    for ( unsigned long k=0; k<samples; k++ ) {
      this->clipTest( this->data_[k] );
      *ptr = (SINT16) (this->data_[k] * 32767.0);
#ifdef __LITTLE_ENDIAN__
      swap16 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType_ == STK_SINT32 ) {
    SINT32 *ptr = (SINT32 *) buffer_;
    for ( unsigned long k=0; k<samples; k++ ) {
      this->clipTest( this->data_[k] );
      *ptr = (SINT32) (this->data_[k] * 2147483647.0);
#ifdef __LITTLE_ENDIAN__
      swap32 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType_ == STK_FLOAT32 ) {
    FLOAT32 *ptr = (FLOAT32 *) buffer_;
    for ( unsigned long k=0; k<samples; k++ ) {
      this->clipTest( this->data_[k] );
      *ptr = (FLOAT32) this->data_[k];
#ifdef __LITTLE_ENDIAN__
      swap32 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    FLOAT64 *ptr = (FLOAT64 *) buffer_;
    for ( unsigned long k=0; k<samples; k++ ) {
      this->clipTest( this->data_[k] );
      *ptr = (FLOAT64) this->data_[k];
#ifdef __LITTLE_ENDIAN__
      swap64 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }

  long bytes = dataBytes_ * samples;
  if ( soket_->writeBuffer( (const void *)buffer_, bytes, 0 ) < 0 ) {
    oStream_ << "InetWvOut: connection to socket server failed!";
    handleError( StkError::PROCESS_SOCKET );
  }
}

template<typename T>
void InetWvOut<T>::incrementFrame( void )
{
  this->frameCounter_++;
  bufferIndex_++;

  if ( bufferIndex_ == bufferFrames_ ) {
    writeData( bufferFrames_ );
    bufferIndex_ = 0;
    iData_ = 0;
  }
}

template<typename T>
void InetWvOut<T>::tick( const T sample )
{
  if ( !soket_ || !soket_->isValid( soket_->id() ) ) {
#if defined(_STK_DEBUG_)
    oStream_ << "InetWvOut::tick(): a valid socket connection does not exist!";
    handleError( StkError::DEBUG_PRINT );
#endif
    return;
  }

  unsigned int nChannels = this->data_.channels();
  T input = sample;
  this->clipTest( input );
  for ( unsigned int j=0; j<nChannels; j++ )
    this->data_[iData_++] = input;

  this->incrementFrame();
}

template<typename T>
void InetWvOut<T>::tick( const StkFrames<T>& frames )
{
  if ( !soket_ || !soket_->isValid( soket_->id() ) ) {
#if defined(_STK_DEBUG_)
    oStream_ << "InetWvOut::tick(): a valid socket connection does not exist!";
    handleError( StkError::DEBUG_PRINT );
#endif
    return;
  }

#if defined(_STK_DEBUG_)
  if ( this->data_.channels() != frames.channels() ) {
    oStream_ << "InetWvOut::tick(): incompatible channel value in StkFrames<T> argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  unsigned int j, nChannels = this->data_.channels();
  unsigned int iFrames = 0;
  for ( unsigned int i=0; i<frames.frames(); i++ ) {

    for ( j=0; j<nChannels; j++ ) {
      this->data_[iData_] = frames[iFrames++];
      this->clipTest( this->data_[iData_++] );
    }

    this->incrementFrame();
  }
}

} // stk namespace

