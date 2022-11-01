#pragma once

#include "StkInstrmnt.hpp"
#include "StkADSR.hpp"
#include "StkFileLoop.hpp"
#include "StkSineWave.hpp"
#include "StkTwoZero.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class FM : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the number of wave/envelope operators to control.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  FM( unsigned int operators = 4 );

  //! Class destructor.
  virtual ~FM( void );

  //! Load the rawwave filenames in waves.
  void loadWaves( const char **filenames );

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency( T frequency );

  //! Set the frequency ratio for the specified wave.
  void setRatio( unsigned int waveIndex, T ratio );

  //! Set the gain for the specified wave.
  void setGain( unsigned int waveIndex, T gain );

  //! Set the modulation speed in Hz.
  void setModulationSpeed( T mSpeed ) { vibrato_.setFrequency( mSpeed ); };

  //! Set the modulation depth.
  void setModulationDepth( T mDepth ) { modDepth_ = mDepth; };

  //! Set the value of control1.
  void setControl1( T cVal ) { control1_ = cVal * 2.0; };

  //! Set the value of control1.
  void setControl2( T cVal ) { control2_ = cVal * 2.0; };

  //! Start envelopes toward "on" targets.
  void keyOn( void );

  //! Start envelopes toward "off" targets.
  void keyOff( void );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange( int number, T value );

  //! Compute and return one output sample.
  virtual T tick( unsigned int ) = 0;

  //! Fill a channel of the StkFrames<T> object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  virtual StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 ) = 0;

 protected:

  std::vector<ADSR<T>*> adsr_; 
  std::vector<FileLoop<T>*> waves_;
  SineWave<T> vibrato_;
  TwoZero<T>  twozero_;
  unsigned int nOperators_;
  T baseFrequency_;
  std::vector<T> ratios_;
  std::vector<T> gains_;
  T modDepth_;
  T control1_;
  T control2_;
  T fmGains_[100];
  T fmSusLevels_[16];
  T fmAttTimes_[32];

};

/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
FM<T>::FM( unsigned int operators )
  : nOperators_(operators)
{
  if ( nOperators_ == 0 ) {
    oStream_ << "FM::FM: Number of operators must be greater than zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  twozero_.setB2( -1.0 );
  twozero_.setGain( 0.0 );

  vibrato_.setFrequency( 6.0 );

  unsigned int j;
  adsr_.resize( nOperators_ );
  waves_.resize( nOperators_ );
  for (j=0; j<nOperators_; j++ ) {
    ratios_.push_back( 1.0 );
    gains_.push_back( 1.0 );
    adsr_[j] = new ADSR<T>();
  }

  modDepth_ = 0.0;
  control1_ = 1.0;
  control2_ = 1.0;
  baseFrequency_ = 440.0;

  int i;
  T temp = 1.0;
  for (i=99; i>=0; i--) {
    fmGains_[i] = temp;
    temp *= 0.933033;
  }

  temp = 1.0;
  for (i=15; i>=0; i--) {
    fmSusLevels_[i] = temp;
    temp *= 0.707101;
  }

  temp = 8.498186;
  for (i=0; i<32; i++) {
    fmAttTimes_[i] = temp;
    temp *= 0.707101;
  }
}

template<typename T>
FM<T>::~FM( void )
{
  for (unsigned int i=0; i<nOperators_; i++ ) {
    delete waves_[i];
    delete adsr_[i];
  }
}

template<typename T>
void FM<T>::loadWaves( const char **filenames )
{
  for (unsigned int i=0; i<nOperators_; i++ )
    waves_[i] = new FileLoop( filenames[i], true );
}

template<typename T>
void FM<T>::setFrequency( T frequency )
{    
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "FM::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  baseFrequency_ = frequency;
  for ( unsigned int i=0; i<nOperators_; i++ )
    waves_[i]->setFrequency( baseFrequency_ * ratios_[i] );
}

template<typename T>
void FM<T>::setRatio( unsigned int waveIndex, T ratio )
{
  if ( waveIndex >= nOperators_ ) {
    oStream_ << "FM:setRatio: waveIndex parameter is greater than the number of operators!";
    handleError( StkError::WARNING ); return;
  }

  ratios_[waveIndex] = ratio;
  if (ratio > 0.0) 
    waves_[waveIndex]->setFrequency( baseFrequency_ * ratio );
  else
    waves_[waveIndex]->setFrequency( ratio );
}

template<typename T>
void FM<T>::setGain( unsigned int waveIndex, T gain )
{
  if ( waveIndex >= nOperators_ ) {
    oStream_ << "FM::setGain: waveIndex parameter is greater than the number of operators!";
    handleError( StkError::WARNING ); return;
  }

  gains_[waveIndex] = gain;
}

template<typename T>
void FM<T>::keyOn( void )
{
  for ( unsigned int i=0; i<nOperators_; i++ )
    adsr_[i]->keyOn();
}

template<typename T>
void FM<T>::keyOff( void )
{
  for ( unsigned int i=0; i<nOperators_; i++ )
    adsr_[i]->keyOff();
}

template<typename T>
void FM<T>::noteOff( T amplitude )
{
  this->keyOff();
}

template<typename T>
void FM<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( Stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "FM::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_Breath_) // 2
    this->setControl1( normalizedValue );
  else if (number == __SK_FootControl_) // 4
    this->setControl2( normalizedValue );
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( normalizedValue * 12.0);
  else if (number == __SK_ModWheel_) // 1
    this->setModulationDepth( normalizedValue );
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    //adsr_[0]->setTarget( normalizedValue );
    adsr_[1]->setTarget( normalizedValue );
    //adsr_[2]->setTarget( normalizedValue );
    adsr_[3]->setTarget( normalizedValue );
  }
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "FM::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace


