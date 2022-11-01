#pragma once
#include "StkInstrmnt.hpp"
#include "StkDelayL.hpp"
#include "StkBowTable.hpp"
#include "StkOnePole.hpp"
#include "StkBiQuad.hpp"
#include "StkSineWave.hpp"
#include "StkADSR.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Bow Velocity = 100
       - Frequency = 101
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
    Contributions by Esteban Maestre, 2011.
*/
/***************************************************/

template<typename T>
class Bowed : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Bowed( T lowestFrequency = 8.0 );

  //! Class destructor.
  ~Bowed( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set vibrato gain.
  void setVibrato( T gain ) { vibratoGain_ = gain; };

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBowing( T amplitude, T rate );

  //! Decrease breath pressure with given rate of decrease.
  void stopBowing( T rate );

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

  DelayL<T>   neckDelay_;
  DelayL<T>   bridgeDelay_;
  BowTable<T> bowTable_;
  OnePole<T>  stringFilter_;
  BiQuad<T>   bodyFilters_[6];
  SineWave<T> vibrato_;
  ADSR<T>     adsr_;

  bool     bowDown_;
  T maxVelocity_;
  T baseDelay_;
  T vibratoGain_;
  T betaRatio_;

};

template<typename T>
inline T Bowed<T>::tick( unsigned int )
{
  T bowVelocity = maxVelocity_ * adsr_.tick();
  T bridgeReflection = -stringFilter_.tick( bridgeDelay_.lastOut() );
  T nutReflection = -neckDelay_.lastOut();
  T stringVelocity = bridgeReflection + nutReflection;
  T deltaV = bowVelocity - stringVelocity;             // Differential velocity

  T newVelocity = 0.0;
  if ( bowDown_ )
    newVelocity = deltaV * bowTable_.tick( deltaV );     // Non-Linear bow function
  neckDelay_.tick( bridgeReflection + newVelocity);      // Do string propagations
  bridgeDelay_.tick(nutReflection + newVelocity);
    
  if ( vibratoGain_ > 0.0 )  {
    neckDelay_.setDelay( (baseDelay_ * (1.0 - betaRatio_) ) + 
                         (baseDelay_ * vibratoGain_ * vibrato_.tick()) );
  }

  this->lastFrame_[0] = 0.1248 * bodyFilters_[5].tick( bodyFilters_[4].tick( bodyFilters_[3].tick( bodyFilters_[2].tick( bodyFilters_[1].tick( bodyFilters_[0].tick( bridgeDelay_.lastOut() ) ) ) ) ) );

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Bowed<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Bowed::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Bow Velocity = 100
       - Frequency = 101
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
    Contributions by Esteban Maestre, 2011.
*/
/***************************************************/



template<typename T>
Bowed<T>::Bowed( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Bowed::Bowed: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long nDelays = (unsigned long) ( sampleRate() / lowestFrequency );

  neckDelay_.setMaximumDelay( nDelays + 1 );
  neckDelay_.setDelay( 100.0 );

  bridgeDelay_.setMaximumDelay( nDelays + 1 );
  bridgeDelay_.setDelay( 29.0 );

  bowTable_.setSlope( 3.0 );
  bowTable_.setOffset( 0.001);
  bowDown_ = false;
  maxVelocity_ = 0.25;

  vibrato_.setFrequency( 6.12723 );
  vibratoGain_ = 0.0;

  stringFilter_.setPole( 0.75 - (0.2 * 22050.0 / sampleRate()) );
  stringFilter_.setGain( 0.95 );

  // Old single body filter
  //bodyFilter_.setResonance( 500.0, 0.85, true );
  //bodyFilter_.setGain( 0.2 );

  // New body filter provided by Esteban Maestre (cascade of second-order sections)
  bodyFilters_[0].setCoefficients( 1.0,  1.5667, 0.3133, -0.5509, -0.3925 );
  bodyFilters_[1].setCoefficients( 1.0, -1.9537, 0.9542, -1.6357, 0.8697 );
  bodyFilters_[2].setCoefficients( 1.0, -1.6683, 0.8852, -1.7674, 0.8735 );
  bodyFilters_[3].setCoefficients( 1.0, -1.8585, 0.9653, -1.8498, 0.9516 );
  bodyFilters_[4].setCoefficients( 1.0, -1.9299, 0.9621, -1.9354, 0.9590 );
  bodyFilters_[5].setCoefficients( 1.0, -1.9800, 0.9888, -1.9867, 0.9923 );

  adsr_.setAllTimes( 0.02, 0.005, 0.9, 0.01 );
    
  betaRatio_ = 0.127236;

  // Necessary to initialize internal variables.
  this->setFrequency( 220.0 );
  this->clear();
}

template<typename T>
Bowed<T>::~Bowed( void )
{
}

template<typename T>
void Bowed<T>::clear( void )
{
  neckDelay_.clear();
  bridgeDelay_.clear();
  stringFilter_.clear();
  for ( int i=0; i<6; i++ ) bodyFilters_[i].clear();
}

template<typename T>
void Bowed<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Bowed::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // Delay = length - approximate filter delay.
  baseDelay_ = sampleRate() / frequency - 4.0;
  if ( baseDelay_ <= 0.0 ) baseDelay_ = 0.3;
  bridgeDelay_.setDelay( baseDelay_ * betaRatio_ ); 	     // bow to bridge length
  neckDelay_.setDelay( baseDelay_ * (1.0 - betaRatio_) );  // bow to nut (finger) length
}

template<typename T>
void Bowed<T>::startBowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "Bowed::startBowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setAttackRate( rate );
  adsr_.keyOn();
  maxVelocity_ = 0.03 + ( 0.2 * amplitude );
  bowDown_ = true;
}

template<typename T>
void Bowed<T>::stopBowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "Bowed::stopBowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setReleaseRate( rate );
  adsr_.keyOff();
}

template<typename T>
void Bowed<T>::noteOn( T frequency, T amplitude )
{
  this->startBowing( amplitude, amplitude * 0.001 );
  this->setFrequency( frequency );
}

template<typename T>
void Bowed<T>::noteOff( T amplitude )
{
  this->stopBowing( (1.0 - amplitude) * 0.005 );
}

template<typename T>
void Bowed<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( value < 0 || ( number != 101 && value > 128.0 ) ) {
    oStream_ << "Bowed::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if ( number == __SK_BowPressure_ ) { // 2
		if ( normalizedValue > 0.0 ) bowDown_ = true;
		else bowDown_ = false;
		bowTable_.setSlope( 5.0 - (4.0 * normalizedValue) );
  }
  else if ( number == __SK_BowPosition_ ) { // 4
    betaRatio_ = normalizedValue;
    bridgeDelay_.setDelay( baseDelay_ * betaRatio_ );
    neckDelay_.setDelay( baseDelay_ * (1.0 - betaRatio_) );
  }
  else if ( number == __SK_ModFrequency_ ) // 11
    vibrato_.setFrequency( normalizedValue * 12.0 );
  else if ( number == __SK_ModWheel_ ) // 1
    vibratoGain_ = ( normalizedValue * 0.4 );
  else if ( number == 100 ) // 100: set instantaneous bow velocity
    adsr_.setTarget( normalizedValue );
	else if ( number == 101 ) // 101: set instantaneous value of frequency
		this->setFrequency( value );	  
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Bowed::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace
