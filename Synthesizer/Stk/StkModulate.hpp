#pragma once
#include "StkGenerator.hpp"
#include "StkSineWave.hpp"
#include "StkNoise.hpp"
#include "StkOnePole.hpp"

namespace stk {

/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Modulate : public Generator<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError can be thrown if the rawwave path is incorrect.
   */
  Modulate( void );

  //! Class destructor.
  ~Modulate( void );

  //! Reset internal state.
  void reset( void ) { this->lastFrame_[0] = 0.0; };

  //! Set the periodic (vibrato) rate or frequency in Hz.
  void setVibratoRate( T rate ) { vibrato_.setFrequency( rate ); };

  //! Set the periodic (vibrato) gain.
  void setVibratoGain( T gain ) { vibratoGain_ = gain; };

  //! Set the periodic (vibrato) rate or frequency in Hz.
  void setRandomRate( T rate ) {  noiseRate_ = (unsigned int) ( rate * stk::sampleRate() / 22050.0 ); };
  
  //! Set the random modulation gain.
  void setRandomGain( T gain );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Compute and return one output sample.
  T tick( void );

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

  void sampleRateChanged( T newRate, T oldRate );

  SineWave<T> vibrato_;
  Noise<T> noise_;
  OnePole<T>  filter_;
  T vibratoGain_;
  T randomGain_;
  unsigned int noiseRate_;
  unsigned int noiseCounter_;

};

template<typename T>
inline T Modulate<T>::tick( void )
{
  // Compute periodic and random modulations.
  this->lastFrame_[0] = vibratoGain_ * vibrato_.tick();
  if ( noiseCounter_++ >= noiseRate_ ) {
    noise_.tick();
    noiseCounter_ = 0;
  }
  this->lastFrame_[0] += filter_.tick( noise_.lastOut() );
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Modulate<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Modulate::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = Modulate::tick();

  return frames;
}

/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
Modulate<T>::Modulate( void )
{
  vibrato_.setFrequency( 6.0 );
  vibratoGain_ = 0.04;

  noiseRate_ = (unsigned int) ( 330.0 * stk::sampleRate() / 22050.0 );
  noiseCounter_ = noiseRate_;

  randomGain_ = 0.05;
  filter_.setPole( 0.999 );
  filter_.setGain( randomGain_ );

  this->addSampleRateAlert( this );
}

template<typename T>
Modulate<T>::~Modulate( void )
{
  this->removeSampleRateAlert( this );
}

template<typename T>
void Modulate<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( ! this->ignoreSampleRateChange_ )
    noiseRate_ = (unsigned int ) ( newRate * noiseRate_ / oldRate );
}

template<typename T>
void Modulate<T>::setRandomGain( T gain )
{
  randomGain_ = gain;
  filter_.setGain( randomGain_ );
}

} // stk namespace
