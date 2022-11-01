#pragma once
#include "StkInstrmnt.hpp"
#include "StkJetTable.hpp"
#include "StkBiQuad.hpp"
#include "StkPoleZero.hpp"
#include "StkNoise.hpp"
#include "StkADSR.hpp"
#include "StkSineWave.hpp"
#include "SKINImsg.h"
#include <cmath>

#define __BOTTLE_RADIUS_ 0.999

namespace stk {

/***************************************************/
/*! \class BlowBotl
    \brief STK blown bottle instrument class.

    This class implements a helmholtz resonator
    (biquad filter) with a polynomial jet
    excitation (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class BlowBotl : public Instrmnt<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BlowBotl( void );

  //! Class destructor.
  ~BlowBotl( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

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

  JetTable<T> jetTable_;
  BiQuad<T> resonator_;
  PoleZero<T> dcBlock_;
  Noise<T> noise_;
  ADSR<T> adsr_;
  SineWave<T> vibrato_;
  T maxPressure_;
  T noiseGain_;
  T vibratoGain_;
  T outputGain_;

};

template<typename T>
inline T BlowBotl<T>::tick( unsigned int )
{
  T breathPressure;
  T randPressure;
  T pressureDiff;

  // Calculate the breath pressure (envelope + vibrato)
  breathPressure = maxPressure_ * adsr_.tick();
  breathPressure += vibratoGain_ * vibrato_.tick();

  pressureDiff = breathPressure - resonator_.lastOut();

  randPressure = noiseGain_ * noise_.tick();
  randPressure *= breathPressure;
  randPressure *= (1.0 + pressureDiff);

  resonator_.tick( breathPressure + randPressure - ( jetTable_.tick( pressureDiff ) * pressureDiff ) );
  this->lastFrame_[0] = 0.2 * outputGain_ * dcBlock_.tick( pressureDiff );

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& BlowBotl<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "BlowBotl::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class BlowBotl
    \brief STK blown bottle instrument class.

    This class implements a helmholtz resonator
    (biquad filter) with a polynomial jet
    excitation (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



template<typename T>
BlowBotl<T>::BlowBotl( void )
{
  dcBlock_.setBlockZero();

  vibrato_.setFrequency( 5.925 );
  vibratoGain_ = 0.0;

  resonator_.setResonance( 500.0, __BOTTLE_RADIUS_, true );
  adsr_.setAllTimes( 0.005, 0.01, 0.8, 0.010 );

  noiseGain_ = 20.0;
	maxPressure_ = 0.0;
}

template<typename T>
BlowBotl<T>::~BlowBotl( void )
{
}

template<typename T>
void BlowBotl<T>::clear( void )
{
  resonator_.clear();
}

template<typename T>
void BlowBotl<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "BlowBotl::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  resonator_.setResonance( frequency, __BOTTLE_RADIUS_, true );
}

template<typename T>
void BlowBotl<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "BlowBotl::startBowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setAttackRate( rate );
  maxPressure_ = amplitude;
  adsr_.keyOn();
}

template<typename T>
void BlowBotl<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "BlowBotl::stopBowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setReleaseRate( rate );
  adsr_.keyOff();
}

template<typename T>
void BlowBotl<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02);
  outputGain_ = amplitude + 0.001;
}

template<typename T>
void BlowBotl<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.02 );
}

template<typename T>
void BlowBotl<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( value < 0 || ( number != 101 && value > 128.0 ) ) {
    oStream_ << "BlowBotl::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = normalizedValue * 30.0;
  else if (number == __SK_ModFrequency_) // 11
    vibrato_.setFrequency( normalizedValue * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain_ = normalizedValue * 0.4;
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "BlowBotl::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}


} // stk namespace
