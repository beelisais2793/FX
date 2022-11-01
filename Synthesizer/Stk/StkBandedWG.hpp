#pragma once
#include "StkInstrmnt.hpp"
#include "StkDelayL.hpp"
#include "StkBowTable.hpp"
#include "StkADSR.hpp"
#include "StkBiQuad.hpp"
#include "SKINImsg.h"
#include <cmath>

namespace stk 
{

/***************************************************/
/*! \class BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999--2004.
    Modified for STK 4.0 by Gary Scavone.
*/
/***************************************************/

const int MAX_BANDED_MODES = 20;

template<typename T>
class BandedWG : public Instrmnt<T>
{
 public:
  //! Class constructor.
  BandedWG( void );

  //! Class destructor.
  ~BandedWG( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set strike position (0.0 - 1.0).
  void setStrikePosition( T position );

  //! Select a preset.
  void setPreset( int preset );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Apply bow velocity/pressure to instrument with given amplitude and rate of increase.
  void startBowing( T amplitude, T rate );

  //! Decrease bow velocity/breath pressure with given rate of decrease.
  void stopBowing( T rate );

  //! Pluck the instrument with given amplitude.
  void pluck( T amp );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, T value );

  //! Compute and return one output sample.
  T tick( unsigned int channel = 0 );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

 protected:

  bool doPluck_;
  bool trackVelocity_;
  int nModes_;
  int presetModes_;
  BowTable<T> bowTable_;
  ADSR<T>     adsr_;
  BiQuad<T>   bandpass_[MAX_BANDED_MODES];
  DelayL<T>   delay_[MAX_BANDED_MODES];
  T maxVelocity_;
  T modes_[MAX_BANDED_MODES];
  T frequency_;
  T baseGain_;
  T gains_[MAX_BANDED_MODES];
  T basegains_[MAX_BANDED_MODES];
  T excitation_[MAX_BANDED_MODES];
  T integrationConstant_;
  T velocityInput_;
  T bowVelocity_;
  T bowTarget_;
  T bowPosition_;
  T strikeAmp_;
  int strikePosition_;

};

template<typename T>
inline StkFrames<T>& BandedWG<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "BandedWG::tick(): channel and StkFrames arguments are incompatible!";
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
/*! \class BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999--2004.
    Modified for STK 4.0 by Gary Scavone.
*/
/***************************************************/

template<typename T>
BandedWG<T>::BandedWG( void )
{
  doPluck_ = true;

  bowTable_.setSlope( 3.0 );
  adsr_.setAllTimes( 0.02, 0.005, 0.9, 0.01 );

  frequency_ = 220.0;
  this->setPreset(0);

  bowPosition_ = 0;
  baseGain_ = 0.999;
  
  integrationConstant_ = 0.0;
  trackVelocity_ = false;

  bowVelocity_ = 0.0;
  bowTarget_ = 0.0;

  strikeAmp_ = 0.0;
}

template<typename T>
BandedWG<T>::~BandedWG( void )
{
}

template<typename T>
void BandedWG<T>::clear( void )
{
  for ( int i=0; i<nModes_; i++ ) {
    delay_[i].clear();
    bandpass_[i].clear();
  }
}

template<typename T>
void BandedWG<T>::setPreset( int preset )
{
  int i;
  switch ( preset ) {

  case 1: // Tuned Bar
    presetModes_ = 4;
    modes_[0] = (T) 1.0;
    modes_[1] = (T) 4.0198391420;
    modes_[2] = (T) 10.7184986595;
    modes_[3] = (T) 18.0697050938;

    for (i=0; i<presetModes_; i++) {
      basegains_[i] = (T) pow(0.999,(double) i+1);
      excitation_[i] = 1.0;
    }

    break;

  case 2: // Glass Harmonica
    presetModes_ = 5;
    modes_[0] = (T) 1.0;
    modes_[1] = (T) 2.32;
    modes_[2] = (T) 4.25;
    modes_[3] = (T) 6.63;
    modes_[4] = (T) 9.38;
    // modes_[5] = (T) 12.22;

    for (i=0; i<presetModes_; i++) {
      basegains_[i] = (T) pow(0.999,(double) i+1);
      excitation_[i] = 1.0;
    }
    /*
      baseGain_ = (T) 0.99999;
      for (i=0; i<presetModes_; i++) 
      gains_[i]= (T) pow(baseGain_, delay_[i].getDelay()+i);
    */

    break;
   
  case 3: // Tibetan Prayer Bowl (ICMC'02)
    presetModes_ = 12;
    modes_[0]=0.996108344;
    basegains_[0]=0.999925960128219;
    excitation_[0]=11.900357/10.0;
    modes_[1]=1.0038916562;
    basegains_[1]=0.999925960128219;
    excitation_[1]=11.900357/10.;
    modes_[2]=2.979178;
    basegains_[2]=0.999982774366897;
    excitation_[2]=10.914886/10.;
    modes_[3]=2.99329767;
    basegains_[3]=0.999982774366897;
    excitation_[3]=10.914886/10.;
    modes_[4]=5.704452;
    basegains_[4]=1.0; //0.999999999999999999987356406352;
    excitation_[4]=42.995041/10.;
    modes_[5]=5.704452;
    basegains_[5]=1.0; //0.999999999999999999987356406352;
    excitation_[5]=42.995041/10.;
    modes_[6]=8.9982;
    basegains_[6]=1.0; //0.999999999999999999996995497558225;
    excitation_[6]=40.063034/10.;
    modes_[7]=9.01549726;
    basegains_[7]=1.0; //0.999999999999999999996995497558225;
    excitation_[7]=40.063034/10.;
    modes_[8]=12.83303;
    basegains_[8]=0.999965497558225;
    excitation_[8]=7.063034/10.;
    modes_[9]=12.807382;
    basegains_[9]=0.999965497558225;
    excitation_[9]=7.063034/10.;
    modes_[10]=17.2808219;
    basegains_[10]=0.9999999999999999999965497558225;
    excitation_[10]=57.063034/10.;
    modes_[11]=21.97602739726;
    basegains_[11]=0.999999999999999965497558225;
    excitation_[11]=57.063034/10.;

    break;	

  default: // Uniform Bar
    presetModes_ = 4;
    modes_[0] = (T) 1.0;
    modes_[1] = (T) 2.756;
    modes_[2] = (T) 5.404;
    modes_[3] = (T) 8.933;

    for (i=0; i<presetModes_; i++) {
      basegains_[i] = (T) pow(0.9,(double) i+1);
      excitation_[i] = 1.0;
    }

    break;
  }

  nModes_ = presetModes_;
  setFrequency( frequency_ );
}

template<typename T>
void BandedWG<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "BandedWG::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  if (frequency > 1568.0) frequency = 1568.0;

  T radius;
  T base = sampleRate() / frequency;
  T length;
  for (int i=0; i<presetModes_; i++) {
    // Calculate the delay line lengths for each mode.
    length = (int)(base / modes_[i]);
    if ( length > 2.0) {
      delay_[i].setDelay( length );
      gains_[i]=basegains_[i];
      //	  gains_[i]=(T) pow(basegains_[i], 1/((T)delay_[i].getDelay()));
      //	  std::cerr << gains_[i];
    }
    else	{
      nModes_ = i;
      break;
    }
    //	std::cerr << std::endl;

    // Set the bandpass filter resonances
    radius = 1.0 - PI * 32 / sampleRate(); //frequency_ * modes_[i] / Stk::sampleRate()/32;
    if ( radius < 0.0 ) radius = 0.0;
    bandpass_[i].setResonance(frequency * modes_[i], radius, true);

    delay_[i].clear();
    bandpass_[i].clear();
  }

  //int olen = (int)(delay_[0].getDelay());
  //strikePosition_ = (int)(strikePosition_*(length/modes_[0])/olen);
}

template<typename T>
void BandedWG<T>::setStrikePosition( T position )
{
  strikePosition_ = (int)(delay_[0].getDelay() * position / 2.0);
}

template<typename T>
void BandedWG<T>::startBowing( T amplitude, T rate )
{
  adsr_.setAttackRate(rate);
  adsr_.keyOn();
  maxVelocity_ = 0.03 + (0.1 * amplitude); 
}

template<typename T>
void BandedWG<T>::stopBowing( T rate )
{
  adsr_.setReleaseRate(rate);
  adsr_.keyOff();
}

template<typename T>
void BandedWG<T>::pluck( T amplitude )
{
  int j;
  T min_len = delay_[nModes_-1].getDelay();
  for (int i=0; i<nModes_; i++)
    for(j=0; j<(int)(delay_[i].getDelay()/min_len); j++)
      delay_[i].tick( excitation_[i]*amplitude / nModes_ );

  //	strikeAmp_ += amplitude;
}

template<typename T>
void BandedWG<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );

  if ( doPluck_ )
    this->pluck( amplitude );
  else
    this->startBowing( amplitude, amplitude * 0.001 );
}

template<typename T>
void BandedWG<T>::noteOff( T amplitude )
{
  if ( !doPluck_ )
    this->stopBowing( (1.0 - amplitude) * 0.005 );
}

template<typename T>
T BandedWG<T>::tick( unsigned int )
{
  int k;

  T input = 0.0;
  if ( doPluck_ ) {
    input = 0.0;
    //  input = strikeAmp_/nModes_;
    //  strikeAmp_ = 0.0;
  }
  else {
    if ( integrationConstant_ == 0.0 )
      velocityInput_ = 0.0;
    else
      velocityInput_ = integrationConstant_ * velocityInput_;

    for ( k=0; k<nModes_; k++ )
      velocityInput_ += baseGain_ * delay_[k].lastOut();
      
    if ( trackVelocity_ )  {
      bowVelocity_ *= 0.9995;
      bowVelocity_ += bowTarget_;
      bowTarget_ *= 0.995;
    }
    else
      bowVelocity_ = adsr_.tick() * maxVelocity_;

    input = bowVelocity_ - velocityInput_;
    input = input * bowTable_.tick(input);
    input = input/(T)nModes_;
  }

  T data = 0.0;  
  for ( k=0; k<nModes_; k++ ) {
    bandpass_[k].tick(input + gains_[k] * delay_[k].lastOut());
    delay_[k].tick(bandpass_[k].lastOut());
    data += bandpass_[k].lastOut();
  }
  
  //lastFrame_[0] = data * nModes_;
  this->lastFrame_[0] = data * 4;
  return this->lastFrame_[0];
}

template<typename T>
void BandedWG<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( value < 0 || ( number != 101 && value > 128.0 ) ) {
    oStream_ << "BandedWG::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_BowPressure_) { // 2
    if ( normalizedValue == 0.0 )
      doPluck_ = true;
    else {
      doPluck_ = false;
      bowTable_.setSlope( 10.0 - (9.0 * normalizedValue));
    }
  }
  else if (number == 4)	{ // 4
    if ( !trackVelocity_ ) trackVelocity_ = true;
    bowTarget_ += 0.005 * (normalizedValue - bowPosition_);
    bowPosition_ = normalizedValue;
    //adsr_.setTarget(bowPosition_);
  }
  else if (number == 8) // 8
    this->setStrikePosition( normalizedValue );
  else if (number == __SK_AfterTouch_Cont_) { // 128
    //bowTarget_ += 0.02 * (normalizedValue - bowPosition_);
    //bowPosition_ = normalizedValue;
    if ( trackVelocity_ ) trackVelocity_ = false;
    maxVelocity_ = 0.13 * normalizedValue; 
    adsr_.setTarget(normalizedValue);
  }      
  else if (number == __SK_ModWheel_) { // 1
    //    baseGain_ = 0.9989999999 + (0.001 * normalizedValue );
	  baseGain_ = 0.8999999999999999 + (0.1 * normalizedValue);
    //	std::cerr << "Yuck!" << std::endl;
    for (int i=0; i<nModes_; i++)
      gains_[i]=(T) basegains_[i]*baseGain_;
    //      gains_[i]=(T) pow(baseGain_, (int)((T)delay_[i].getDelay()+i));
  }
  else if (number == __SK_ModFrequency_) // 11
    integrationConstant_ = normalizedValue;
  else if (number == __SK_Sustain_)	{ // 64
    if (value < 65) doPluck_ = true;
    else doPluck_ = false;
  }
  else if (number == __SK_Portamento_)	{ // 65
    if (value < 65) trackVelocity_ = false;
    else trackVelocity_ = true;
  }
  else if (number == __SK_ProphesyRibbon_) // 16
    this->setPreset((int) value);
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "BandedWG::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

}