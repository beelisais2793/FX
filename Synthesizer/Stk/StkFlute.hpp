#pragma once
#include "StkInstrmnt.hpp"
#include "StkJetTable.hpp"
#include "StkDelayL.hpp"
#include "StkOnePole.hpp"
#include "StkPoleZero.hpp"
#include "StkNoise.hpp"
#include "StkADSR.hpp"
#include "StkSineWave.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Flute
    \brief STK flute physical model class.

    This class implements a simple flute
    physical model, as discussed by Karjalainen,
    Smith, Waryznyk, etc.  The jet model uses
    a polynomial, a la Cook.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Flute : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Flute( T lowestFrequency );

  //! Class destructor.
  ~Flute( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set the reflection coefficient for the jet delay (-1.0 - 1.0).
  void setJetReflection( T coefficient ) { jetReflection_ = coefficient; };

  //! Set the reflection coefficient for the air column delay (-1.0 - 1.0).
  void setEndReflection( T coefficient ) { endReflection_ = coefficient; };

  //! Set the length of the jet delay in terms of a ratio of jet delay to air column delay lengths.
  void setJetDelay( T aRatio );

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing( T amplitude, T rate );

  //! Decrease breath velocity with given rate of decrease.
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

  DelayL<T>   jetDelay_;
  DelayL<T>   boreDelay_;
  JetTable<T> jetTable_;
  OnePole<T>  filter_;
  PoleZero<T> dcBlock_;
  Noise<T>    noise_;
  ADSR<T>     adsr_;
  SineWave<T> vibrato_;

  T lastFrequency_;
  T maxPressure_;
  T jetReflection_;
  T endReflection_;
  T noiseGain_;
  T vibratoGain_;
  T outputGain_;
  T jetRatio_;

};

template<typename T>
inline T Flute<T>::tick( unsigned int )
{
  T pressureDiff;
  T breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = maxPressure_ * adsr_.tick();
  breathPressure += breathPressure * ( noiseGain_ * noise_.tick() + vibratoGain_ * vibrato_.tick() );

  T temp = -filter_.tick( boreDelay_.lastOut() );
  //temp = dcBlock_.tick( temp ); // Block DC on reflection.

  pressureDiff = breathPressure - (jetReflection_ * temp);
  pressureDiff = jetDelay_.tick( pressureDiff );
  //pressureDiff = jetTable_.tick( pressureDiff ) + (endReflection_ * temp);
  pressureDiff = dcBlock_.tick(jetTable_.tick( pressureDiff )) + (endReflection_ * temp); // moved the DC blocker to after the jet non-linearity (GPS, 29 Jan. 2020)
  this->lastFrame_[0] = (T) 0.3 * boreDelay_.tick( pressureDiff );

  this->lastFrame_[0] *= outputGain_;
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Flute<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Flute::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \Class Flute
    \brief STK flute physical model class.

    This class implements a simple flute
    physical model, as discussed by Karjalainen,
    Smith, Waryznyk, etc.  The jet model uses
    a polynomial, a la Cook.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
Flute<T>::Flute( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Flute::Flute: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long nDelays = (unsigned long) ( sampleRate() / lowestFrequency );
  boreDelay_.setMaximumDelay( nDelays + 1 );

  jetDelay_.setMaximumDelay( nDelays + 1 );
  jetDelay_.setDelay( 49.0 );

  vibrato_.setFrequency( 5.925 );
  filter_.setPole( 0.7 - ( 0.1 * 22050.0 / sampleRate() ) );
  dcBlock_.setBlockZero();

  adsr_.setAllTimes( 0.005, 0.01, 0.8, 0.010 );
  endReflection_ = 0.5;
  jetReflection_ = 0.5;
  noiseGain_     = 0.15;    // Breath pressure random component
  vibratoGain_   = 0.05;    // Breath periodic vibrato component
  jetRatio_      = 0.32;

	maxPressure_ = 0.0;
  this->clear();
  this->setFrequency( 220.0 );
}

template<typename T>
Flute<T>::~Flute( void )
{
}

template<typename T>
void Flute<T>::clear( void )
{
  jetDelay_.clear();
  boreDelay_.clear();
  filter_.clear();
  dcBlock_.clear();
}

template<typename T>
void Flute<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Flute::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // We're overblowing here.
  lastFrequency_ = frequency * 0.66666;

  // Account for filter delay and one sample "lastOut" delay
  // (previously approximated as 2.0 samples).  The tuning is still
  // not perfect but I'm not sure why.  Also, we are not accounting
  // for the dc blocking filter delay.
  T delay = sampleRate() / lastFrequency_ - filter_.phaseDelay( lastFrequency_ ) - 1.0;

  boreDelay_.setDelay( delay );
  jetDelay_.setDelay( delay * jetRatio_ );
}

template<typename T>
void Flute<T>::setJetDelay( T aRatio )
{
  jetRatio_ = aRatio;
  jetDelay_.setDelay( boreDelay_.getDelay() * aRatio ); // Scaled by ratio.
}

template<typename T>
void Flute<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "Flute::startBlowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setAttackRate( rate );
  maxPressure_ = amplitude / (T) 0.8;
  adsr_.keyOn();
}

template<typename T>
void Flute<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "Flute::stopBlowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setReleaseRate( rate );
  adsr_.keyOff();
}

template<typename T>
void Flute<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02 );
  outputGain_ = amplitude + 0.001;
}

template<typename T>
void Flute<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.02 );
}


template<typename T>
void Flute<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Flute::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_JetDelay_) // 2
    this->setJetDelay( (T) (0.08 + (0.48 * normalizedValue)) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = ( normalizedValue * 0.4);
  else if (number == __SK_ModFrequency_) // 11
    vibrato_.setFrequency( normalizedValue * 12.0);
  else if (number == __SK_ModWheel_) // 1
    vibratoGain_ = ( normalizedValue * 0.4 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Flute::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace
