#pragma once
#include "StkInstrmnt.hpp"
#include "StkEnvelope.hpp"
#include "StkNoise.hpp"
#include "StkSingWave.hpp"
#include "StkFormSwep.hpp"
#include "StkOnePole.hpp"
#include "StkOneZero.hpp"
#include "StkPhonemes.hpp"
#include "SKINImsg.h"
#include <cstring>
#include <cmath>

namespace stk {

/***************************************************/
/*! \class VoicForm
    \brief Four formant synthesis instrument.

    This instrument contains an excitation singing
    wavetable (looping wave with random and
    periodic vibrato, smoothing on frequency,
    etc.), excitation noise, and four sweepable
    complex resonances.

    Measured formant data is included, and enough
    data is there to support either parallel or
    cascade synthesis.  In the floating point case
    cascade synthesis is the most natural so
    that's what you'll find here.

    Control Change Numbers: 
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class VoicForm : public Instrmnt<T>
{
  public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  VoicForm( void );

  //! Class destructor.
  ~VoicForm( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set instrument parameters for the given phoneme.  Returns false if phoneme not found.
  bool setPhoneme( const char* phoneme );

  //! Set the voiced component gain.
  void setVoiced( T vGain ) { voiced_->setGainTarget(vGain); };

  //! Set the unvoiced component gain.
  void setUnVoiced( T nGain ) { noiseEnv_.setTarget(nGain); };

  //! Set the sweep rate for a particular formant filter (0-3).
  void setFilterSweepRate( unsigned int whichOne, T rate );

  //! Set voiced component pitch sweep rate.
  void setPitchSweepRate( T rate ) { voiced_->setSweepRate(rate); };

  //! Start the voice.
  void speak( void ) { voiced_->noteOn(); };

  //! Stop the voice.
  void quiet( void );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude ) { this->quiet(); };

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

  SingWave<T> *voiced_;
  Noise<T>    noise_;
  Envelope<T> noiseEnv_;
  FormSwep<T> filters_[4];
  OnePole<T>  onepole_;
  OneZero<T>  onezero_;

};



template<typename T>
inline T VoicForm<T>::tick( unsigned int )
{
  T temp;
  temp = onepole_.tick( onezero_.tick( voiced_->tick() ) );
  temp += noiseEnv_.tick() * noise_.tick();
  this->lastFrame_[0] = filters_[0].tick(temp);
  this->lastFrame_[0] += filters_[1].tick(temp);
  this->lastFrame_[0] += filters_[2].tick(temp);
  this->lastFrame_[0] += filters_[3].tick(temp);
  /*
    temp  += noiseEnv_.tick() * noise_.tick();
    this->lastFrame_[0]  = filters_[0].tick(temp);
    this->lastFrame_[0]  = filters_[1].tick(this->lastFrame_[0]);
    this->lastFrame_[0]  = filters_[2].tick(this->lastFrame_[0]);
    this->lastFrame_[0]  = filters_[3].tick(this->lastFrame_[0]);
  */
  return this->lastFrame_[0];
}



template<typename T>
inline StkFrames<T>& VoicForm<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "VoicForm::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class VoicForm
    \brief Four formant synthesis instrument.

    This instrument contains an excitation singing
    wavetable (looping wave with random and
    periodic vibrato, smoothing on frequency,
    etc.), excitation noise, and four sweepable
    complex resonances.

    Measured formant data is included, and enough
    data is there to support either parallel or
    cascade synthesis.  In the floating point case
    cascade synthesis is the most natural so
    that's what you'll find here.

    Control Change Numbers: 
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/




template<typename T>
VoicForm<T>::VoicForm( void ) : Instrmnt<T>()
{
  // Concatenate the STK rawwave path to the rawwave file
  voiced_ = new SingWave<T>( (stk::rawwavePath() + "impuls20.raw").c_str(), true );
  voiced_->setGainRate( 0.001 );
  voiced_->setGainTarget( 0.0 );

  for ( int i=0; i<4; i++ )
    filters_[i].setSweepRate( 0.001 );
    
  onezero_.setZero( -0.9 );
  onepole_.setPole( 0.9 );
    
  noiseEnv_.setRate( 0.001 );
  noiseEnv_.setTarget( 0.0 );
    
  this->setPhoneme( "eee" );
  this->clear();
}  


template<typename T>
VoicForm<T>::~VoicForm( void )
{
  delete voiced_;
}


template<typename T>
void VoicForm<T>::clear( void )
{
  onezero_.clear();
  onepole_.clear();
  for ( int i=0; i<4; i++ ) {
    filters_[i].clear();
  }
}


template<typename T>
void VoicForm<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "VoicForm::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  voiced_->setFrequency( frequency );
}


template<typename T>
bool VoicForm<T>::setPhoneme( const char *phoneme )
{
  bool found = false;
  unsigned int i = 0;
  while( i < 32 && !found ) {
    if ( !strcmp( Phonemes<T>::name(i), phoneme ) ) {
      found = true;
      filters_[0].setTargets( Phonemes<T>::formantFrequency(i, 0), Phonemes<T>::formantRadius(i, 0), pow(10.0, Phonemes<T>::formantGain(i, 0 ) / 20.0) );
      filters_[1].setTargets( Phonemes<T>::formantFrequency(i, 1), Phonemes<T>::formantRadius(i, 1), pow(10.0, Phonemes<T>::formantGain(i, 1 ) / 20.0) );
      filters_[2].setTargets( Phonemes<T>::formantFrequency(i, 2), Phonemes<T>::formantRadius(i, 2), pow(10.0, Phonemes<T>::formantGain(i, 2 ) / 20.0) );
      filters_[3].setTargets( Phonemes<T>::formantFrequency(i, 3), Phonemes<T>::formantRadius(i, 3), pow(10.0, Phonemes<T>::formantGain(i, 3 ) / 20.0) );
      this->setVoiced( Phonemes<T>::voiceGain( i ) );
      this->setUnVoiced( Phonemes<T>::noiseGain( i ) );
    }
    i++;
  }

  if ( !found ) {
    oStream_ << "VoicForm::setPhoneme: phoneme " << phoneme << " not found!";
    handleError( StkError::WARNING );
  }

  return found;
}


template<typename T>
void VoicForm<T>::setFilterSweepRate( unsigned int whichOne, T rate )
{
  if ( whichOne > 3 ) {
    oStream_ << "VoicForm::setFilterSweepRate: filter select argument outside range 0-3!";
    handleError( StkError::WARNING ); return;
  }

  filters_[whichOne].setSweepRate(rate);
}


template<typename T>
void VoicForm<T>::quiet( void )
{
  voiced_->noteOff();
  noiseEnv_.setTarget( 0.0 );
}


template<typename T>
void VoicForm<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  voiced_->setGainTarget( amplitude );
  onepole_.setPole( 0.97 - (amplitude * 0.2) );
}


template<typename T>
void VoicForm<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "VoicForm::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == __SK_Breath_)	{ // 2
    this->setVoiced( 1.0 - normalizedValue );
    this->setUnVoiced( 0.01 * normalizedValue );
  }
  else if (number == __SK_FootControl_)	{ // 4
    T temp = 0.0;
    unsigned int i = (int) value;
    if (i < 32)	{
      temp = 0.9;
    }
    else if (i < 64)	{
      i -= 32;
      temp = 1.0;
    }
    else if (i < 96)	{
      i -= 64;
      temp = 1.1;
    }
    else if (i < 128)	{
      i -= 96;
      temp = 1.2;
    }
    else if (i == 128)	{
      i = 0;
      temp = 1.4;
    }
    filters_[0].setTargets( temp * Phonemes<T>::formantFrequency(i, 0), Phonemes<T>::formantRadius(i, 0), pow(10.0, Phonemes<T>::formantGain(i, 0 ) / 20.0) );
    filters_[1].setTargets( temp * Phonemes<T>::formantFrequency(i, 1), Phonemes<T>::formantRadius(i, 1), pow(10.0, Phonemes<T>::formantGain(i, 1 ) / 20.0) );
    filters_[2].setTargets( temp * Phonemes<T>::formantFrequency(i, 2), Phonemes<T>::formantRadius(i, 2), pow(10.0, Phonemes<T>::formantGain(i, 2 ) / 20.0) );
    filters_[3].setTargets( temp * Phonemes<T>::formantFrequency(i, 3), Phonemes<T>::formantRadius(i, 3), pow(10.0, Phonemes<T>::formantGain(i, 3 ) / 20.0) );
    this->setVoiced( Phonemes<T>::voiceGain( i ) );
    this->setUnVoiced( Phonemes<T>::noiseGain( i ) );
  }
  else if (number == __SK_ModFrequency_) // 11
    voiced_->setVibratoRate( normalizedValue * 12.0);  // 0 to 12 Hz
  else if (number == __SK_ModWheel_) // 1
    voiced_->setVibratoGain( normalizedValue * 0.2);
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    this->setVoiced( normalizedValue );
    onepole_.setPole( 0.97 - ( normalizedValue * 0.2) );
  }
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "VoicForm::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

