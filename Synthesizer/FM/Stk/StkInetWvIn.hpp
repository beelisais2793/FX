#pragma once
#include "StkWvIn.hpp"
#include "StkTcpServer.hpp"
#include "StkUdpSocket.hpp"
#include "StkThread.hpp"
#include "StkMutex.hpp"
#include <sstream>

namespace stk {

/***************************************************/
/*! \class InetWvIn
    \brief STK internet streaming input class.

    This Wvin subclass reads streamed audio data over a network via a
    TCP or UDP socket connection.  The data is assumed in big-endian,
    or network, byte order.  Only a single socket connection is
    supported.

    InetWvIn supports multi-channel data.  It is important to
    distinguish the tick() method that computes a single frame (and
    returns only the specified sample of a multi-channel frame) from
    the overloaded one that takes an StkFrames<T> object for
    multi-channel and/or multi-frame data.

    This class implements a socket server.  When using the TCP
    protocol, the server "listens" for a single remote connection
    within the InetWvIn::start() function.  For the UDP protocol, no
    attempt is made to verify packet delivery or order.  The default
    data type for the incoming stream is signed 16-bit integers,
    though any of the defined StkFormats are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

typedef struct {
  bool finished;
  void *object;
} ThreadInfo;

template<typename T>
class InetWvIn : public WvIn<T>
{
public:
  //! Default constructor.
  /*!
    An StkError will be thrown if an error occurs while initializing the input thread.
  */
  InetWvIn( unsigned long bufferFrames = 1024, unsigned int nBuffers = 8 );

  //! Class destructor.
  ~InetWvIn();

  //! Wait for a (new) socket connection with specified protocol, port, data channels and format.
  /*!
    For the UDP protocol, this function will create a socket
    instance and return.  For the TCP protocol, this function will
    block until a connection is established.  An StkError will be
    thrown if a socket error occurs or an invalid function argument is
    provided.
  */
  void listen( int port = 2006, unsigned int nChannels = 1,
               StkFormat format = STK_SINT16,
               ProtocolType protocol = PROTO_TCP );

  //! Returns true is an input connection exists or input data remains in the queue.
  /*!
    This method will not return false after an input connection has been closed until
    all buffered input data has been read out.
  */
  bool isConnected( void );

  //! Return the specified channel value of the last computed frame.
  /*!
    For multi-channel files, use the lastFrame() function to get
    all values from the last computed frame.  If no connection exists,
    the returned value is 0.0.  The \c channel argument must be less
    than the number of channels in the data stream (the first channel
    is specified by 0).  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  T lastOut( unsigned int channel = 0 );

  //! Compute a sample frame and return the specified \c channel value.
  /*!
    For multi-channel files, use the lastFrame() function to get
    all values from the computed frame.  If no connection exists, the
    returned value is 0.0 (and a warning will be issued if _STK_DEBUG_
    is defined during compilation).  The \c channel argument must be
    less than the number of channels in the data stream (the first
    channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  T tick( unsigned int channel = 0 );

  //! Fill the StkFrames<T> object with computed sample frames, starting at the specified channel and return the same reference.
  /*!
    The \c channel argument plus the number of channels specified
    in the listen() function must be less than the number of channels
    in the StkFrames<T> argument (the first channel is specified by 0).
    However, this is only checked if _STK_DEBUG_ is defined during
    compilation, in which case an incompatibility will trigger an
    StkError exception.  If no connection exists, the function does
    nothing (a warning will be issued if _STK_DEBUG_ is defined during
    compilation).
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  // Called by the thread routine to receive data via the socket connection
  // and fill the socket buffer.  This is not intended for general use but
  // must be public for access from the thread.
  void receive( void );

protected:

  // Read buffered socket data into the data buffer ... will block if none available.
  int readData( void );

  Socket *soket_;
  Thread thread_;
  Mutex mutex_;
  char *buffer_;
  unsigned long bufferFrames_;
  unsigned long bufferBytes_;
  unsigned long bytesFilled_;
  unsigned int nBuffers_;
  unsigned long writePoint_;
  unsigned long readPoint_;
  long bufferCounter_;
  int dataBytes_;
  bool connected_;
  int fd_;
  ThreadInfo threadInfo_;
  StkFormat dataType_;

};

template<typename T>
inline T InetWvIn<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= this->data_.channels() ) {
    oStream_ << "InetWvIn::lastOut(): channel argument and data stream are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  // If no connection and we've output all samples in the queue, return.
  if ( !connected_ && bytesFilled_ == 0 && bufferCounter_ == 0 ) return 0.0;

  return this->lastFrame_[channel];
}

/***************************************************/
/*! \class InetWvIn
    \brief STK internet streaming input class.

    This Wvin subclass reads streamed audio data over a network via a
    TCP or UDP socket connection.  The data is assumed in big-endian,
    or network, byte order.  Only a single socket connection is
    supported.

    InetWvIn supports multi-channel data.  It is important to
    distinguish the tick() method that computes a single frame (and
    returns only the specified sample of a multi-channel frame) from
    the overloaded one that takes an StkFrames<T> object for
    multi-channel and/or multi-frame data.

    This class implements a socket server.  When using the TCP
    protocol, the server "listens" for a single remote connection
    within the InetWvIn::start() function.  For the UDP protocol, no
    attempt is made to verify packet delivery or order.  The default
    data type for the incoming stream is signed 16-bit integers,
    though any of the defined StkFormats are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T> THREAD_RETURN THREAD_TYPE inputThread( void * ptr )
{
  ThreadInfo *info = (ThreadInfo *)ptr;

  while ( !info->finished ) {
    ((InetWvIn<T> *) info->object)->receive();
  }

  return 0;
}


template<typename T>
InetWvIn<T>::InetWvIn( unsigned long bufferFrames, unsigned int nBuffers )
  :soket_(0), buffer_(0), bufferFrames_(bufferFrames), bufferBytes_(0), nBuffers_(nBuffers), connected_(false)
{
  threadInfo_.finished = false;
  threadInfo_.object = (void *) this;

  // Start the input thread.
  if ( !thread_.start( &inputThread<T>, &threadInfo_ ) ) {
    oStream_ << "InetWvIn(): unable to start input thread in constructor!";
    handleError( StkError::PROCESS_THREAD );
  }
}

template<typename T>
InetWvIn<T>::~InetWvIn()
{
  // Close down the thread.
  connected_ = false;
  threadInfo_.finished = true;

  if ( soket_ ) delete soket_;
  if ( buffer_ ) delete [] buffer_;
}

template<typename T>
void InetWvIn<T>::listen( int port, unsigned int nChannels,
                         StkFormat format, ProtocolType protocol )
{
  mutex_.lock();

  if ( connected_ ) delete soket_;

  if ( nChannels < 1 ) {
    oStream_ << "InetWvIn()::listen(): the channel argument must be greater than zero.";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( format == STK_SINT16 ) dataBytes_ = 2;
  else if ( format == STK_SINT32 || format == STK_FLOAT32 ) dataBytes_ = 4;
  else if ( format == STK_FLOAT64 ) dataBytes_ = 8;
  else if ( format == STK_SINT8 ) dataBytes_ = 1;
  else {
    oStream_ << "InetWvIn(): unknown data type specified!";
    handleError( StkError::FUNCTION_ARGUMENT );
  } 
  dataType_ = format;

  unsigned long bufferBytes = bufferFrames_ * nBuffers_ * nChannels * dataBytes_;
  if ( bufferBytes > bufferBytes_ ) {
    if ( buffer_) delete [] buffer_;
    buffer_ = (char *) new char[ bufferBytes ];
    bufferBytes_ = bufferBytes;
  }

  this->data_.resize( bufferFrames_, nChannels );
  this->lastFrame_.resize( 1, nChannels, 0.0 );

  bufferCounter_ = 0;
  writePoint_ = 0;
  readPoint_ = 0;
  bytesFilled_ = 0;

  if ( protocol == PROTO_TCP ) {
    TcpServer *socket = new TcpServer( port );
    oStream_ << "InetWvIn:listen(): waiting for TCP connection on port " << socket->port() << " ... ";
    handleError( StkError::STATUS );
    fd_ = socket->accept();
    if ( fd_ < 0) {
      oStream_ << "InetWvIn::listen(): Error accepting TCP connection request!";
      handleError( StkError::PROCESS_SOCKET );
    }
    oStream_ << "InetWvIn::listen(): TCP socket connection made!";
    handleError( StkError::STATUS );
    soket_ = (Socket*) socket;
  }
  else {
    soket_ = new UdpSocket( port );
    fd_ = soket_->id();
  }

  connected_ = true;

  mutex_.unlock();
}

template<typename T>
void InetWvIn<T>::receive( void )
{
  if ( !connected_ ) {
    stk::sleep(100);
    return;
  }

  fd_set mask;
  FD_ZERO( &mask );
  FD_SET( fd_, &mask );

  // The select function will block until data is available for reading.
  select( fd_+1, &mask, (fd_set *)0, (fd_set *)0, NULL );

  if ( FD_ISSET( fd_, &mask ) ) {
    mutex_.lock();
    unsigned long unfilled = bufferBytes_ - bytesFilled_;
    if ( unfilled > 0 ) {
      // There's room in our buffer for more data.
      unsigned long endPoint = writePoint_ + unfilled;
      if ( endPoint > bufferBytes_ ) unfilled -= endPoint - bufferBytes_;
      int i = soket_->readBuffer( fd_, (void *)&buffer_[writePoint_], unfilled, 0 );
      //int i = Socket::readBuffer( fd_, (void *)&buffer_[writePoint_], unfilled, 0 );
      if ( i <= 0 ) {
        oStream_ << "InetWvIn::receive(): the remote InetWvIn socket has closed.";
        handleError( StkError::STATUS );
        connected_ = false;
        mutex_.unlock();
        return;
      }
      bytesFilled_ += i;
      writePoint_ += i;
      if ( writePoint_ == bufferBytes_ )
        writePoint_ = 0;
      mutex_.unlock();
    }
    else {
      // Sleep 10 milliseconds AFTER unlocking mutex.
      mutex_.unlock();
      stk::sleep( 10 );
    }
  }
}

template<typename T>
int InetWvIn<T>::readData( void )
{
  // We have two potential courses of action should this method
  // be called and the input buffer isn't sufficiently filled.
  // One solution is to fill the data buffer with zeros and return.
  // The other solution is to wait until the necessary data exists.
  // I chose the latter, as it works for both streamed files
  // (non-realtime data transport) and realtime playback (given
  // adequate network bandwidth and speed).

  // Wait until data is ready.
  unsigned long bytes = this->data_.size() * dataBytes_;
  while ( connected_ && bytesFilled_ < bytes )
    stk::sleep( 10 );

  if ( !connected_ && bytesFilled_ == 0 ) return 0;
  bytes = ( bytesFilled_ < bytes ) ? bytesFilled_ : bytes;

  // Copy samples from buffer to data.
  T gain;
  long samples = bytes / dataBytes_;
  mutex_.lock();
  if ( dataType_ == STK_SINT16 ) {
    gain = 1.0 / 32767.0;
    SINT16 *buf = (SINT16 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap16((unsigned char *) buf);
#endif
      this->data_[i] = (T) *buf++;
      this->data_[i] *= gain;
    }
  }
  else if ( dataType_ == STK_SINT32 ) {
    gain = 1.0 / 2147483647.0;
    SINT32 *buf = (SINT32 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap32((unsigned char *) buf);
#endif
      this->data_[i] = (T) *buf++;
      this->data_[i] *= gain;
    }
  }
  else if ( dataType_ == STK_FLOAT32 ) {
    FLOAT32 *buf = (FLOAT32 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap32((unsigned char *) buf);
#endif
      this->data_[i] = (T) *buf++;
    }
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    FLOAT64 *buf = (FLOAT64 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap64((unsigned char *) buf);
#endif
      this->data_[i] = (T) *buf++;
    }
  }
  else if ( dataType_ == STK_SINT8 ) {
    gain = 1.0 / 127.0;
    signed char *buf = (signed char *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
      this->data_[i] = (T) *buf++;
      this->data_[i] *= gain;
    }
  }

  readPoint_ += bytes;
  if ( readPoint_ == bufferBytes_ )
    readPoint_ = 0;
  bytesFilled_ -= bytes;

  mutex_.unlock();
  return samples / this->data_.channels();
}

template<typename T>
bool InetWvIn<T>::isConnected( void )
{
  if ( bytesFilled_ > 0 || bufferCounter_ > 0 )
    return true;
  else
    return connected_;
}

template<typename T>
T InetWvIn<T>::tick( unsigned int channel )
{
  // If no connection and we've output all samples in the queue, return 0.0.
  if ( !connected_ && bytesFilled_ == 0 && bufferCounter_ == 0 ) {
#if defined(_STK_DEBUG_)
    oStream_ << "InetWvIn::tick(): a valid socket connection does not exist!";
    handleError( StkError::DEBUG_PRINT );
#endif
    return 0.0;
  }

#if defined(_STK_DEBUG_)
  if ( channel >= this->data_.channels() ) {
    oStream_ << "InetWvIn::tick(): channel argument is incompatible with data stream!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( bufferCounter_ == 0 )
    bufferCounter_ = readData();

  unsigned int nChannels = this->lastFrame_.channels();
  long index = ( bufferFrames_ - bufferCounter_ ) * nChannels;
  for ( unsigned int i=0; i<nChannels; i++ )
    this->lastFrame_[i] = this->data_[index++];

  bufferCounter_--;
  if ( bufferCounter_ < 0 )
    bufferCounter_ = 0;

  return this->lastFrame_[channel];
}



template<typename T>
StkFrames<T>& InetWvIn<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - this->data_.channels() ) {
    oStream_ << "InetWvIn::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  // If no connection and we've output all samples in the queue, return.
  if ( !connected_ && bytesFilled_ == 0 && bufferCounter_ == 0 ) {
#if defined(_STK_DEBUG_)
    oStream_ << "InetWvIn::tick(): a valid socket connection does not exist!";
    handleError( StkError::DEBUG_PRINT );
#endif
    return frames;
  }

  T *samples = &frames[channel];
  unsigned int j, hop = frames.channels() - this->data_.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->tick();
    for ( j=0; j<this->lastFrame_.channels(); j++ )
      *samples++ = this->lastFrame_[j];
  }

  return frames;
}

} // stk namespace

