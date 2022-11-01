#pragma once
#include "Stk.hpp"
#include "StkDelayA.hpp"
#include "StkDelayL.hpp"
#include "StkFir.hpp"

namespace stk {

/***************************************************/
/*! \class Twang
    \brief STK enhanced plucked string class.

    This class implements an enhanced plucked-string
    physical model, a la Jaffe-Smith, Smith,
    Karjalainen and others.  It includes a comb
    filter to simulate pluck position.  The tick()
    function takes an input sample, which is added
    to the delayline input.  This can be used to
    implement commuted synthesis (if the input
    samples are derived from the impulse response of
    a body filter) and/or feedback (as in an electric
    guitar model).

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Twang : public Stk
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Twang( T lowestFrequency = 50.0 );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set the delayline parameters to allow frequencies as low as specified.
  void setLowestFrequency( T frequency );

  //! Set the delayline parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPluckPosition( T position );

  //! Set the nominal loop gain.
  /*!
    The actual loop gain is based on the value set with this
    function, but scaled slightly according to the frequency.  Higher
    frequency settings have greater loop gains because of
    high-frequency loop-filter roll-off.
  */
  void setLoopGain( T loopGain );

  //! Set the loop filter coefficients.
  /*!
    The loop filter can be any arbitrary FIR filter.  By default,
    the coefficients are set for a first-order lowpass filter with
    coefficients b = [0.5 0.5].
  */
  void setLoopFilter( std::vector<T> coefficients );

  //! Return an StkFrames<T> reference to the last output sample frame.
  //const StkFrames<T>& lastFrame( void ) const { return this->lastFrame_; };

  //! Return the last computed output value.
  // T lastOut( void ) { return this->lastFrame_[0]; };
  T lastOut( void ) { return lastOutput_; };

  //! Compute and return one output sample.
  T tick( T input );

  //! Take a channel of the \c iFrames object as inputs to the class and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the effect and write outputs to the \c oFrames object.
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

  DelayA<T>   delayLine_;
  DelayL<T>   combDelay_;
  Fir<T>      loopFilter_;

  T lastOutput_;
  T frequency_;
  T loopGain_;
  T pluckPosition_;
};


template<typename T>
inline T Twang<T>::tick( T input )
{
  lastOutput_ = delayLine_.tick( input + loopFilter_.tick( delayLine_.lastOut() ) );
  lastOutput_ -= combDelay_.tick( lastOutput_ ); // comb filtering on output
  lastOutput_ *= 0.5;

  return lastOutput_;
}


template<typename T>
inline StkFrames<T>& Twang<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Twang::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = tick( *samples );

  return frames;
}


template<typename T>
inline StkFrames<T>& Twang<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "Twang::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop )
    *oSamples = tick( *iSamples );

  return iFrames;
}

/***************************************************/
/*! \class Twang
    \brief STK enhanced plucked string class.

    This class implements an enhanced plucked-string
    physical model, a la Jaffe-Smith, Smith,
    Karjalainen and others.  It includes a comb
    filter to simulate pluck position.  The tick()
    function takes an input sample, which is added
    to the delayline input.  This can be used to
    implement commuted synthesis (if the input
    samples are derived from the impulse response of
    a body filter) or feedback (as in an electric
    guitar model).

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Twang<T>::Twang( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Twang::Twang: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  this->setLowestFrequency( lowestFrequency );

  std::vector<T> coefficients( 2, 0.5 );
  loopFilter_.setCoefficients( coefficients );

  loopGain_ = 0.995;
  pluckPosition_ = 0.4;
  this->setFrequency( 220.0 );
}


template<typename T>
void Twang<T>::clear( void )
{
  delayLine_.clear();
  combDelay_.clear();
  loopFilter_.clear();
  lastOutput_ = 0.0;
}


template<typename T>
void Twang<T>::setLowestFrequency( T frequency )
{
  unsigned long nDelays = (unsigned long) ( stk::sampleRate() / frequency );
  delayLine_.setMaximumDelay( nDelays + 1 );
  combDelay_.setMaximumDelay( nDelays + 1 );
}

template<typename T>
void Twang<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Twang::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  frequency_ = frequency;
  // Delay = length - filter delay.
  T delay = ( stk::sampleRate() / frequency ) - loopFilter_.phaseDelay( frequency );
  delayLine_.setDelay( delay );

  this->setLoopGain( loopGain_ );

  // Set the pluck position, which puts zeroes at position * length.
  combDelay_.setDelay( 0.5 * pluckPosition_ * delay );
}


template<typename T>
void Twang<T>::setLoopGain( T loopGain )
{
  if ( loopGain < 0.0 || loopGain >= 1.0 ) {
    oStream_ << "Twang::setLoopGain: parameter is out of range!";
    handleError( StkError::WARNING ); return;
  }

  loopGain_ = loopGain;
  T gain = loopGain_ + (frequency_ * 0.000005);
  if ( gain >= 1.0 ) gain = 0.99999;
  loopFilter_.setGain( gain );
}


template<typename T>
void Twang<T>::setPluckPosition( T position )
{
  if ( position < 0.0 || position > 1.0 ) {
    oStream_ << "Twang::setPluckPosition: argument (" << position << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }

  pluckPosition_ = position;
}

} // stk namespace
