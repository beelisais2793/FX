#pragma once
#include "StkInstrmnt.hpp"
#include "StkADSR.hpp"
#include "StkFileLoop.hpp"
#include "StkOnePole.hpp"
#include "StkBiQuad.hpp"
#include "StkNoise.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Simple : public Instrmnt<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Simple( void );

  //! Class destructor.
  ~Simple( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Start envelope toward "on" target.
  void keyOn( void );

  //! Start envelope toward "off" target.
  void keyOff( void );

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

  ADSR<T>      adsr_; 
  FileLoop<T> *loop_;
  OnePole<T>   filter_;
  BiQuad<T>    biquad_;
  Noise<T>     noise_;
  T  baseFrequency_;
  T  loopGain_;

};

template<typename T>
inline T Simple<T>::tick( unsigned int )
{
  this->lastFrame_[0] = loopGain_ * loop_->tick();
  biquad_.tick( noise_.tick() );
  this->lastFrame_[0] += (1.0 - loopGain_) * biquad_.lastOut();
  this->lastFrame_[0] = filter_.tick( this->lastFrame_[0] );
  this->lastFrame_[0] *= adsr_.tick();
  return this->lastFrame_[0];
}


template<typename T>
inline StkFrames<T>& Simple<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Simple::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Simple<T>::Simple( void )
{
  // Concatenate the STK rawwave path to the rawwave file
  loop_ = new FileLoop<T>( (stk::rawwavePath() + "impuls10.raw").c_str(), true );

  filter_.setPole( 0.5 );
  baseFrequency_ = 440.0;
  setFrequency( baseFrequency_ );
  loopGain_ = 0.5;
}  

template<typename T>
Simple<T>::~Simple( void )
{
  delete loop_;
}

template<typename T>
void Simple<T>::keyOn( void )
{
  adsr_.keyOn();
}

template<typename T>
void Simple<T>::keyOff( void )
{
  adsr_.keyOff();
}


template<typename T>
void Simple<T>::noteOn( T frequency, T amplitude )
{
  this->keyOn();
  this->setFrequency( frequency );
  filter_.setGain( amplitude ); 
}


template<typename T>
void Simple<T>::noteOff( T amplitude )
{
  this->keyOff();
}


template<typename T>
void Simple<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Simple::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  biquad_.setResonance( frequency, 0.98, true );
  loop_->setFrequency( frequency );
}


template<typename T>
void Simple<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Simple::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_Breath_) // 2
    filter_.setPole( 0.99 * (1.0 - (normalizedValue * 2.0)) );
  else if (number == __SK_NoiseLevel_) // 4
    loopGain_ = normalizedValue;
  else if (number == __SK_ModFrequency_) { // 11
    normalizedValue /= 0.2 * stk::sampleRate();
    adsr_.setAttackRate( normalizedValue );
    adsr_.setDecayRate( normalizedValue );
    adsr_.setReleaseRate( normalizedValue );
  }
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Simple::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

