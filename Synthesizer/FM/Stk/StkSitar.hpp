#pragma once
#include "StkInstrmnt.hpp"
#include "StkDelayA.hpp"
#include "StkOneZero.hpp"
#include "StkNoise.hpp"
#include "StkADSR.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Sitar
    \brief STK sitar string model class.

    This class implements a sitar plucked string
    physical model based on the Karplus-Strong
    algorithm.

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
class Sitar : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Sitar( T lowestFrequency = 8.0 );

  //! Class destructor.
  ~Sitar( void );

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

  DelayA<T>  delayLine_;
  OneZero<T> loopFilter_;
  Noise<T>   noise_;
  ADSR<T>    envelope_;

  T loopGain_;
  T amGain_;
  T delay_;
  T targetDelay_;

};

template<typename T>
inline T Sitar<T>::tick( unsigned int )
{
  if ( fabs(targetDelay_ - delay_) > 0.001 ) {
    if ( targetDelay_ < delay_ )
      delay_ *= 0.99999;
    else
      delay_ *= 1.00001;
    delayLine_.setDelay( delay_ );
  }

  this->lastFrame_[0] = delayLine_.tick( loopFilter_.tick( delayLine_.lastOut() * loopGain_ ) + 
                                (amGain_ * envelope_.tick() * noise_.tick()));
  
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Sitar<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Sitar::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Sitar
    \brief STK sitar string model class.

    This class implements a sitar plucked string
    physical model based on the Karplus-Strong
    algorithm.

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
Sitar<T>::Sitar( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Sitar::Sitar: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned long length = (unsigned long) ( stk::sampleRate() / lowestFrequency + 1 );
  delayLine_.setMaximumDelay( length );
  delay_ = 0.5 * length;
  delayLine_.setDelay( delay_ );
  targetDelay_ = delay_;

  loopFilter_.setZero( 0.01 );
  loopGain_ = 0.999;

  envelope_.setAllTimes( 0.001, 0.04, 0.0, 0.5 );
  this->clear();
}

template<typename T>
Sitar<T>::~Sitar( void )
{
}

template<typename T>
void Sitar<T>::clear( void )
{
  delayLine_.clear();
  loopFilter_.clear();
}

template<typename T>
void Sitar<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Sitar::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  targetDelay_ = (stk::sampleRate() / frequency);
  delay_ = targetDelay_ * (1.0 + (0.05 * noise_.tick()));
  delayLine_.setDelay( delay_ );
  loopGain_ = 0.995 + (frequency * 0.0000005);
  if ( loopGain_ > 0.9995 ) loopGain_ = 0.9995;
}

template<typename T>
void Sitar<T>::pluck( T amplitude )
{
  envelope_.keyOn();
}

template<typename T>
void Sitar<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->pluck( amplitude );
  amGain_ = 0.1 * amplitude;
}

template<typename T>
void Sitar<T>::noteOff( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "Sitar::noteOff: amplitude is out of range!";
    handleError( StkError::WARNING ); return;
  }

  loopGain_ = (T) 1.0 - amplitude;
}

} // stk namespace



