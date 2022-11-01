#pragma once
#include "StkFilter.hpp"

namespace stk {

/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This class implements a non-interpolating digital delay-line.  If
    the delay and maximum length are not specified during
    instantiation, a fixed maximum length of 4095 and a delay of zero
    is set.
    
    A non-interpolating delay line is typically used in fixed
    delay-length applications, such as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Delay : public Filter<T>
{
public:

  //! The default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  /*!
    An StkError will be thrown if the delay parameter is less than
    zero, the maximum delay parameter is less than one, or the delay
    parameter is greater than the maxDelay value.
   */
  Delay( unsigned long delay = 0, unsigned long maxDelay = 4095 );

  //! Class destructor.
  ~Delay();

  //! Get the maximum delay-line length.
  unsigned long getMaximumDelay( void ) { return this->inputs_.size() - 1; };

  //! Set the maximum delay-line length.
  /*!
    This method should generally only be used during initial setup
    of the delay line.  If it is used between calls to the tick()
    function, without a call to clear(), a signal discontinuity will
    likely occur.  If the current maximum length is greater than the
    new length, no memory allocation change is made.
  */
  void setMaximumDelay( unsigned long delay );

  //! Set the delay-line length.
  /*!
    The valid range for \e delay is from 0 to the maximum delay-line length.
  */
  void setDelay( unsigned long delay );

  //! Return the current delay-line length.
  unsigned long getDelay( void ) const { return delay_; };

  //! Return the value at \e tapDelay samples from the delay-line input.
  /*!
    The tap point is determined modulo the delay-line length and is
    relative to the last input value (i.e., a tapDelay of zero returns
    the last input value).
  */
  T tapOut( unsigned long tapDelay );

  //! Set the \e value at \e tapDelay samples from the delay-line input.
  void tapIn( T value, unsigned long tapDelay );

  //! Sum the provided \e value into the delay line at \e tapDelay samples from the input.
  /*!
    The new value is returned.  The tap point is determined modulo
    the delay-line length and is relative to the last input value
    (i.e., a tapDelay of zero sums into the last input value).
  */
  T addTo( T value, unsigned long tapDelay );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Return the value that will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  T nextOut( void ) { return this->inputs_[outPoint_]; };

  //! Calculate and return the signal energy in the delay-line.
  T energy( void ) const;

  //! Input one sample to the filter and return one output.
  T tick( T input );

  //! Take a channel of the StkFrames<T> object as inputs to the filter and replace with corresponding outputs.
  /*!
    The StkFrames<T> argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames<T> argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the filter and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

protected:

  unsigned long inPoint_;
  unsigned long outPoint_;
  unsigned long delay_;
};

template<typename T>
inline T Delay<T>::tick( T input )
{
  this->inputs_[inPoint_++] = input * this->gain_;

  // Check for end condition
  if ( inPoint_ == this->inputs_.size() )
    inPoint_ = 0;

  // Read out next value
  this->lastFrame_[0] = this->inputs_[outPoint_++];

  if ( outPoint_ == this->inputs_.size() )
    outPoint_ = 0;

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Delay<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Delay::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[inPoint_++] = *samples * this->gain_;
    if ( inPoint_ == this->inputs_.size() ) inPoint_ = 0;
    *samples = this->inputs_[outPoint_++];
    if ( outPoint_ == this->inputs_.size() ) outPoint_ = 0;
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& Delay<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "Delay::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[inPoint_++] = *iSamples * this->gain_;
    if ( inPoint_ == this->inputs_.size() ) inPoint_ = 0;
    *oSamples = this->inputs_[outPoint_++];
    if ( outPoint_ == this->inputs_.size() ) outPoint_ = 0;
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This class implements a non-interpolating digital delay-line.  If
    the delay and maximum length are not specified during
    instantiation, a fixed maximum length of 4095 and a delay of zero
    is set.
    
    A non-interpolating delay line is typically used in fixed
    delay-length applications, such as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Delay<T>::Delay( unsigned long delay, unsigned long maxDelay )
{
  // Writing before reading allows delays from 0 to length-1. 
  // If we want to allow a delay of maxDelay, we need a
  // delay-line of length = maxDelay+1.
  if ( delay > maxDelay ) {
    oStream_ << "Delay::Delay: maxDelay must be > than delay argument!\n";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( ( maxDelay + 1 ) > this->inputs_.size() )
    this->inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setDelay( delay );
}

template<typename T>
Delay<T>::~Delay()
{
}

template<typename T>
void Delay<T>::setMaximumDelay( unsigned long delay )
{
  if ( delay < this->inputs_.size() ) return;
  this->inputs_.resize( delay + 1, 1, 0.0 );
}

template<typename T>
void Delay<T>::setDelay( unsigned long delay )
{
  if ( delay > this->inputs_.size() - 1 ) { // The value is too big.
    oStream_ << "Delay::setDelay: argument (" << delay << ") greater than maximum!\n";
    handleError( StkError::WARNING ); return;
  }

  // read chases write
  if ( inPoint_ >= delay ) outPoint_ = inPoint_ - delay;
  else outPoint_ = this->inputs_.size() + inPoint_ - delay;
  delay_ = delay;
}

template<typename T>
T Delay<T>::energy( void ) const
{
  unsigned long i;
  T e = 0;
  if ( inPoint_ >= outPoint_ ) {
    for ( i=outPoint_; i<inPoint_; i++ ) {
      T t = this->inputs_[i];
      e += t*t;
    }
  } else {
    for ( i=outPoint_; i<this->inputs_.size(); i++ ) {
      T t = this->inputs_[i];
      e += t*t;
    }
    for ( i=0; i<inPoint_; i++ ) {
      T t = this->inputs_[i];
      e += t*t;
    }
  }
  return e;
}

template<typename T>
T Delay<T>::tapOut( unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += this->inputs_.size();

  return this->inputs_[tap];
}

template<typename T>
void Delay<T>::tapIn( T value, unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += this->inputs_.size();

  this->inputs_[tap] = value;
}

template<typename T>
T Delay<T>::addTo( T value, unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += this->inputs_.size();

  return this->inputs_[tap]+= value;
}

} // stk namespace

