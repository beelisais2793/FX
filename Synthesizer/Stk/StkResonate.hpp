#pragma once
#include "StkInstrmnt.hpp"
#include "StkADSR.hpp"
#include "StkBiQuad.hpp"
#include "StkNoise.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Resonate : public Instrmnt<T>
{
 public:
  //! Class constructor.
  Resonate( void );

  //! Class destructor.
  ~Resonate( void );

  //! Set the filter for a resonance at the given frequency (Hz) and radius.
  void setResonance( T frequency, T radius );

  //! Set the filter for a notch at the given frequency (Hz) and radius.
  void setNotch( T frequency, T radius );

  //! Set the filter zero coefficients for contant resonance gain.
  void setEqualGainZeroes( void ) { filter_.setEqualGainZeroes(); };

  //! Initiate the envelope with a key-on event.
  void keyOn( void ) { adsr_.keyOn(); };

  //! Signal a key-off event to the envelope.
  void keyOff( void ) { adsr_.keyOff(); };

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

  ADSR<T>     adsr_;
  BiQuad<T>   filter_;
  Noise<T>    noise_;
  T poleFrequency_;
  T poleRadius_;
  T zeroFrequency_;
  T zeroRadius_;

};

template<typename T>
inline T Resonate<T>::tick( unsigned int )
{
  this->lastFrame_[0] = filter_.tick( noise_.tick() );
  this->lastFrame_[0] *= adsr_.tick();
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Resonate<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Resonate::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



template<typename T>
Resonate<T>::Resonate( void )
{
  poleFrequency_ = 4000.0;
  poleRadius_ = 0.95;
  // Set the filter parameters.
  filter_.setResonance( poleFrequency_, poleRadius_, true );
  zeroFrequency_ = 0.0;
  zeroRadius_ = 0.0;
}  

template<typename T>
Resonate<T>::~Resonate( void )
{
}

template<typename T>
void Resonate<T>::noteOn( T frequency, T amplitude )
{
  adsr_.setTarget( amplitude );
  this->keyOn();
  this->setResonance( frequency, poleRadius_ );
}

template<typename T>
void Resonate<T>::noteOff( T amplitude )
{
  this->keyOff();
}

template<typename T>
void Resonate<T>::setResonance( T frequency, T radius )
{
  if ( frequency < 0.0 ) {
    oStream_ << "Resonate::setResonance: frequency parameter is less than zero!";
    handleError( StkError::WARNING ); return;
  }

  if ( radius < 0.0 || radius >= 1.0 ) {
    oStream_ << "Resonate::setResonance: radius parameter is out of range!";
    handleError( StkError::WARNING ); return;
  }

  poleFrequency_ = frequency;
  poleRadius_ = radius;
  filter_.setResonance( poleFrequency_, poleRadius_, true );
}


template<typename T>
void Resonate<T>::setNotch( T frequency, T radius )
{
  if ( frequency < 0.0 ) {
    oStream_ << "Resonate::setNotch: frequency parameter is less than zero ... setting to 0.0!";
    handleError( StkError::WARNING ); return;
  }

  if ( radius < 0.0 ) {
    oStream_ << "Resonate::setNotch: radius parameter is less than 0.0!";
    handleError( StkError::WARNING ); return;
  }

  zeroFrequency_ = frequency;
  zeroRadius_ = radius;
  filter_.setNotch( zeroFrequency_, zeroRadius_ );
}


template<typename T>
void Resonate<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Resonate::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == 2) // 2
    setResonance( normalizedValue * stk::sampleRate() * 0.5, poleRadius_ );
  else if (number == 4) // 4
    setResonance( poleFrequency_, normalizedValue * 0.9999 );
  else if (number == 11) // 11
    this->setNotch( normalizedValue * stk::sampleRate() * 0.5, zeroRadius_ );
  else if (number == 1)
    this->setNotch( zeroFrequency_, normalizedValue );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Resonate::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace
