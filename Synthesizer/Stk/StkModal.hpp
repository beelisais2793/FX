#pragma once
#include "StkInstrmnt.hpp"
#include "StkEnvelope.hpp"
#include "StkFileLoop.hpp"
#include "StkSineWave.hpp"
#include "StkBiQuad.hpp"
#include "StkOnePole.hpp"
#include <cstdlib>

namespace stk {

/***************************************************/
/*! \class Modal
    \brief STK resonance model abstract base class.

    This class contains an excitation wavetable,
    an envelope, an oscillator, and N resonances
    (non-sweeping BiQuad<T>filters), where N is set
    during instantiation.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Modal : public Instrmnt<T>
{
public:
  //! Class constructor, taking the desired number of modes to create.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Modal( unsigned int modes = 4 );

  //! Class destructor.
  virtual ~Modal( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency( T frequency );

  //! Set the ratio and radius for a specified mode filter.
  void setRatioAndRadius( unsigned int modeIndex, T ratio, T radius );

  //! Set the master gain.
  void setMasterGain( T aGain ) { masterGain_ = aGain; };

  //! Set the direct gain.
  void setDirectGain( T aGain ) { directGain_ = aGain; };

  //! Set the gain for a specified mode filter.
  void setModeGain( unsigned int modeIndex, T gain );

  //! Initiate a strike with the given amplitude (0.0 - 1.0).
  virtual void strike( T amplitude );

  //! Damp modes with a given decay factor (0.0 - 1.0).
  void damp( T amplitude );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange( int number, T value ) = 0;

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

  Envelope<T> envelope_; 
  FileWvIn<T> *wave_;
  BiQuad<T> **filters_;
  OnePole<T>  onepole_;
  SineWave<T> vibrato_;

  unsigned int nModes_;
  std::vector<T> ratios_;
  std::vector<T> radii_;

  T vibratoGain_;
  T masterGain_;
  T directGain_;
  T stickHardness_;
  T strikePosition_;
  T baseFrequency_;
};

template<typename T>
inline T Modal<T>::tick( unsigned int )
{
  T temp = masterGain_ * onepole_.tick( wave_->tick() * envelope_.tick() );

  T temp2 = 0.0;
  for ( unsigned int i=0; i<nModes_; i++ )
    temp2 += filters_[i]->tick(temp);

  temp2  -= temp2 * directGain_;
  temp2 += directGain_ * temp;

  if ( vibratoGain_ != 0.0 ) {
    // Calculate AM and apply to master out
    temp = 1.0 + ( vibrato_.tick() * vibratoGain_ );
    temp2 = temp * temp2;
  }
    
  this->lastFrame_[0] = temp2;
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Modal<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Modal::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Modal
    \brief STK resonance model abstract base class.

    This class contains an excitation wavetable,
    an envelope, an oscillator, and N resonances
    (non-sweeping BiQuad<T>filters), where N is set
    during instantiation.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Modal<T>::Modal( unsigned int modes )
  : nModes_(modes)
{
  if ( nModes_ == 0 ) {
    oStream_ << "Modal: 'modes' argument to constructor is zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // We don't make the excitation wave here yet, because we don't know
  // what it's going to be.

  ratios_.resize( nModes_ );
  radii_.resize( nModes_ );
  filters_ = (BiQuad<T>**) calloc( nModes_, sizeof(BiQuad<T>*) );
  for (unsigned int i=0; i<nModes_; i++ ) {
    filters_[i] = new BiQuad<T>;
    filters_[i]->setEqualGainZeroes();
  }

  // Set some default values.
  vibrato_.setFrequency( 6.0 );
  vibratoGain_ = 0.0;
  directGain_ = 0.0;
  masterGain_ = 1.0;
  baseFrequency_ = 440.0;

  this->clear();

  stickHardness_ =  0.5;
  strikePosition_ = 0.561;
}  

template<typename T>
Modal<T>::~Modal( void )
{
  for ( unsigned int i=0; i<nModes_; i++ ) {
    delete filters_[i];
  }
  free( filters_ );
}

template<typename T>
void Modal<T>::clear( void )
{    
  onepole_.clear();
  for ( unsigned int i=0; i<nModes_; i++ )
    filters_[i]->clear();
}

template<typename T>
void Modal<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Modal::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  baseFrequency_ = frequency;
  for ( unsigned int i=0; i<nModes_; i++ )
    this->setRatioAndRadius( i, ratios_[i], radii_[i] );
}

template<typename T>
void Modal<T>::setRatioAndRadius( unsigned int modeIndex, T ratio, T radius )
{
  if ( modeIndex >= nModes_ ) {
    oStream_ << "Modal::setRatioAndRadius: modeIndex parameter is greater than number of modes!";
    handleError( StkError::WARNING ); return;
  }

  T nyquist = stk::sampleRate() / 2.0;
  T temp;

  if ( ratio * baseFrequency_ < nyquist ) {
    ratios_[modeIndex] = ratio;
  }
  else {
    temp = ratio;
    while (temp * baseFrequency_ > nyquist) temp *= 0.5;
    ratios_[modeIndex] = temp;
#if defined(_STK_DEBUG_)
    oStream_ << "Modal::setRatioAndRadius: aliasing would occur here ... correcting.";
    handleError( StkError::DEBUG_PRINT );
#endif
  }
  radii_[modeIndex] = radius;
  if (ratio < 0) 
    temp = -ratio;
  else
    temp = ratio * baseFrequency_;

  filters_[modeIndex]->setResonance(temp, radius);
}

template<typename T>
void Modal<T>::setModeGain( unsigned int modeIndex, T gain )
{
  if ( modeIndex >= nModes_ ) {
    oStream_ << "Modal::setModeGain: modeIndex parameter is greater than number of modes!";
    handleError( StkError::WARNING ); return;
  }

  filters_[modeIndex]->setGain( gain );
}

template<typename T>
void Modal<T>::strike( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "Modal::strike: amplitude is out of range!";
    handleError( StkError::WARNING );
  }

  envelope_.setRate( 1.0 );
  envelope_.setTarget( amplitude );
  onepole_.setPole( 1.0 - amplitude );
  envelope_.tick();
  wave_->reset();

  T temp;
  for ( unsigned int i=0; i<nModes_; i++ ) {
    if (ratios_[i] < 0)
      temp = -ratios_[i];
    else
      temp = ratios_[i] * baseFrequency_;
    filters_[i]->setResonance(temp, radii_[i]);
  }
}

template<typename T>
void Modal<T>::noteOn( T frequency, T amplitude )
{
  this->strike( amplitude );
  this->setFrequency( frequency );
}

template<typename T>
void Modal<T>::noteOff( T amplitude )
{
  // This calls damp, but inverts the meaning of amplitude (high
  // amplitude means fast damping).
  this->damp( 1.0 - (amplitude * 0.03) );
}

template<typename T>
void Modal<T>::damp( T amplitude )
{
  T temp;
  for ( unsigned int i=0; i<nModes_; i++ ) {
    if ( ratios_[i] < 0 )
      temp = -ratios_[i];
    else
      temp = ratios_[i] * baseFrequency_;
    filters_[i]->setResonance( temp, radii_[i]*amplitude );
  }
}

} // stk namespace
