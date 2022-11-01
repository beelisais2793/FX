#pragma once
#include "StkFilter.hpp"

namespace stk {

/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This class implements a fractional-length digital delay-line using
    a first-order allpass filter.  If the delay and maximum length are
    not specified during instantiation, a fixed maximum length of 4095
    and a delay of 0.5 is set.

    An allpass filter has unity magnitude gain but variable phase
    delay properties, making it useful in achieving fractional delays
    without affecting a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay response, the
    minimum delay possible in this implementation is limited to a
    value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class DelayA : public Filter<T>
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and delay = 0.5.
  /*!
    An StkError will be thrown if the delay parameter is less than
    zero, the maximum delay parameter is less than one, or the delay
    parameter is greater than the maxDelay value.
   */  
  DelayA( T delay = 0.5, unsigned long maxDelay = 4095 );

  //! Class destructor.
  ~DelayA();

  //! Clears all internal states of the delay line.
  void clear( void );

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

  //! Set the delay-line length
  /*!
    The valid range for \e delay is from 0.5 to the maximum delay-line length.
  */
  void setDelay( T delay );

  //! Return the current delay-line length.
  T getDelay( void ) const { return delay_; };

  //! Return the value at \e tapDelay samples from the delay-line input.
  /*!
    The tap point is determined modulo the delay-line length and is
    relative to the last input value (i.e., a tapDelay of zero returns
    the last input value).
  */
  T tapOut( unsigned long tapDelay );

  //! Set the \e value at \e tapDelay samples from the delay-line input.
  void tapIn( T value, unsigned long tapDelay );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Return the value which will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  T nextOut( void );

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
  T delay_;
  T alpha_;
  T coeff_;
  T apInput_;
  T nextOutput_;
  bool doNextOut_;
};

template<typename T>
inline T DelayA<T>::nextOut( void )
{
  if ( doNextOut_ ) {
    // Do allpass interpolation delay.
    nextOutput_ = -coeff_ * this->lastFrame_[0];
    nextOutput_ += apInput_ + ( coeff_ * this->inputs_[outPoint_] );
    doNextOut_ = false;
  }

  return nextOutput_;
}

template<typename T>
inline T DelayA<T>::tick( T input )
{
  this->inputs_[inPoint_++] = input * this->gain_;

  // Increment input pointer modulo length.
  if ( inPoint_ == this->inputs_.size() )
    inPoint_ = 0;

  this->lastFrame_[0] = nextOut();
  doNextOut_ = true;

  // Save the allpass input and increment modulo length.
  apInput_ = this->inputs_[outPoint_++];
  if ( outPoint_ == this->inputs_.size() )
    outPoint_ = 0;

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& DelayA<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "DelayA::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[inPoint_++] = *samples * this->gain_;
    if ( inPoint_ == this->inputs_.size() ) inPoint_ = 0;
    *samples = nextOut();
    this->lastFrame_[0] = *samples;
    doNextOut_ = true;
    apInput_ = this->inputs_[outPoint_++];
    if ( outPoint_ == this->inputs_.size() ) outPoint_ = 0;
  }

  return frames;
}

template<typename T>
inline StkFrames<T>& DelayA<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "DelayA::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[inPoint_++] = *iSamples * this->gain_;
    if ( inPoint_ == this->inputs_.size() ) inPoint_ = 0;
    *oSamples = nextOut();
    this->lastFrame_[0] = *oSamples;
    doNextOut_ = true;
    apInput_ = this->inputs_[outPoint_++];
    if ( outPoint_ == this->inputs_.size() ) outPoint_ = 0;
  }

  return iFrames;
}

/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This class implements a fractional-length digital delay-line using
    a first-order allpass filter.  If the delay and maximum length are
    not specified during instantiation, a fixed maximum length of 4095
    and a delay of zero is set.

    An allpass filter has unity magnitude gain but variable phase
    delay properties, making it useful in achieving fractional delays
    without affecting a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay response, the
    minimum delay possible in this implementation is limited to a
    value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
DelayA<T>::DelayA( T delay, unsigned long maxDelay )
{
  if ( delay < 0.5 ) {
    oStream_ << "DelayA::DelayA: delay must be >= 0.5!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( delay > (T) maxDelay ) {
    oStream_ << "DelayA::DelayA: maxDelay must be > than delay argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // Writing before reading allows delays from 0 to length-1. 
  if ( maxDelay + 1 > this->inputs_.size() )
    this->inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setDelay( delay );
  apInput_ = 0.0;
  doNextOut_ = true;
}

template<typename T>
DelayA<T>::~DelayA()
{
}

template<typename T>
void DelayA<T>::clear()
{
  for ( unsigned int i=0; i<this->inputs_.size(); i++ )
    this->inputs_[i] = 0.0;
  this->lastFrame_[0] = 0.0;
  apInput_ = 0.0;
}

template<typename T>
void DelayA<T>::setMaximumDelay( unsigned long delay )
{
  if ( delay < this->inputs_.size() ) return;
  this->inputs_.resize(delay + 1, 1, 0.0);
}

template<typename T>
void DelayA<T>::setDelay( T delay )
{
  unsigned long length = this->inputs_.size();
  if ( delay + 1 > length ) { // The value is too big.
    oStream_ << "DelayA::setDelay: argument (" << delay << ") greater than maximum!";
    handleError( StkError::WARNING ); return;
  }

  if ( delay < 0.5 ) {
    oStream_ << "DelayA::setDelay: argument (" << delay << ") less than 0.5 not possible!";
    handleError( StkError::WARNING );
  }

  T outPointer = inPoint_ - delay + 1.0;     // outPoint chases inpoint
  delay_ = delay;

  while ( outPointer < 0 )
    outPointer += length;  // modulo maximum length

  outPoint_ = (long) outPointer;         // integer part
  if ( outPoint_ == length ) outPoint_ = 0;
  alpha_ = 1.0 + outPoint_ - outPointer; // fractional part

  if ( alpha_ < 0.5 ) {
    // The optimal range for alpha is about 0.5 - 1.5 in order to
    // achieve the flattest phase delay response.
    outPoint_ += 1;
    if ( outPoint_ >= length ) outPoint_ -= length;
    alpha_ += (T) 1.0;
  }

  coeff_ = (1.0 - alpha_) / (1.0 + alpha_);  // coefficient for allpass
}

template<typename T>
T DelayA<T>::tapOut( unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += this->inputs_.size();

  return this->inputs_[tap];
}

template<typename T>
void DelayA<T>::tapIn( T value, unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += this->inputs_.size();

  this->inputs_[tap] = value;
}

} // stk namespace

