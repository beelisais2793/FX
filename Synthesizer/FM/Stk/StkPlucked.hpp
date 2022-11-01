#pragma once
#include "StkInstrmnt.hpp"
#include "StkDelayA.hpp"
#include "StkOneZero.hpp"
#include "StkOnePole.hpp"
#include "StkNoise.hpp"

namespace stk {

/***************************************************/
/*! \class Plucked
    \brief STK basic plucked string class.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    For a more advanced plucked string implementation,
    see the stk::Twang class.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Plucked : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Plucked( T lowestFrequency = 10.0 );

  //! Class destructor.
  ~Plucked( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck( T amplitude );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

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

  DelayA<T>   delayLine_;
  OneZero<T>  loopFilter_;
  OnePole<T>  pickFilter_;
  Noise<T>    noise_;

  T loopGain_;
};

template<typename T>
inline T Plucked<T>::tick( unsigned int )
{
  // Here's the whole inner loop of the instrument!!
  return this->lastFrame_[0] = 3.0 * delayLine_.tick( loopFilter_.tick( delayLine_.lastOut() * loopGain_ ) ); 
}


template<typename T>
inline StkFrames<T>& Plucked<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Plucked::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Plucked
    \brief STK basic plucked string class.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    For a more advanced plucked string implementation,
    see the stk::Twang class.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
Plucked<T>::Plucked( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Plucked::Plucked: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long delays = (unsigned long) ( stk::sampleRate() / lowestFrequency );
  delayLine_.setMaximumDelay( delays + 1 );

  this->setFrequency( 220.0 );
}


template<typename T>
Plucked<T>::~Plucked( void )
{
}


template<typename T>
void Plucked<T>::clear( void )
{
  delayLine_.clear();
  loopFilter_.clear();
  pickFilter_.clear();
}


template<typename T>
void Plucked<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Plucked::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // Delay = length - filter delay.
  T delay = ( stk::sampleRate() / frequency ) - loopFilter_.phaseDelay( frequency );
  delayLine_.setDelay( delay );

  loopGain_ = 0.995 + (frequency * 0.000005);
  if ( loopGain_ >= 1.0 ) loopGain_ = 0.99999;
}


template<typename T>
void Plucked<T>::pluck( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "Plucked::pluck: amplitude is out of range!";
    handleError( StkError::WARNING ); return;
  }

  pickFilter_.setPole( 0.999 - (amplitude * 0.15) );
  pickFilter_.setGain( amplitude * 0.5 );
  for ( unsigned long i=0; i<delayLine_.getDelay(); i++ )
    // Fill delay with noise additively with current contents.
    delayLine_.tick( 0.6 * delayLine_.lastOut() + pickFilter_.tick( noise_.tick() ) );
}


template<typename T>
void Plucked<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->pluck( amplitude );
}


template<typename T>
void Plucked<T>::noteOff( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "Plucked::noteOff: amplitude is out of range!";
    handleError( StkError::WARNING ); return;
  }

  loopGain_ = 1.0 - amplitude;
}

} // stk namespace


