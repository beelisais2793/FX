#pragma once
#include "StkInstrmnt.hpp"
#include "StkSphere.hpp"
#include "StkVector3D.hpp"
#include "StkNoise.hpp"
#include "StkSineWave.hpp"
#include "StkOnePole.hpp"
#include "StkEnvelope.hpp"
#include "SKINImsg.h"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1995--2021.
*/
/***************************************************/

template<typename T>
class Whistle : public Instrmnt<T>
{
public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Whistle( void );

  //! Class destructor.
  ~Whistle( void );

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

	Vector3D<T> *tempVectorP_;
  Vector3D<T> tempVector_;
  OnePole<T> onepole_;
  Noise<T> noise_;
	Envelope<T> envelope_;
  Sphere<T> can_;           // Declare a Spherical "can".
  Sphere<T> pea_, bumper_;  // One spherical "pea", and a spherical "bumper".

  SineWave<T> sine_;

  T baseFrequency_;
  T noiseGain_;
  T fippleFreqMod_;
	T fippleGainMod_;
	T blowFreqMod_;
	T tickSize_;
	T canLoss_;
	int subSample_, subSampCount_;
};


template<typename T>
inline StkFrames<T>& Whistle<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Whistle::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \Class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1995--2021.
*/
/***************************************************/




const int CAN_RADIUS = 100;
const int PEA_RADIUS = 30;
const int BUMP_RADIUS = 5;

const double NORM_CAN_LOSS = 0.97;
//const double SLOW_CAN_LOSS = 0.90;
const double GRAVITY = 20.0;

const double NORM_TICK_SIZE = 0.004;
//const T SLOW_TICK_SIZE = 0.0001;

const double ENV_RATE = 0.001;

template<typename T>
Whistle<T>::Whistle( void )
{
  sine_.setFrequency( 2800.0 );

  can_.setRadius( CAN_RADIUS );
  can_.setPosition(0, 0, 0); // set can location
  can_.setVelocity(0, 0, 0); // and the velocity

  onepole_.setPole(0.95);  // 0.99

  bumper_.setRadius( BUMP_RADIUS );
  bumper_.setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  bumper_.setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);

  pea_.setRadius( PEA_RADIUS );
  pea_.setPosition(0, CAN_RADIUS/2, 0);
  pea_.setVelocity(35, 15, 0);

  envelope_.setRate( ENV_RATE );
  envelope_.keyOn();

  fippleFreqMod_ = 0.5;
  fippleGainMod_ = 0.5;
  blowFreqMod_ = 0.25;
  noiseGain_ = 0.125;
  baseFrequency_ = 2000;

  tickSize_ = NORM_TICK_SIZE;
  canLoss_ = NORM_CAN_LOSS;

  subSample_ = 1;
  subSampCount_ = subSample_;
}

template<typename T>
Whistle<T>::~Whistle( void )
{
#ifdef WHISTLE_ANIMATION
  printf("Exit, Whistle bye bye!!\n");
#endif
}

template<typename T>
void Whistle<T>::clear( void )
{
}

template<typename T>
void Whistle<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Whistle::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  baseFrequency_ = frequency * 4;  // the whistle is a transposing instrument
}


template<typename T>
void Whistle<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "Whistle::startBlowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( ENV_RATE );
  envelope_.setTarget( amplitude );
}


template<typename T>
void Whistle<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "Whistle::stopBlowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  envelope_.setRate( rate );
  envelope_.keyOff();
}


template<typename T>
void Whistle<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( amplitude*2.0 ,amplitude * 0.2 );
}


template<typename T>
void Whistle<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.02 );
}

int frameCount = 0;


template<typename T>
T Whistle<T>::tick( unsigned int )
{
  T soundMix, tempFreq;
  T envOut = 0, temp, temp1, temp2, tempX, tempY;
  double phi, cosphi, sinphi;
  double gain = 0.5, mod = 0.0;

  if ( --subSampCount_ <= 0 )	{
    tempVectorP_ = pea_.getPosition();
    subSampCount_ = subSample_;
    temp = bumper_.isInside( tempVectorP_ );
#ifdef WHISTLE_ANIMATION
    frameCount += 1;
    if ( frameCount >= (1470 / subSample_) ) {
      frameCount = 0;
      printf("%f %f %f\n",tempVectorP_->getX(),tempVectorP_->getY(),envOut);
      fflush(stdout);
    }
#endif
    envOut = envelope_.tick();

    if (temp < (BUMP_RADIUS + PEA_RADIUS)) {
      tempX = envOut * tickSize_ * 2000 * noise_.tick();
      tempY = -envOut * tickSize_ * 1000 * (1.0 + noise_.tick());
      pea_.addVelocity( tempX, tempY, 0 ); 
      pea_.tick( tickSize_ );
    }

    mod  = exp(-temp * 0.01);	  // exp. distance falloff of fipple/pea effect
    temp = onepole_.tick(mod);	// smooth it a little
    gain = (1.0 - (fippleGainMod_*0.5)) + (2.0 * fippleGainMod_ * temp);
    gain *= gain;	              // squared distance/gain
    //    tempFreq = 1.0				//  Normalized Base Freq
    //			+ (fippleFreqMod_ * 0.25) - (fippleFreqMod_ * temp) // fippleModulation 
    //			- (blowFreqMod_) + (blowFreqMod_ * envOut); // blowingModulation
    // short form of above
    tempFreq = 1.0 + fippleFreqMod_*(0.25-temp) + blowFreqMod_*(envOut-1.0);
    tempFreq *= baseFrequency_;

    sine_.setFrequency(tempFreq);

    tempVectorP_ = pea_.getPosition();
    temp = can_.isInside(tempVectorP_);
    temp  = -temp;       // We know (hope) it's inside, just how much??
    if (temp < (PEA_RADIUS * 1.25)) {
      pea_.getVelocity( &tempVector_ );  // This is the can/pea collision
      tempX = tempVectorP_->getX();     // calculation.  Could probably
      tempY = tempVectorP_->getY();     // simplify using tables, etc.
      phi = -atan2(tempY,tempX);

      cosphi = cos(phi);
      sinphi = sin(phi);
      temp1 = (cosphi*tempVector_.getX()) - (sinphi*tempVector_.getY());
      temp2 = (sinphi*tempVector_.getX()) + (cosphi*tempVector_.getY());
      temp1 = -temp1;
      tempX = (cosphi*temp1) + (sinphi*temp2);
      tempY = (-sinphi*temp1) + (cosphi*temp2);
      pea_.setVelocity(tempX, tempY, 0);
      pea_.tick(tickSize_);
      pea_.setVelocity( tempX*canLoss_, tempY*canLoss_, 0 );
      pea_.tick(tickSize_);
    }

    temp = tempVectorP_->getLength();	
    if (temp > 0.01) {
      tempX = tempVectorP_->getX();
      tempY = tempVectorP_->getY();
      phi = atan2( tempY, tempX );
      phi += 0.3 * temp / CAN_RADIUS;
      cosphi = cos(phi);
      sinphi = sin(phi);
      tempX = 3.0 * temp * cosphi;
      tempY = 3.0 * temp * sinphi;
    }
    else {
      tempX = 0.0;
      tempY = 0.0;
    }

    temp = (0.9 + 0.1*subSample_*noise_.tick()) * envOut * 0.6 * tickSize_;
    pea_.addVelocity( temp * tempX, (temp*tempY) - (GRAVITY*tickSize_), 0 );
    pea_.tick( tickSize_ );

    // bumper_.tick(0.0);
  }

  temp = envOut * envOut * gain / 2;
  soundMix = temp * ( sine_.tick() + ( noiseGain_*noise_.tick() ) );
  this->lastFrame_[0] = 0.20 * soundMix; // should probably do one-zero filter here

  return this->lastFrame_[0];
}


template<typename T>
void Whistle<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( Stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Whistle::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if ( number == __SK_NoiseLevel_ ) // 4
    noiseGain_ = 0.25 * normalizedValue;
  else if ( number == __SK_ModFrequency_ ) // 11
    fippleFreqMod_ = normalizedValue;
  else if ( number == __SK_ModWheel_ ) // 1
    fippleGainMod_ = normalizedValue;
  else if ( number == __SK_AfterTouch_Cont_ ) // 128
    envelope_.setTarget( normalizedValue * 2.0 );
  else if ( number == __SK_Breath_ ) // 2
    blowFreqMod_ = normalizedValue * 0.5;
  else if ( number == __SK_Sustain_ )	{ // 64
    subSample_ = (int) value;
    if ( subSample_ < 1.0 ) subSample_ = 1;
    envelope_.setRate( ENV_RATE / subSample_ );
  }
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Whistle::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace
