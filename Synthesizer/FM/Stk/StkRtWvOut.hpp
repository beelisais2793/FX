#pragma once
#include "StkWvOut.hpp"
#include "RtAudio.h"
#include "StkMutex.hpp"
#include <cstring>

namespace stk {

/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio (blocking) output class.

    This class provides a simplified interface to RtAudio for realtime
    audio output.  It is a subclass of WvOut.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    into which data is written.  This class should not be used when
    low-latency is desired.

    RtWvOut supports multi-channel data in interleaved format.  It is
    important to distinguish the tick() method that outputs a single
    sample to all channels in a sample frame from the overloaded one
    that takes a reference to an StkFrames<T> object for multi-channel
    and/or multi-frame data.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class RtWvOut : public WvOut<T>
{
 public:

  //! Default constructor.
  /*!
    The default \e device argument value (zero) will select the
    default output device on your system.  The first device enumerated
    by the underlying audio API is specified with a value of one.  The
    default buffer size of RT_BUFFER_SIZE is defined in Stk.h.  An
    StkError will be thrown if an error occurs duing instantiation.
  */
  RtWvOut( unsigned int nChannels = 1, T sampleRate = stk::sampleRate(),
           int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 20 );

  //! Class destructor.
  ~RtWvOut();

  //! Start the audio output stream.
  /*!
    The stream is started automatically, if necessary, when a
    tick() method is called.
  */
  void start( void );

  //! Stop the audio output stream.
  /*!
    It may be necessary to use this method to avoid undesireable
    audio buffer cycling if you wish to temporarily stop audio output.
  */
  void stop( void );

  //! Output a single sample to all channels in a sample frame.
  /*!
    If the device is "stopped", it is "started".
  */
  void tick( const T sample );

  //! Output the StkFrames<T> data.
  /*!
    If the device is "stopped", it is "started".  The number of
    channels in the StkFrames<T> argument must equal the number of
    channels specified during instantiation.  However, this is only
    checked if _STK_DEBUG_ is defined during compilation, in which
    case an incompatibility will trigger an StkError exception.
  */
  void tick( const StkFrames<T>& frames );

  // This function is not intended for general use but must be
  // public for access from the audio callback function.
  int readBuffer( void *buffer, unsigned int frameCount );

 protected:

  RtAudio dac_;
  Mutex mutex_;
  bool stopped_;
  unsigned int readIndex_;
  unsigned int writeIndex_;
  long framesFilled_;
  unsigned int status_; // running = 0, emptying buffer = 1, finished = 2

};

/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio (blocking) output class.

    This class provides a simplified interface to RtAudio for realtime
    audio output.  It is a subclass of WvOut.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    into which data is written.  This class should not be used when
    low-latency is desired.

    RtWvOut supports multi-channel data in interleaved format.  It is
    important to distinguish the tick() method that outputs a single
    sample to all channels in a sample frame from the overloaded one
    that takes a reference to an StkFrames<T> object for multi-channel
    and/or multi-frame data.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/




// Streaming status states.
enum { RUNNING, EMPTYING, FINISHED };

// This function is automatically called by RtAudio to get audio data for output.
template<typename T>
int write( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
  return ( (RtWvOut<T> *) dataPointer )->readBuffer( outputBuffer, nBufferFrames );
}

// This function does not block.  If the user does not write output
// data to the buffer fast enough, previous data will be re-output
// (data underrun).
template<typename T>
int RtWvOut<T>::readBuffer( void *buffer, unsigned int frameCount )
{
  unsigned int nSamples, nChannels = this->data_.channels();
  unsigned int nFrames = frameCount;
  T *input = (T *) &this->data_[ readIndex_ * nChannels ];
  T *output = (T *) buffer;
  long counter;

  while ( nFrames > 0 ) {

    // I'm assuming that both the RtAudio and StkFrames<T> buffers
    // contain interleaved data.
    counter = nFrames;

    // Pre-increment read pointer and check bounds.
    readIndex_ += nFrames;
    if ( readIndex_ >= this->data_.frames() ) {
      counter -= readIndex_ - this->data_.frames();
      readIndex_ = 0;
    }

    // Copy data from the StkFrames<T> container.
    if ( status_ == EMPTYING && framesFilled_ <= counter ) {
      nSamples = framesFilled_ * nChannels;
      unsigned int i;
      for ( i=0; i<nSamples; i++ ) *output++ = *input++;
      nSamples = (counter - framesFilled_) * nChannels;
      for ( i=0; i<nSamples; i++ ) *output++ = 0.0;
      status_ = FINISHED;
      return 1;
    }
    else {
      nSamples = counter * nChannels;
      for ( unsigned int i=0; i<nSamples; i++ )
        *output++ = *input++;
    }

    nFrames -= counter;
  }

  mutex_.lock();
  framesFilled_ -= frameCount;
  mutex_.unlock();
  if ( framesFilled_ < 0 ) {
    framesFilled_ = 0;
    //    writeIndex_ = readIndex_;
    oStream_ << "RtWvOut: audio buffer underrun!";
    handleError( StkError::WARNING );
  }

  return 0;
}


template<typename T>
RtWvOut<T>::RtWvOut( unsigned int nChannels, T sampleRate, int device, int bufferFrames, int nBuffers )
  : stopped_( true ), readIndex_( 0 ), writeIndex_( 0 ), framesFilled_( 0 ), status_(0)
{
  // We'll let RtAudio deal with channel and sample rate limitations.
  RtAudio::StreamParameters parameters;
  if ( device == 0 )
    parameters.deviceId = dac_.getDefaultOutputDevice();
  else
    parameters.deviceId = device - 1;
  parameters.nChannels = nChannels;
  unsigned int size = bufferFrames;
  RtAudioFormat format = ( sizeof(T) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  // Open a stream and set the callback function.
  try {
    dac_.openStream( &parameters, NULL, format, (unsigned int)stk::sampleRate(), &size, &this->write, (void *)this );
  }
  catch ( RtAudioError &error ) {
    handleError( error.what(), StkError::AUDIO_SYSTEM );
  }

  this->data_.resize( size * nBuffers, nChannels );

  // Start writing half-way into buffer.
  writeIndex_ = (unsigned int ) (this->data_.frames() / 2.0);
  framesFilled_ = writeIndex_;
}

template<typename T>
RtWvOut<T>::~RtWvOut( void )
{
  // Change status flag to signal callback to clear the buffer and close.
  status_ = EMPTYING;
  while ( status_ != FINISHED && dac_.isStreamRunning() == true ) stk::sleep( 100 );
  dac_.closeStream();
}

template<typename T>
void RtWvOut<T>::start( void )
{
  if ( stopped_ ) {
    dac_.startStream();
    stopped_ = false;
  }
}

template<typename T>
void RtWvOut<T>::stop( void )
{
  if ( !stopped_ ) {
    dac_.stopStream();
    stopped_ = true;
  }
}

template<typename T>
void RtWvOut<T>::tick( const T sample )
{
  if ( stopped_ ) this->start();

  // Block until we have room for at least one frame of output this->data.
  while ( framesFilled_ == (long) this->data_.frames() ) stk::sleep( 1 );

  unsigned int nChannels = this->data_.channels();
  T input = sample;
  clipTest( input );
  unsigned long index = writeIndex_ * nChannels;
  for ( unsigned int j=0; j<nChannels; j++ )
    this->data_[index++] = input;

  mutex_.lock();
  framesFilled_++;
  mutex_.unlock();
  this->frameCounter_++;
  writeIndex_++;
  if ( writeIndex_ == this->data_.frames() )
    writeIndex_ = 0;
}


template<typename T>
void RtWvOut<T>::tick( const StkFrames<T>& frames )
{
#if defined(_STK_DEBUG_)
  if ( data_.channels() != frames.channels() ) {
    oStream_ << "RtWvOut::tick(): incompatible channel value in StkFrames<T> argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( stopped_ ) this->start();

  // See how much space we have and fill as much as we can ... if we
  // still have samples left in the frames object, then wait and
  // repeat.
  unsigned int framesEmpty, nFrames, bytes, framesWritten = 0;
  unsigned int nChannels = this->data_.channels();
  while ( framesWritten < frames.frames() ) {

    // Block until we have some room for output this->data.
    while ( framesFilled_ == (long) this->data_.frames() ) stk::sleep( 1 );
    framesEmpty = this->data_.frames() - framesFilled_;

    // Copy this->data in one chunk up to the end of the this->data buffer.
    nFrames = framesEmpty;
    if ( writeIndex_ + nFrames > this->data_.frames() )
      nFrames = this->data_.frames() - writeIndex_;
    if ( nFrames > frames.frames() - framesWritten )
      nFrames = frames.frames() - framesWritten;
    bytes = nFrames * nChannels * sizeof( T );
    T *samples = &this->data_[writeIndex_ * nChannels];
    StkFrames<T> *ins = (StkFrames<T> *) &frames;
    memcpy( samples, &(*ins)[framesWritten * nChannels], bytes );
    for ( unsigned int i=0; i<nFrames * nChannels; i++ ) clipTest( *samples++ );

    writeIndex_ += nFrames;
    if ( writeIndex_ == this->data_.frames() ) writeIndex_ = 0;

    framesWritten += nFrames;
    mutex_.lock();
    framesFilled_ += nFrames;
    mutex_.unlock();
    this->frameCounter_ += nFrames;
  }
}

} // stk namespace
