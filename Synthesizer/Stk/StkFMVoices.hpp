#pragma once
#include "StkFM.hpp"
#include "SKINImsg.h"
#include "StkPhonemes.hpp"

namespace stk {

/***************************************************/
/*! \class FMVoices
    \brief STK singing FM synthesis instrument.

    This class implements 3 carriers and a common
    modulator, also referred to as algorithm 6 of
    the TX81Z.

    \code
    Algorithm 6 is :
                        /->1 -\
                     4-|-->2 - +-> Out
                        \->3 -/
    \endcode

    Control Change Numbers: 
       - Vowel = 2
       - Spectral Tilt = 4
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
class FMVoices : public FM<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  FMVoices( void );

  //! Class destructor.
  ~FMVoices( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

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

  int currentVowel_;
  T tilt_[3];
  T mods_[3];
};

template<typename T>
inline T FMVoices <T>:: tick( unsigned int )
{
  T temp, temp2;

  temp = this->gains_ [3] * this->adsr_ [3]->tick() * this->waves_ [3]->tick();
  temp2 = this->vibrato_ .tick() * this->modDepth_  * 0.1;

  
  this->waves_ [0]->setFrequency(this->baseFrequency_  * (1.0 + temp2) * this->ratios_ [0]);
  this->waves_ [1]->setFrequency(this->baseFrequency_  * (1.0 + temp2) * this->ratios_ [1]);
  this->waves_ [2]->setFrequency(this->baseFrequency_  * (1.0 + temp2) * this->ratios_ [2]);
  this->waves_ [3]->setFrequency(this->baseFrequency_  * (1.0 + temp2) * this->ratios_ [3]);

  this->waves_ [0]->addPhaseOffset(temp * mods_[0]);
  this->waves_ [1]->addPhaseOffset(temp * mods_[1]);
  this->waves_ [2]->addPhaseOffset(temp * mods_[2]);
  this->waves_ [3]->addPhaseOffset( this->twozero_ .lastOut() );
  this->twozero_ .tick( temp );
  temp =  this->gains_ [0] * tilt_[0] * this->adsr_ [0]->tick() * this->waves_ [0]->tick();
  temp += this->gains_ [1] * tilt_[1] * this->adsr_ [1]->tick() * this->waves_ [1]->tick();
  temp += this->gains_ [2] * tilt_[2] * this->adsr_ [2]->tick() * this->waves_ [2]->tick();

  this->lastFrame_[0] = temp * 0.33;
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& FMVoices <T>:: tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "FMVoices::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class FMVoices
    \brief STK singing FM synthesis instrument.

    This class implements 3 carriers and a common
    modulator, also referred to as algorithm 6 of
    the TX81Z.

    \code
    Algorithm 6 is :
                        /->1 -\
                     4-|-->2 - +-> Out
                        \->3 -/
    \endcode

    Control Change Numbers: 
       - Vowel = 2
       - Spectral Tilt = 4
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
FMVoices <T>:: FMVoices( void )
  : FM<T>()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( unsigned int i=0; i<3; i++ )
    this->waves_ [i] = new FileLoop<T> ( (rawwavePath() + "sinewave.raw").c_str(), true );
  this->waves_ [3] = new FileLoop<T> ( (rawwavePath() + "fwavblnk.raw").c_str(), true );

  this->setRatio(0, 2.00);
  this->setRatio(1, 4.00);
  this->setRatio(2, 12.0);
  this->setRatio(3, 1.00);

  this->gains_ [3] = this->fmGain_ [80];

  this->adsr_ [0]->setAllTimes( 0.05, 0.05, this->fmSusLevels_ [15], 0.05);
  this->adsr_ [1]->setAllTimes( 0.05, 0.05, this->fmSusLevels_ [15], 0.05);
  this->adsr_ [2]->setAllTimes( 0.05, 0.05, this->fmSusLevels_ [15], 0.05);
  this->adsr_ [3]->setAllTimes( 0.01, 0.01, this->fmSusLevels_ [15], 0.5);

  this->twozero_ .setGain( 0.0 );
  this->modDepth_  = (T) 0.005;
  currentVowel_ = 0;
  tilt_[0] = 1.0;
  tilt_[1] = 0.5;
  tilt_[2] = 0.2;    
  mods_[0] = 1.0;
  mods_[1] = 1.1;
  mods_[2] = 1.1;
  this->baseFrequency_  = 110.0;
  this->setFrequency( 110.0 );    
}  

template<typename T>
FMVoices <T>:: ~FMVoices( void )
{
}

template<typename T>
void FMVoices <T>:: setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "FMVoices::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T temp, temp2 = 0.0;
  int tempi = 0;
  unsigned int i = 0;

  if (currentVowel_ < 32)	{
    i = currentVowel_;
    temp2 = 0.9;
  }
  else if (currentVowel_ < 64)	{
    i = currentVowel_ - 32;
    temp2 = 1.0;
  }
  else if (currentVowel_ < 96)	{
    i = currentVowel_ - 64;
    temp2 = 1.1;
  }
  else if (currentVowel_ < 128)	{
    i = currentVowel_ - 96;
    temp2 = 1.2;
  }
  else return;

  this->baseFrequency_  = frequency;
  temp = (temp2 * Phonemes<T>:: formantFrequency(i, 0) / this->baseFrequency_ ) + 0.5;
  tempi = (int) temp;
  this->setRatio( 0, (T) tempi );
  temp = (temp2 * Phonemes<T>:: formantFrequency(i, 1) / this->baseFrequency_ ) + 0.5;
  tempi = (int) temp;
  this->setRatio( 1, (T) tempi );
  temp = (temp2 * Phonemes<T>:: formantFrequency(i, 2) / this->baseFrequency_ ) + 0.5;
  tempi = (int) temp;
  this->setRatio( 2, (T) tempi );    
  this->gains_ [0] = 1.0;
  this->gains_ [1] = 1.0;
  this->gains_ [2] = 1.0;
}

template<typename T>
void FMVoices <T>:: noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  tilt_[0] = amplitude;
  tilt_[1] = amplitude * amplitude;
  tilt_[2] = tilt_[1] * amplitude;
  this->keyOn();
}

template<typename T>
void FMVoices <T>:: controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "FMVoices::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_Breath_) // 2
    this->gains_ [3] = this->fmGain_ [(int) ( normalizedValue * 99.9 )];
  else if (number == __SK_FootControl_)	{ // 4
    currentVowel_ = (int) (normalizedValue * 127.0);
    this->setFrequency(this->baseFrequency_ );
  }
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( normalizedValue * 12.0);
  else if (number == __SK_ModWheel_) // 1
    this->setModulationDepth( normalizedValue );
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    tilt_[0] = normalizedValue;
    tilt_[1] = normalizedValue * normalizedValue;
    tilt_[2] = tilt_[1] * normalizedValue;
  }
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "FMVoices::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

