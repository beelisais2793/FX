#pragma once
#include "StkInstrmnt.hpp"
#include "StkDelayA.hpp"
#include "StkBiQuad.hpp"
#include "StkPoleZero.hpp"
#include "StkADSR.hpp"
#include "StkSineWave.hpp"
#include "SKINImsg.h"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Brass: public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Brass( T lowestFrequency = 8.0 );

  //! Class destructor.
  ~Brass(  );

  //! Reset and clear all internal state.
  void clear(  );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set the lips frequency.
  void setLip( T frequency );

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

  DelayA<T>   delayLine_;
  BiQuad<T>   lipFilter_;
  PoleZero<T> dcBlock_;
  ADSR<T>     adsr_;
  SineWave<T> vibrato_;

  T lipTarget_;
  T slideTarget_;
  T vibratoGain_;
  T maxPressure_;

};

template<typename T>
inline T Brass<T>::tick( unsigned int )
{
  T breathPressure = maxPressure_ * adsr_.tick();
  breathPressure += vibratoGain_ * vibrato_.tick();

  T mouthPressure = 0.3 * breathPressure;
  T borePressure = 0.85 * delayLine_.lastOut();
  T deltaPressure = mouthPressure - borePressure; // Differential pressure.
  deltaPressure = lipFilter_.tick( deltaPressure );      // Force - > position.
  deltaPressure *= deltaPressure;                        // Basic position to area mapping.
  if ( deltaPressure > 1.0 ) deltaPressure = 1.0;        // Non-linear saturation.

  // The following input scattering assumes the mouthPressure = area.
  this->lastFrame_[0] = deltaPressure * mouthPressure + ( 1.0 - deltaPressure) * borePressure;
  this->lastFrame_[0] = delayLine_.tick( dcBlock_.tick( this->lastFrame_[0] ) );

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Brass<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Brass::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
Brass<T>::Brass( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Brass::Brass: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long nDelays = (unsigned long) ( sampleRate() / lowestFrequency );
  delayLine_.setMaximumDelay( nDelays + 1 );

  lipFilter_.setGain( 0.03 );
  dcBlock_.setBlockZero();
  adsr_.setAllTimes( 0.005, 0.001, 1.0, 0.010 );

  vibrato_.setFrequency( 6.137 );
  vibratoGain_ = 0.0;
	maxPressure_ = 0.0;
  lipTarget_ = 0.0;

  this->clear();

  // This is necessary to initialize variables.
  this->setFrequency( 220.0 );
}

template<typename T>
Brass<T>::~Brass( void )
{
}

template<typename T>
void Brass<T>::clear( void )
{
  delayLine_.clear();
  lipFilter_.clear();
  dcBlock_.clear();
}

template<typename T>
void Brass<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Brass::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // Fudge correction for filter delays.
  slideTarget_ = ( sampleRate() / frequency * 2.0 ) + 3.0;
  delayLine_.setDelay( slideTarget_ ); // play a harmonic

  lipTarget_ = frequency;
  lipFilter_.setResonance( frequency, 0.997 );
}

template<typename T>
void Brass<T>::setLip( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Brass::setLip: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  lipFilter_.setResonance( frequency, 0.997 );
}

template<typename T>
void Brass<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "Brass::startBlowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setAttackRate( rate );
  maxPressure_ = amplitude;
  adsr_.keyOn();
}

template<typename T>
void Brass<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "Brass::stopBlowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setReleaseRate( rate );
  adsr_.keyOff();
}

template<typename T>
void Brass<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( amplitude, amplitude * 0.001 );
}

template<typename T>
void Brass<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.005 );
}

template<typename T>
void Brass<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Brass::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_LipTension_)	{ // 2
    T temp = lipTarget_ * pow( 4.0, (2.0 * normalizedValue) - 1.0 );
    this->setLip( temp );
  }
  else if (number == __SK_SlideLength_) // 4
    delayLine_.setDelay( slideTarget_ * (0.5 + normalizedValue) );
  else if (number == __SK_ModFrequency_) // 11
    vibrato_.setFrequency( normalizedValue * 12.0 );
  else if (number == __SK_ModWheel_ ) // 1
    vibratoGain_ = normalizedValue * 0.4;
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Brass::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

