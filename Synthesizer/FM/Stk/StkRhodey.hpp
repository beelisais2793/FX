#pragma once
#include "StkFM.hpp"

namespace stk {

/***************************************************/
/*! \class Rhodey
    \brief STK Fender Rhodes electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
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
class Rhodey : public FM<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Rhodey( void );

  //! Class destructor.
  ~Rhodey( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

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

};

template<typename T>
inline T Rhodey<T>::tick( unsigned int )
{
  T temp, temp2;

  temp = this->gains_[1] * this->adsr_[1]->tick() * this->waves_[1]->tick();
  temp = temp * this->control1_;

  this->waves_[0]->addPhaseOffset( temp );
  this->waves_[3]->addPhaseOffset( this->twozero_.lastOut() );
  temp = this->gains_[3] * this->adsr_[3]->tick() * this->waves_[3]->tick();
  this->twozero_.tick(temp);

  this->waves_[2]->addPhaseOffset( temp );
  temp = ( 1.0 - (this->control2_ * 0.5)) * this->gains_[0] * this->adsr_[0]->tick() * this->waves_[0]->tick();
  temp += this->control2_ * 0.5 * this->gains_[2] * this->adsr_[2]->tick() * this->waves_[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = this->vibrato_.tick() * this->modDepth_;
  temp = temp * (1.0 + temp2);
    
  this->lastFrame_[0] = temp * 0.5;
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Rhodey<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Rhodey::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Rhodey
    \brief STK Fender Rhodes-like electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
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
Rhodey<T>::Rhodey( void )
  : FM<T>()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( unsigned int i=0; i<3; i++ )
    this->waves_[i] = new FileLoop<T>( (stk::rawwavePath() + "sinewave.raw").c_str(), true );
  this->waves_[3] = new FileLoop<T>( (stk::rawwavePath() + "fwavblnk.raw").c_str(), true );

  this->setRatio(0, 1.0);
  this->setRatio(1, 0.5);
  this->setRatio(2, 1.0);
  this->setRatio(3, 15.0);

  this->gains_[0] = this->fmGains_[99];
  this->gains_[1] = this->fmGains_[90];
  this->gains_[2] = this->fmGains_[99];
  this->gains_[3] = this->fmGains_[67];

  this->adsr_[0]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  this->adsr_[1]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  this->adsr_[2]->setAllTimes( 0.001, 1.00, 0.0, 0.04);
  this->adsr_[3]->setAllTimes( 0.001, 0.25, 0.0, 0.04);

  this->twozero_.setGain( 1.0 );
}

template<typename T>
Rhodey<T>::~Rhodey( void )
{
}

template<typename T>
void Rhodey<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Rhodey::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  this->baseFrequency_ = frequency * 2.0;

  for (unsigned int i=0; i<this->nOperators_; i++ )
    this->waves_[i]->setFrequency( this->baseFrequency_ * this->ratios_[i] );
}

template<typename T>
void Rhodey<T>::noteOn( T frequency, T amplitude )
{
  this->gains_[0] = amplitude * this->fmGains_[99];
  this->gains_[1] = amplitude * this->fmGains_[90];
  this->gains_[2] = amplitude * this->fmGains_[99];
  this->gains_[3] = amplitude * this->fmGains_[67];
  this->setFrequency( frequency );
  this->keyOn();
}

} // stk namespace

