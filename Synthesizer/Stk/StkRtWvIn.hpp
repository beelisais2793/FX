#pragma once
#include "StkWvIn.hpp"
#include "RtAudio.h"
#include "StkMutex.hpp"
#include <cstring>

namespace stk {

/***************************************************/
/*! \class RtWvIn
    \brief STK realtime audio (blocking) input class.

    This class provides a simplified interface to RtAudio for realtime
    audio input.  It is a subclass of WvIn.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    from which data is read.  This class should not be used when
    low-latency is desired.

    RtWvIn supports multi-channel data in both interleaved and
    non-interleaved formats.  It is important to distinguish the
    tick() method that computes a single frame (and returns only the
    specified sample of a multi-channel frame) from the overloaded one
    that takes an StkFrames<T> object for multi-channel and/or
    multi-frame data.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class RtWvIn : public WvIn<T>
{
public:
  //! Default constructor.
  /*!
    The default \e device argument value (zero) will select the
    default input device on your system.  The first device enumerated
    by the underlying audio API is specified with a value of one.  The
    default buffer size of RT_BUFFER_SIZE is defined in Stk.h.  An
    StkError will be thrown if an error occurs duing instantiation.
  */
  RtWvIn( unsigned int nChannels = 1, T sampleRate = stk::sampleRate(),
          int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 20 );

  //! Class destructor.
  ~RtWvIn();

  //! Start the audio input stream.
  /*!
    The stream is started automatically, if necessary, when a
    tick() or tickFrame() method is called.
  */
  void start( void );

  //! Stop the audio input stream.
  /*!
    It may be necessary to use this method to avoid audio underflow
    problems if you wish to temporarily stop audio input.
  */
  void stop( void );

  //! Return the specified channel value of the last computed frame.
  /*!
    For multi-channel files, use the this->lastFrame() function to get
    all values from the last computed frame.  If the device is
    stopped, the returned value is 0.0.  The \c channel argument must
    be less than the number of channels in the audio stream (the first
    channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  T lastOut( unsigned int channel = 0 );

  //! Compute a sample frame and return the specified \c channel value.
  /*!
    For multi-channel files, use the this->lastFrame() function to get
    all values from the computed frame.  If the device is "stopped",
    it is "started".  The \c channel argument must be less than the
    number of channels in the audio stream (the first channel is
    specified by 0).  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  T tick( unsigned int channel = 0 );

  //! Fill the StkFrames<T> object with computed sample frames, starting at the specified channel and return the same reference.
  /*!
    If the device is "stopped", it is "started".  The \c channel
    argument plus the number of input channels must be less than the
    number of channels in the StkFrames<T> argument (the first channel is
    specified by 0).  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  // This function is not intended for general use but must be
  // public for access from the audio callback function.
  void fillBuffer( void *buffer, unsigned int nFrames );

protected:

  RtAudio adc_;
  Mutex mutex_;
  bool stopped_;
  unsigned int readIndex_;
  unsigned int writeIndex_;
  unsigned int framesFilled_;

};

template<typename T>
inline T RtWvIn<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= data_.channels() ) {
    oStream_ << "RtWvIn::lastOut(): channel argument and audio stream are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return this->lastFrame_[channel];
}

/***************************************************/
/*! \class RtWvIn
    \brief STK realtime audio (blocking) input class.

    This class provides a simplified interface to RtAudio for realtime
    audio input.  It is a subclass of WvIn.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    from which data is read.  This class should not be used when
    low-latency is desired.

    RtWvIn supports multi-channel data in both interleaved and
    non-interleaved formats.  It is important to distinguish the
    tick() method that computes a single frame (and returns only the
    specified sample of a multi-channel frame) from the overloaded one
    that takes an StkFrames<T> object for multi-channel and/or
    multi-frame data.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



// This function is automatically called by RtAudio to supply input audio data.
template<typename T>
int read( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
          double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
  ( (RtWvIn<T>*) dataPointer )->fillBuffer( inputBuffer, nBufferFrames );
  return 0;
}

// This function does not block.  If the user does not read the buffer
// data fast enough, unread data will be overwritten (data overrun).
template<typename T>
void RtWvIn<T>::fillBuffer( void *buffer, unsigned int nFrames )
{
  T *samples = (T *) buffer;
  unsigned int counter, iStart, nSamples = nFrames * this->data_.channels();

  while ( nSamples > 0 ) {

    // I'm assuming that both the RtAudio and StkFrames<T> buffers
    // contain interleaved data.
    iStart = writeIndex_ * this->data_.channels();
    counter = nSamples;

    // Pre-increment write pointer and check bounds.
    writeIndex_ += nSamples / this->data_.channels();
    if ( writeIndex_ >= this->data_.frames() ) {
      writeIndex_ = 0;
      counter = this->data_.size() - iStart;
    }

    // Copy data to the StkFrames<T> container.
    for ( unsigned int i=0; i<counter; i++ )
      this->data_[iStart++] = *samples++;

    nSamples -= counter;
  }

  mutex_.lock();
  framesFilled_ += nFrames;
  mutex_.unlock();
  if ( framesFilled_ > this->data_.frames() ) {
    framesFilled_ = this->data_.frames();
    oStream_ << "RtWvIn: audio buffer overrun!";
    handleError( StkError::WARNING );
  }
}

template<typename T>
RtWvIn<T>::RtWvIn( unsigned int nChannels, T sampleRate, int device, int bufferFrames, int nBuffers )
  : stopped_( true ), readIndex_( 0 ), writeIndex_( 0 ), framesFilled_( 0 )
{
  // We'll let RtAudio deal with channel and sample rate limitations.
  RtAudio::StreamParameters parameters;
  if ( device == 0 )
    parameters.deviceId = adc_.getDefaultInputDevice();
  else
    parameters.deviceId = device - 1;
  parameters.nChannels = nChannels;
  unsigned int size = bufferFrames;
  RtAudioFormat format = ( sizeof(T) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  try {
    adc_.openStream( NULL, &parameters, format, (unsigned int)stk::sampleRate(), &size, &this->read, (void *)this );
  }
  catch ( RtAudioError &error ) {
    handleError( error.what(), StkError::AUDIO_SYSTEM );
  }

  this->data_.resize( size * nBuffers, nChannels );
  this->lastFrame_.resize( 1, nChannels );
}

template<typename T>
RtWvIn<T>::~RtWvIn()
{
  if ( !stopped_ ) adc_.stopStream();
  adc_.closeStream();
}

template<typename T>
void RtWvIn<T>::start()
{
  if ( stopped_ ) {
    adc_.startStream();
    stopped_ = false;
  }
}

template<typename T>
void RtWvIn<T>::stop(
)
{
  if ( !stopped_ ) {
    adc_.stopStream();
    stopped_ = true;
    for ( unsigned int i=0; i<this->lastFrame_.size(); i++ ) this->lastFrame_[i] = 0.0;
  }
}

template<typename T>
T RtWvIn<T>::tick( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= data_.channels() ) {
    oStream_ << "RtWvIn::tick(): channel argument is incompatible with streamed channels!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( stopped_ ) this->start();

  // Block until at least one frame is available.
  while ( framesFilled_ == 0 ) stk::sleep( 1 );

  unsigned long index = readIndex_ * this->lastFrame_.channels();
  for ( unsigned int i=0; i<this->lastFrame_.size(); i++ )
    this->lastFrame_[i] = this->data_[index++];

  mutex_.lock();
  framesFilled_--;
  mutex_.unlock();
  readIndex_++;
  if ( readIndex_ >= this->data_.frames() )
    readIndex_ = 0;

  return this->lastFrame_[channel];
}

template<typename T>
StkFrames<T>& RtWvIn<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "RtWvIn::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( stopped_ ) this->start();

  // See how much space we have and fill as much as we can ... if we
  // still have space left in the frames object, then wait and repeat.
  unsigned int nFrames, bytes, framesRead = 0;
  while ( framesRead < frames.frames() ) {

    // Block until we have some input data.
    while ( framesFilled_ == 0 ) stk::sleep( 1 );

    // Copy data in one chunk up to the end of the data buffer.
    nFrames = framesFilled_;
    if ( readIndex_ + nFrames > this->data_.frames() )
      nFrames = this->data_.frames() - readIndex_;
    if ( nFrames > frames.frames() - framesRead )
      nFrames = frames.frames() - framesRead;
    bytes = nFrames * nChannels * sizeof( T );
    T *samples = &this->data_[readIndex_ * nChannels];
    unsigned int hop = frames.channels() - nChannels;
    if ( hop == 0 ) 
      memcpy( &frames[framesRead * nChannels], samples, bytes );
    else {
      T *fSamples = &frames[channel];
      unsigned int j;
      for ( unsigned int i=0; i<frames.frames(); i++, fSamples += hop ) {
        for ( j=1; j<nChannels; j++ )
          *fSamples++ = *samples++;
      }
    }

    readIndex_ += nFrames;
    if ( readIndex_ == this->data_.frames() ) readIndex_ = 0;

    framesRead += nFrames;
    mutex_.lock();
    framesFilled_ -= nFrames;
    mutex_.unlock();
  }

  unsigned long index = (frames.frames() - 1) * nChannels;
  for ( unsigned int i=0; i<this->lastFrame_.size(); i++ )
    this->lastFrame_[i] = frames[channel+index++];

  return frames;
}

} // stk namespace
