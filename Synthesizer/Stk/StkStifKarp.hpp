#pragma once
#include "StkInstrmnt.hpp"
#include "StkDelayL.hpp"
#include "StkDelayA.hpp"
#include "StkOneZero.hpp"
#include "StkNoise.hpp"
#include "StkBiQuad.hpp"
#include "SKINImsg.h"
#include <cmath>


namespace stk {

/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class StifKarp : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  StifKarp( T lowestFrequency = 10.0 );

  //! Class destructor.
  ~StifKarp( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set the stretch "factor" of the string (0.0 - 1.0).
  void setStretch( T stretch );

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPickupPosition( T position );

  //! Set the base loop gain.
  /*!
    The actual loop gain is set according to the frequency.
    Because of high-frequency loop filter roll-off, higher
    frequency settings have greater loop gains.
  */
  void setBaseLoopGain( T aGain );

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck( T amplitude );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, T value );

  //! Compute and return one output sample.
  T tick( unsigned int channel = 0 );

  //! Fill a channel of the StkFrames<T> object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

 protected:

  DelayA<T>  delayLine_;
  DelayL<T>  combDelay_;
  OneZero<T> filter_;
  Noise<T>   noise_;
  BiQuad<T>  biquad_[4];

  T loopGain_;
  T baseLoopGain_;
  T lastFrequency_;
  T lastLength_;
  T stretching_;
  T pluckAmplitude_;
  T pickupPosition_;

};

template<typename T>
inline T StifKarp<T>:: tick( unsigned int )
{
  T temp = delayLine_.lastOut() * loopGain_;

  // Calculate allpass stretching.
  for (int i=0; i<4; i++)
    temp = biquad_[i].tick(temp);

  // Moving average filter.
  temp = filter_.tick(temp);

  this->lastFrame_[0] = delayLine_.tick(temp);
  this->lastFrame_[0] = this->lastFrame_[0] - combDelay_.tick( this->lastFrame_[0] );
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& StifKarp<T>:: tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "StifKarp::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int j, hop = frames.channels() - nChannels;
  if ( nChannels == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
      *samples++ = tick();
  }
  else {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
      *samples++ = tick();
      for ( j=1; j<nChannels; j++ )
        *samples++ = this->lastFrame_[j];
    }
  }

  return frames;
}

/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
StifKarp<T>:: StifKarp( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "StifKarp::StifKarp: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long nDelays = (unsigned long) ( stk::sampleRate() / lowestFrequency );
  delayLine_.setMaximumDelay( nDelays + 1 );
  combDelay_.setMaximumDelay( nDelays + 1 );

  pluckAmplitude_ = 0.3;
  pickupPosition_ = 0.4;

  stretching_ = 0.9999;
  baseLoopGain_ = 0.995;
  loopGain_ = 0.999;

  this->clear();
  this->setFrequency( 220.0 );
}

template<typename T>
StifKarp<T>:: ~StifKarp( void )
{
}

template<typename T>
void StifKarp<T>:: clear( void )
{
  delayLine_.clear();
  combDelay_.clear();
  filter_.clear();
}


template<typename T>
void StifKarp<T>:: setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "StifKarp::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  lastFrequency_ = frequency; 
  lastLength_ = stk::sampleRate() / lastFrequency_;
  T delay = lastLength_ - 0.5;
  delayLine_.setDelay( delay );

  loopGain_ = baseLoopGain_ + (frequency * 0.000005);
  if (loopGain_ >= 1.0) loopGain_ = 0.99999;

  setStretch(stretching_);

  combDelay_.setDelay( 0.5 * pickupPosition_ * lastLength_ ); 
}


template<typename T>
void StifKarp<T>:: setStretch( T stretch )
{
  stretching_ = stretch;
  T coefficient;
  T freq = lastFrequency_ * 2.0;
  T dFreq = ( (0.5 * stk::sampleRate()) - freq ) * 0.25;
  T temp = 0.5 + (stretch * 0.5);
  if ( temp > 0.99999 ) temp = 0.99999;
  for ( int i=0; i<4; i++ )	{
    coefficient = temp * temp;
    biquad_[i].setA2( coefficient );
    biquad_[i].setB0( coefficient );
    biquad_[i].setB2( 1.0 );

    coefficient = -2.0 * temp * cos(TWO_PI * freq / stk::sampleRate());
    biquad_[i].setA1( coefficient );
    biquad_[i].setB1( coefficient );

    freq += dFreq;
  }
}

template<typename T>
void StifKarp<T>:: setPickupPosition( T position ) {

  if ( position < 0.0 || position > 1.0 ) {
    oStream_ << "StifKarp::setPickupPosition: parameter is out of range!";
    handleError( StkError::WARNING ); return;
  }

  // Set the pick position, which puts zeroes at position * length.
  pickupPosition_ = position;
  combDelay_.setDelay( 0.5 * pickupPosition_ * lastLength_ );
}


template<typename T>
void StifKarp<T>:: setBaseLoopGain( T aGain )
{
  baseLoopGain_ = aGain;
  loopGain_ = baseLoopGain_ + (lastFrequency_ * 0.000005);
  if ( loopGain_ > 0.99999 ) loopGain_ = (T) 0.99999;
}


template<typename T>
void StifKarp<T>:: pluck( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "StifKarp::pluck: amplitude is out of range!";
    handleError( StkError::WARNING ); return;
  }

  pluckAmplitude_ = amplitude;
  for ( unsigned long i=0; i<lastLength_; i++ ) {
    // Fill delay with noise additively with current contents.
    delayLine_.tick( (delayLine_.lastOut() * 0.6) + 0.4 * noise_.tick() * pluckAmplitude_ );
    //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
  }
}

template<typename T>
void StifKarp<T>:: noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->pluck( amplitude );
}


template<typename T>
void StifKarp<T>:: noteOff( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "StifKarp::noteOff: amplitude is out of range!";
    handleError( StkError::WARNING ); return;
  }

  loopGain_ =  (1.0 - amplitude) * 0.5;
}


template<typename T>
void StifKarp<T>:: controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Clarinet::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_PickPosition_) // 4
    this->setPickupPosition( normalizedValue );
  else if (number == __SK_StringDamping_) // 11
    this->setBaseLoopGain( 0.97 + (normalizedValue * 0.03) );
  else if (number == __SK_StringDetune_) // 1
    this->setStretch( 0.9 + (0.1 * (1.0 - normalizedValue)) );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "StifKarp::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace
