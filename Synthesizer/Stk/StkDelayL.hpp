#pragma once
#include "StkFilter.hpp"

namespace stk 
{
/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This class implements a fractional-length digital delay-line using
    first-order linear interpolation.  If the delay and maximum length
    are not specified during instantiation, a fixed maximum length of
    4095 and a delay of zero is set.

    Linear interpolation is an efficient technique for achieving
    fractional delay lengths, though it does introduce high-frequency
    signal attenuation to varying degrees depending on the fractional
    delay setting.  The use of higher order Lagrange interpolators can
    typically improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class DelayL : public Filter<T>
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  /*!
    An StkError will be thrown if the delay parameter is less than
    zero, the maximum delay parameter is less than one, or the delay
    parameter is greater than the maxDelay value.
   */  
  DelayL( T delay = 0.0, unsigned long maxDelay = 4095 );

  //! Class destructor.
  ~DelayL();

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

  //! Take a channel of the StkFrames object as inputs to the filter and replace with corresponding outputs.
  /*!
    The StkFrames argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the filter and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames argument (the first channel is specified
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
  T omAlpha_;
  T nextOutput_;
  bool doNextOut_;
};

template<typename T>
inline T DelayL<T>::nextOut( void )
{
  if ( doNextOut_ ) {
    // First 1/2 of interpolation
    nextOutput_ = this->inputs_[outPoint_] * omAlpha_;
    // Second 1/2 of interpolation
    if (outPoint_+1 < this->inputs_.size())
      nextOutput_ += this->inputs_[outPoint_+1] * alpha_;
    else
      nextOutput_ += this->inputs_[0] * alpha_;
    doNextOut_ = false;
  }

  return nextOutput_;
}

template<typename T>
inline void DelayL<T>::setDelay( T delay )
{
  if ( delay + 1 > this->inputs_.size() ) { // The value is too big.
    oStream_ << "DelayL::setDelay: argument (" << delay << ") greater than  maximum!";
    handleError( StkError::WARNING ); return;
  }

  if (delay < 0 ) {
    oStream_ << "DelayL::setDelay: argument (" << delay << ") less than zero!";
    handleError( StkError::WARNING ); return;
  }

  T outPointer = inPoint_ - delay;  // read chases write
  delay_ = delay;

  while ( outPointer < 0 )
    outPointer += this->inputs_.size(); // modulo maximum length

  outPoint_ = (long) outPointer;   // integer part

  alpha_ = outPointer - outPoint_; // fractional part
  omAlpha_ = (T) 1.0 - alpha_;

  if ( outPoint_ == this->inputs_.size() ) outPoint_ = 0;
  doNextOut_ = true;
}

template<typename T>
inline T DelayL<T>::tick( T input )
{
  this->inputs_[inPoint_++] = input * this->gain_;

  // Increment input pointer modulo length.
  if ( inPoint_ == this->inputs_.size() )
    inPoint_ = 0;

  this->lastFrame_[0] = nextOut();
  doNextOut_ = true;

  // Increment output pointer modulo length.
  if ( ++outPoint_ == this->inputs_.size() )
    outPoint_ = 0;

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& DelayL<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "DelayL::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[inPoint_++] = *samples * this->gain_;
    if ( inPoint_ == this->inputs_.size() ) inPoint_ = 0;
    *samples = nextOut();
    doNextOut_ = true;
    if ( ++outPoint_ == this->inputs_.size() ) outPoint_ = 0;
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& DelayL<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "DelayL::tick(): channel and StkFrames arguments are incompatible!";
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
    doNextOut_ = true;
    if ( ++outPoint_ == this->inputs_.size() ) outPoint_ = 0;
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This class implements a fractional-length digital delay-line using
    first-order linear interpolation.  If the delay and maximum length
    are not specified during instantiation, a fixed maximum length of
    4095 and a delay of zero is set.

    Linear interpolation is an efficient technique for achieving
    fractional delay lengths, though it does introduce high-frequency
    signal attenuation to varying degrees depending on the fractional
    delay setting.  The use of higher order Lagrange interpolators can
    typically improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
DelayL<T>::DelayL( T delay, unsigned long maxDelay )
{
  if ( delay < 0.0 ) {
    oStream_ << "DelayL::DelayL: delay must be >= 0.0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( delay > (T) maxDelay ) {
    oStream_ << "DelayL::DelayL: maxDelay must be > than delay argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // Writing before reading allows delays from 0 to length-1. 
  if ( maxDelay + 1 > this->inputs_.size() )
    this->inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setDelay( delay );
  doNextOut_ = true;
}

template<typename T>
DelayL<T>::~DelayL()
{
}

template<typename T>
void DelayL<T>::setMaximumDelay( unsigned long delay )
{
  if ( delay < this->inputs_.size() ) return;
  this->inputs_.resize(delay + 1, 1, 0.0);
}

template<typename T>
T DelayL<T>::tapOut( unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += this->inputs_.size();

  return this->inputs_[tap];
}

template<typename T>
void DelayL<T>::tapIn( T value, unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += this->inputs_.size();

  this->inputs_[tap] = value;
}

} // stk namespace

