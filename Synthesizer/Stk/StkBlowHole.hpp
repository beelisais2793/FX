#pragma once

#include "StkInstrmnt.hpp"
#include "StkDelayL.hpp"
#include "StkReedTable.hpp"
#include "StkOneZero.hpp"
#include "StkPoleZero.hpp"
#include "StkEnvelope.hpp"
#include "StkNoise.hpp"
#include "StkSineWave.hpp"

namespace stk {

/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class BlowHole : public Instrmnt<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BlowHole( T lowestFrequency );

  //! Class destructor.
  ~BlowHole( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set the tonehole state (0.0 = closed, 1.0 = fully open).
  void setTonehole( T newValue );

  //! Set the register hole state (0.0 = closed, 1.0 = fully open).
  void setVent( T newValue );

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

  DelayL<T>    delays_[3];
  ReedTable<T> reedTable_;
  OneZero<T>   filter_;
  PoleZero<T>  tonehole_;
  PoleZero<T>  vent_;
  Envelope<T>  envelope_;
  Noise<T>     noise_;
  SineWave<T>  vibrato_;

  T scatter_;
  T thCoeff_;
  T rhGain_;
  T outputGain_;
  T noiseGain_;
  T vibratoGain_;
};

template<typename T>
inline T BlowHole<T>::tick( unsigned int )
{
  T pressureDiff;
  T breathPressure;
  T temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope_.tick(); 
  breathPressure += breathPressure * noiseGain_ * noise_.tick();
  breathPressure += breathPressure * vibratoGain_ * vibrato_.tick();

  // Calculate the differential pressure = reflected - mouthpiece pressures
  pressureDiff = delays_[0].lastOut() - breathPressure;

  // Do two-port junction scattering for register vent
  T pa = breathPressure + pressureDiff * reedTable_.tick( pressureDiff );
  T pb = delays_[1].lastOut();
  vent_.tick( pa+pb );

  this->lastFrame_[0] = delays_[0].tick( vent_.lastOut()+pb );
  this->lastFrame_[0] *= outputGain_;

  // Do three-port junction scattering (under tonehole)
  pa += vent_.lastOut();
  pb = delays_[2].lastOut();
  T pth = tonehole_.lastOut();
  temp = scatter_ * (pa + pb - 2 * pth);

  delays_[2].tick( filter_.tick(pa + temp) * -0.95 );
  delays_[1].tick( pb + temp );
  tonehole_.tick( pa + pb - pth + temp );

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& BlowHole<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "BlowHole::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
BlowHole<T>::BlowHole( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "BlowHole::BlowHole: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long nDelays = (unsigned long) ( 0.5 * sampleRate() / lowestFrequency );

  // delays[0] is the delay line between the reed and the register vent.
  delays_[0].setDelay( 5.0 * sampleRate() / 22050.0 );
  // delays[1] is the delay line between the register vent and the tonehole.
  delays_[1].setMaximumDelay( nDelays + 1 );
  // delays[2] is the delay line between the tonehole and the end of the bore.
  delays_[2].setDelay( 4.0 * sampleRate() / 22050.0 );

  reedTable_.setOffset( 0.7 );
  reedTable_.setSlope( -0.3 );

  // Calculate the initial tonehole three-port scattering coefficient
  T rb = 0.0075;    // main bore radius
  T rth = 0.003;    // tonehole radius
  scatter_ = -pow(rth,2) / ( pow(rth,2) + 2*pow(rb,2) );

  // Calculate tonehole coefficients and set for initially open.
  T te = 1.4 * rth;    // effective length of the open hole
  thCoeff_ = (te*2*sampleRate() - 347.23) / (te*2*sampleRate() + 347.23);
  tonehole_.setA1( -thCoeff_ );
  tonehole_.setB0( thCoeff_ );
  tonehole_.setB1( -1.0 );

  // Calculate register hole filter coefficients
  double r_rh = 0.0015;    // register vent radius
  te = 1.4 * r_rh;         // effective length of the open hole
  double xi = 0.0;         // series resistance term
  double zeta = 347.23 + 2*PI*pow(rb,2)*xi/1.1769;
  double psi = 2*PI*pow(rb,2)*te / (PI*pow(r_rh,2));
  T rhCoeff = (zeta - 2 * sampleRate() * psi) / (zeta + 2 * sampleRate() * psi);
  rhGain_ = -347.23 / (zeta + 2 * sampleRate() * psi);
  vent_.setA1( rhCoeff );
  vent_.setB0( 1.0 );
  vent_.setB1( 1.0 );
  // Start with register vent closed
  vent_.setGain( 0.0 );

  vibrato_.setFrequency((T) 5.735);
  outputGain_ = 1.0;
  noiseGain_ = 0.2;
  vibratoGain_ = 0.01;

  this->setFrequency( 220.0 );
  this->clear();
}

template<typename T>
BlowHole<T>::~BlowHole( void )
{
}

template<typename T>
void BlowHole<T>::clear( void )
{
  delays_[0].clear();
  delays_[1].clear();
  delays_[2].clear();
  filter_.tick( 0.0 );
  tonehole_.tick( 0.0 );
  vent_.tick( 0.0 );
}

template<typename T>
void BlowHole<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "BlowHole::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // Account for approximate filter delays and one sample "lastOut" delay.
  T delay = ( sampleRate() / frequency ) * 0.5 - 3.5;
  delay -= delays_[0].getDelay() + delays_[2].getDelay();

  delays_[1].setDelay( delay );
}

template<typename T>
void BlowHole<T>::setVent( T newValue )
{
  // This method allows setting of the register vent "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).

  T gain;

  if ( newValue <= 0.0 )
    gain = 0.0;
  else if ( newValue >= 1.0 )
    gain = rhGain_;
  else
    gain = newValue * rhGain_;

  vent_.setGain( gain );
}

template<typename T>
void BlowHole<T>::setTonehole( T newValue )
{
  // This method allows setting of the tonehole "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).
  T new_coeff;

  if ( newValue <= 0.0 )
    new_coeff = 0.9995;
  else if ( newValue >= 1.0 )
    new_coeff = thCoeff_;
  else
    new_coeff = ( newValue * (thCoeff_ - 0.9995) ) + 0.9995;

  tonehole_.setA1( -new_coeff );
  tonehole_.setB0( new_coeff );
}

template<typename T>
void BlowHole<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "BlowHole::startBlowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( rate );
  envelope_.setTarget( amplitude );
}

template<typename T>
void BlowHole<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "BlowHole::stopBlowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( rate );
  envelope_.setTarget( 0.0 ); 
}

template<typename T>
void BlowHole<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( 0.55 + (amplitude * 0.30), amplitude * 0.005 );
  outputGain_ = amplitude + 0.001;
}

template<typename T>
void BlowHole<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.01 );
}

template<typename T>
void BlowHole<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "BlowHole::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_ReedStiffness_) // 2
    reedTable_.setSlope( -0.44 + (0.26 * normalizedValue) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = ( normalizedValue * 0.4);
  else if (number == __SK_ModFrequency_) // 11
    this->setTonehole( normalizedValue );
  else if (number == __SK_ModWheel_) // 1
    this->setVent( normalizedValue );
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope_.setValue( normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "BlowHole::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace


