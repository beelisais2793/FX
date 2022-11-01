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
/*! \class Saxofony
    \brief STK faux conical bore reed instrument class.

    This class implements a "hybrid" digital
    waveguide instrument that can generate a
    variety of wind-like sounds.  It has also been
    referred to as the "blowed string" model.  The
    waveguide section is essentially that of a
    string, with one rigid and one lossy
    termination.  The non-linear function is a
    reed table.  The string can be "blown" at any
    point between the terminations, though just as
    with strings, it is impossible to excite the
    system at either end.  If the excitation is
    placed at the string mid-point, the sound is
    that of a clarinet.  At points closer to the
    "bridge", the sound is closer to that of a
    saxophone.  See Scavone (2002) for more details.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Saxofony : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Saxofony( T lowestFrequency );

  //! Class destructor.
  ~Saxofony( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set the "blowing" position between the air column terminations (0.0 - 1.0).
  void setBlowPosition( T aPosition );

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

  DelayL<T>    delays_[2];
  ReedTable<T> reedTable_;
  OneZero<T>   filter_;
  Envelope<T>  envelope_;
  Noise<T>     noise_;
  SineWave<T> vibrato_;

  T outputGain_;
  T noiseGain_;
  T vibratoGain_;
  T position_;

};

template<typename T>
inline T Saxofony<T>::tick( unsigned int )
{
  T pressureDiff;
  T breathPressure;
  T temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope_.tick(); 
  breathPressure += breathPressure * noiseGain_ * noise_.tick();
  breathPressure += breathPressure * vibratoGain_ * vibrato_.tick();

  temp = -0.95 * filter_.tick( delays_[0].lastOut() );
  this->lastFrame_[0] = temp - delays_[1].lastOut();
  pressureDiff = breathPressure - this->lastFrame_[0];
  delays_[1].tick( temp );
  delays_[0].tick( breathPressure - (pressureDiff * reedTable_.tick(pressureDiff)) - temp );

  this->lastFrame_[0] *= outputGain_;
  return this->lastFrame_[0];
}


template<typename T>
inline StkFrames<T>& Saxofony<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Saxofony::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Saxofony
    \brief STK faux conical bore reed instrument class.

    This class implements a "hybrid" digital
    waveguide instrument that can generate a
    variety of wind-like sounds.  It has also been
    referred to as the "blowed string" model.  The
    waveguide section is essentially that of a
    string, with one rigid and one lossy
    termination.  The non-linear function is a
    reed table.  The string can be "blown" at any
    point between the terminations, though just as
    with strings, it is impossible to excite the
    system at either end.  If the excitation is
    placed at the string mid-point, the sound is
    that of a clarinet.  At points closer to the
    "bridge", the sound is closer to that of a
    saxophone.  See Scavone (2002) for more details.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Saxofony<T>::Saxofony( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Saxofony::Saxofony: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long nDelays = (unsigned long) ( stk::sampleRate() / lowestFrequency );
  delays_[0].setMaximumDelay( nDelays + 1 );
  delays_[1].setMaximumDelay( nDelays + 1 );

  // Initialize blowing position to 0.2 of length.
  position_ = 0.2;

  reedTable_.setOffset( 0.7 );
  reedTable_.setSlope( 0.3 );

  vibrato_.setFrequency( 5.735 );

  outputGain_ = 0.3;
  noiseGain_ = 0.2;
  vibratoGain_ = 0.1;

  this->setFrequency( 220.0 );
  this->clear();
}

template<typename T>
Saxofony<T>::~Saxofony( void )
{
}


template<typename T>
void Saxofony<T>::clear( void )
{
  delays_[0].clear();
  delays_[1].clear();
  filter_.clear();
}


template<typename T>
void Saxofony<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Saxofony::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // Account for filter delay and one sample "lastOut" delay.
  T delay = ( stk::sampleRate() / frequency ) - filter_.phaseDelay( frequency ) - 1.0;
  delays_[0].setDelay( (1.0-position_) * delay );
  delays_[1].setDelay( position_ * delay );
}


template<typename T>
void Saxofony<T>::setBlowPosition( T position )
{
  if ( position_ == position ) return;

  if ( position < 0.0 ) position_ = 0.0;
  else if ( position > 1.0 ) position_ = 1.0;
  else position_ = position;

  T totalDelay = delays_[0].getDelay();
  totalDelay += delays_[1].getDelay();

  delays_[0].setDelay( (1.0-position_) * totalDelay );
  delays_[1].setDelay( position_ * totalDelay );
}


template<typename T>
void Saxofony<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "Saxofony::startBlowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( rate );
  envelope_.setTarget( amplitude );
}


template<typename T>
void Saxofony<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "Saxofony::stopBlowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( rate );
  envelope_.setTarget( 0.0 );
}


template<typename T>
void Saxofony<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( 0.55 + (amplitude * 0.30), amplitude * 0.005 );
  outputGain_ = amplitude + 0.001;
}


template<typename T>
void Saxofony<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.01 );
}


template<typename T>
void Saxofony<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Saxofony::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_ReedStiffness_) // 2
    reedTable_.setSlope( 0.1 + (0.4 * normalizedValue) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = ( normalizedValue * 0.4 );
  else if (number == 29) // 29
    vibrato_.setFrequency( normalizedValue * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain_ = ( normalizedValue * 0.5 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope_.setValue( normalizedValue );
  else if (number == 11) // 11
    this->setBlowPosition( normalizedValue );
  else if (number == 26) // reed table offset
    reedTable_.setOffset(0.4 + ( normalizedValue * 0.6));
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Saxofony::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace
