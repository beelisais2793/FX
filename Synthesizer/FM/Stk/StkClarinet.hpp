#pragma once
#include "StkInstrmnt.hpp"
#include "StkDelayL.hpp"
#include "StkReedTable.hpp"
#include "StkOneZero.hpp"
#include "StkEnvelope.hpp"
#include "StkNoise.hpp"
#include "StkSineWave.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Clarinet : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Clarinet( T lowestFrequency = 8.0 );

  //! Class destructor.
  ~Clarinet( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing( T amplitude, T rate );

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing( T rate );

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

  DelayL<T> delayLine_;
  ReedTable<T> reedTable_;
  OneZero<T> filter_;
  Envelope<T> envelope_;
  Noise<T> noise_;
  SineWave<T> vibrato_;

  T outputGain_;
  T noiseGain_;
  T vibratoGain_;
};

template<typename T>
inline T Clarinet<T>::tick( unsigned int )
{
  T pressureDiff;
  T breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope_.tick(); 
  breathPressure += breathPressure * noiseGain_ * noise_.tick();
  breathPressure += breathPressure * vibratoGain_ * vibrato_.tick();

  // Perform commuted loss filtering.
  pressureDiff = -0.95 * filter_.tick( delayLine_.lastOut() );

  // Calculate pressure difference of reflected and mouthpiece pressures.
  pressureDiff = pressureDiff - breathPressure;

  // Perform non-linear scattering using pressure difference in reed function.
  this->lastFrame_[0] = delayLine_.tick(breathPressure + pressureDiff * reedTable_.tick(pressureDiff));

  // Apply output gain.
  this->lastFrame_[0] *= outputGain_;

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Clarinet<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Clarinet::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



template<typename T>
Clarinet<T>::Clarinet( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Clarinet::Clarinet: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long nDelays = (unsigned long) ( 0.5 * sampleRate() / lowestFrequency );
  delayLine_.setMaximumDelay( nDelays + 1 );

  reedTable_.setOffset( 0.7 );
  reedTable_.setSlope( -0.3 );

  vibrato_.setFrequency( 5.735 );
  outputGain_ = 1.0;
  noiseGain_ = 0.2;
  vibratoGain_ = 0.1;

  this->setFrequency( 220.0 );
  this->clear();
}

template<typename T>
Clarinet<T>::~Clarinet( void )
{
}

template<typename T>
void Clarinet<T>::clear( void )
{
  delayLine_.clear();
  filter_.tick( 0.0 );
}

template<typename T>
void Clarinet<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Clarinet::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // Account for filter delay and one sample "lastOut" delay.
  T delay = ( sampleRate() / frequency ) * 0.5 - filter_.phaseDelay( frequency ) - 1.0;
  delayLine_.setDelay( delay );
}

template<typename T>
void Clarinet<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "Clarinet::startBlowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( rate );
  envelope_.setTarget( amplitude );
}

template<typename T>
void Clarinet<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "Clarinet::stopBlowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( rate );
  envelope_.setTarget( 0.0 );
}

template<typename T>
void Clarinet<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( 0.55 + (amplitude * 0.30), amplitude * 0.005 );
  outputGain_ = amplitude + 0.001;
}

template<typename T>
void Clarinet<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.01 );
}

template<typename T>
void Clarinet<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Clarinet::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if ( number == __SK_ReedStiffness_ ) // 2
    reedTable_.setSlope( -0.44 + ( 0.26 * normalizedValue ) );
  else if ( number == __SK_NoiseLevel_ ) // 4
    noiseGain_ = ( normalizedValue * 0.4 );
  else if ( number == __SK_ModFrequency_ ) // 11
    vibrato_.setFrequency( normalizedValue * 12.0 );
  else if ( number == __SK_ModWheel_ ) // 1
    vibratoGain_ = ( normalizedValue * 0.5 );
  else if ( number == __SK_AfterTouch_Cont_ ) // 128
    envelope_.setValue( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Clarinet::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

