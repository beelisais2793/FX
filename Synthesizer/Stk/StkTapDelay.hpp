#pragma once
#include "StkFilter.hpp"

namespace stk {

/***************************************************/
/*! \class TapDelay
    \brief STK non-interpolating tapped delay line class.

    This class implements a non-interpolating digital delay-line with
    an arbitrary number of output "taps".  If the maximum length and
    tap delays are not specified during instantiation, a fixed maximum
    length of 4095 and a single tap delay of zero is set.
    
    A non-interpolating delay line is typically used in fixed
    delay-length applications, such as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class TapDelay : public Filter<T>
{
 public:

  //! The default constructor creates a delay-line with maximum length of 4095 samples and a single tap at delay = 0.
  /*!
    An StkError will be thrown if any tap delay parameter is less
    than zero, the maximum delay parameter is less than one, or any
    tap delay parameter is greater than the maxDelay value.
   */
  TapDelay( std::vector<unsigned long> taps = std::vector<unsigned long>( 1, 0 ), unsigned long maxDelay = 4095 );

  //! Class destructor.
  ~TapDelay();

  //! Set the maximum delay-line length.
  /*!
    This method should generally only be used during initial setup
    of the delay line.  If it is used between calls to the tick()
    function, without a call to clear(), a signal discontinuity will
    likely occur.  If the current maximum length is greater than the
    new length, no change will be made.
  */
  void setMaximumDelay( unsigned long delay );

  //! Set the delay-line tap lengths.
  /*!
    The valid range for each tap length is from 0 to the maximum delay-line length.
  */
  void setTapDelays( std::vector<unsigned long> taps );

  //! Return the current delay-line length.
  std::vector<unsigned long> getTapDelays( void ) const { return delays_; };

  //! Return the specified tap value of the last computed frame.
  /*!
    Use the lastFrame() function to get all tap values from the
    last computed frame.  The \c tap argument must be less than the
    number of delayline taps (the first tap is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  T lastOut( unsigned int tap = 0 ) const;

  //! Input one sample to the delayline and return outputs at all tap positions.
  /*!
    The StkFrames<T> argument reference is returned.  The output
    values are ordered according to the tap positions set using the
    setTapDelays() function (no sorting is performed).  The StkFrames<T>
    argument must contain at least as many channels as the number of
    taps.  However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( T input, StkFrames<T>& outputs );

  //! Take a channel of the StkFrames<T> object as inputs to the filter and write outputs back to the same object.
  /*!
    The StkFrames<T> argument reference is returned.  The output
    values are ordered according to the tap positions set using the
    setTapDelays() function (no sorting is performed).  The StkFrames<T>
    argument must contain at least as many channels as the number of
    taps.  However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the filter and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  The output values
    are ordered according to the tap positions set using the
    setTapDelays() function (no sorting is performed).  The \c
    iChannel argument must be less than the number of channels in
    the \c iFrames argument (the first channel is specified by 0).
    The \c oFrames argument must contain at least as many channels as
    the number of taps.  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0 );

 protected:

  unsigned long inPoint_;
  std::vector<unsigned long> outPoint_;
  std::vector<unsigned long> delays_;

};

template<typename T>
inline T TapDelay<T>::lastOut( unsigned int tap ) const
{
#if defined(_STK_DEBUG_)
  if ( tap >= this->lastFrame_.size() ) {
    oStream_ << "TapDelay::lastOut(): tap argument and number of taps are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return this->lastFrame_[tap];
}


template<typename T>
inline StkFrames<T>& TapDelay<T>::tick( T input, StkFrames<T>& outputs )
{
#if defined(_STK_DEBUG_)
  if ( outputs.channels() < outPoint_.size() ) {
    oStream_ << "TapDelay::tick(): number of taps > channels in StkFrames<T> argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  this->inputs_[inPoint_++] = input * this->gain_;

  // Check for end condition
  if ( inPoint_ == this->inputs_.size() )
    inPoint_ = 0;

  // Read out next values
  T *outs = &outputs[0];
  for ( unsigned int i=0; i<outPoint_.size(); i++ ) {
    *outs++ = this->inputs_[outPoint_[i]];
    this->lastFrame_[i] = *outs;
    if ( ++outPoint_[i] == this->inputs_.size() )
      outPoint_[i] = 0;
  }

  return outputs;
}


template<typename T>
inline StkFrames<T>& TapDelay<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "TapDelay::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
  if ( frames.channels() < outPoint_.size() ) {
    oStream_ << "TapDelay::tick(): number of taps > channels in StkFrames<T> argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &frames[channel];
  T *oSamples = &frames[0];
  std::size_t j;
  unsigned int iHop = frames.channels();
  std::size_t oHop = frames.channels() - outPoint_.size();
  for ( unsigned long i=0; i<frames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[inPoint_++] = *iSamples * this->gain_;
    if ( inPoint_ == this->inputs_.size() ) inPoint_ = 0;
    for ( j=0; j<outPoint_.size(); j++ ) {
      *oSamples++ = this->inputs_[outPoint_[j]];
      if ( ++outPoint_[j] == this->inputs_.size() ) outPoint_[j] = 0;
    }
  }

  oSamples -= frames.channels();
  for ( j=0; j<outPoint_.size(); j++ ) this->lastFrame_[j] = *oSamples++;
  return frames;
}


template<typename T>
inline StkFrames<T>& TapDelay<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() ) {
    oStream_ << "TapDelay::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
  if ( oFrames.channels() < outPoint_.size() ) {
    oStream_ << "TapDelay::tick(): number of taps > channels in output StkFrames<T> argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[0];
  std::size_t j;
  unsigned int iHop = iFrames.channels();
  std::size_t oHop = oFrames.channels() - outPoint_.size();
  for ( unsigned long i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[inPoint_++] = *iSamples * this->gain_;
    if ( inPoint_ == this->inputs_.size() ) inPoint_ = 0;
    for ( j=0; j<outPoint_.size(); j++ ) {
      *oSamples++ = this->inputs_[outPoint_[j]];
      if ( ++outPoint_[j] == this->inputs_.size() ) outPoint_[j] = 0;
    }
  }

  oSamples -= oFrames.channels();
  for ( j=0; j<outPoint_.size(); j++ ) this->lastFrame_[j] = *oSamples++;
  return iFrames;
}

/***************************************************/
/*! \class TapDelay
    \brief STK non-interpolating tapped delay line class.

    This class implements a non-interpolating digital delay-line with
    an arbitrary number of output "taps".  If the maximum length and
    tap delays are not specified during instantiation, a fixed maximum
    length of 4095 and a single tap delay of zero is set.
    
    A non-interpolating delay line is typically used in fixed
    delay-length applications, such as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
TapDelay<T>::TapDelay( std::vector<unsigned long> taps, unsigned long maxDelay )
{
  // Writing before reading allows delays from 0 to length-1. 
  // If we want to allow a delay of maxDelay, we need a
  // delayline of length = maxDelay+1.
  if ( maxDelay < 1 ) {
    oStream_ << "TapDelay::TapDelay: maxDelay must be > 0!\n";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  for ( unsigned int i=0; i<taps.size(); i++ ) {
    if ( taps[i] > maxDelay ) {
      oStream_ << "TapDelay::TapDelay: maxDelay must be > than all tap delay values!\n";
      handleError( StkError::FUNCTION_ARGUMENT );
    }
  }

  if ( ( maxDelay + 1 ) > this->inputs_.size() )
    this->inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setTapDelays( taps );
}

template<typename T>
TapDelay<T>::~TapDelay()
{
}

template<typename T>
void TapDelay<T>::setMaximumDelay( unsigned long delay )
{
  if ( delay < this->inputs_.size() ) return;

  for ( unsigned int i=0; i<delays_.size(); i++ ) {
    if ( delay < delays_[i] ) {
      oStream_ << "TapDelay::setMaximumDelay: argument (" << delay << ") less than a current tap delay setting (" << delays_[i] << ")!\n";
      handleError( StkError::WARNING ); return;
    }
  }

  this->inputs_.resize( delay + 1 );
}

template<typename T>
void TapDelay<T>::setTapDelays( std::vector<unsigned long> taps )
{
  for ( unsigned int i=0; i<taps.size(); i++ ) {
    if ( taps[i] > this->inputs_.size() - 1 ) { // The value is too big.
      oStream_ << "TapDelay::setTapDelay: argument (" << taps[i] << ") greater than maximum!\n";
      handleError( StkError::WARNING ); return;
    }
  }

  if ( taps.size() != outPoint_.size() ) {
    outPoint_.resize( taps.size() );
    delays_.resize( taps.size() );
    this->lastFrame_.resize( 1, (unsigned int)taps.size(), 0.0 );
  }

  for ( unsigned int i=0; i<taps.size(); i++ ) {
    // read chases write
    if ( inPoint_ >= taps[i] ) outPoint_[i] = inPoint_ - taps[i];
    else outPoint_[i] = this->inputs_.size() + inPoint_ - taps[i];
    delays_[i] = taps[i];
  }
}

} // stk namespace


