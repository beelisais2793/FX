#pragma once
#include "StkFilter.hpp"
#include <cmath>


namespace stk {

/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This class implements a formant (resonance) which can be "swept"
    over time from one frequency setting to another.  It provides
    methods for controlling the sweep rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class FormSwep : public Filter<T>
{
 public:

  //! Default constructor creates a second-order pass-through filter.
  FormSwep( void );

  //! Class destructor.
  ~FormSwep();

  //! A function to enable/disable the automatic updating of class data when the STK sample rate changes.
  void ignoreSampleRateChange( bool ignore = true ) { this->ignoreSampleRateChange_ = ignore; };

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  The filter zeros are
    placed at z = 1, z = -1, and the coefficients are then normalized
    to produce a constant unity gain (independent of the filter \e
    gain parameter).  The resulting filter frequency response has a
    resonance at the given \e frequency.  The closer the poles are to
    the unit-circle (\e radius close to one), the narrower the
    resulting resonance width.  An unstable filter will result for \e
    radius >= 1.0.  The \e frequency value should be between zero and
    half the sample rate.
  */
  void setResonance( T frequency, T radius );

  //! Set both the current and target resonance parameters.
  void setStates( T frequency, T radius, T gain = 1.0 );

  //! Set target resonance parameters.
  void setTargets( T frequency, T radius, T gain = 1.0 );

  //! Set the sweep rate (between 0.0 - 1.0).
  /*!
    The formant parameters are varied in increments of the
    sweep rate between their current and target values.
    A sweep rate of 1.0 will produce an immediate change in
    resonance parameters from their current values to the
    target values.  A sweep rate of 0.0 will produce no
    change in resonance parameters.  
  */
  void setSweepRate( T rate );

  //! Set the sweep rate in terms of a time value in seconds.
  /*!
    This method adjusts the sweep rate based on a
    given time for the formant parameters to reach
    their target values.
  */
  void setSweepTime( T time );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Input one sample to the filter and return a reference to one output.
  T tick( T input );

  //! Take a channel of the StkFrames<T> object as inputs to the filter and replace with corresponding outputs.
  /*!
    The StkFrames<T> argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames<T> argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the filter and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

 protected:

  virtual void sampleRateChanged( T newRate, T oldRate );

  bool dirty_;
  T frequency_;
  T radius_;
  T startFrequency_;
  T startRadius_;
  T startGain_;
  T targetFrequency_;
  T targetRadius_;
  T targetGain_;
  T deltaFrequency_;
  T deltaRadius_;
  T deltaGain_;
  T sweepState_;
  T sweepRate_;

};

template<typename T>
inline T FormSwep<T>::tick( T input )
{                                     
  if ( dirty_ )  {
    sweepState_ += sweepRate_;
    if ( sweepState_ >= 1.0 )   {
      sweepState_ = 1.0;
      dirty_ = false;
      radius_ = targetRadius_;
      frequency_ = targetFrequency_;
      this->gain_ = targetGain_;
    }
    else {
      radius_ = startRadius_ + (deltaRadius_ * sweepState_);
      frequency_ = startFrequency_ + (deltaFrequency_ * sweepState_);
      this->gain_ = startGain_ + (deltaGain_ * sweepState_);
    }
    this->setResonance( frequency_, radius_ );
  }

  this->inputs_[0] = this->gain_ * input;
  this->lastFrame_[0] = this->b_[0] * this->inputs_[0] + this->b_[1] * this->inputs_[1] + this->b_[2] * this->inputs_[2];
  this->lastFrame_[0] -= this->a_[2] * this->outputs_[2] + this->a_[1] * this->outputs_[1];
  this->inputs_[2] = this->inputs_[1];
  this->inputs_[1] = this->inputs_[0];
  this->outputs_[2] = this->outputs_[1];
  this->outputs_[1] = this->lastFrame_[0];

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& FormSwep<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "FormSwep::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = tick( *samples );

  return frames;
}

template<typename T>
inline StkFrames<T>& FormSwep<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "FormSwep::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop )
    *oSamples = tick( *iSamples );

  return iFrames;
}

/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This class implements a formant (resonance) which can be "swept"
    over time from one frequency setting to another.  It provides
    methods for controlling the sweep rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
FormSwep<T>::FormSwep( void )
{
  frequency_ = 0.0;
  radius_ = 0.0;
  targetGain_ = 1.0;
  targetFrequency_ = 0.0;
  targetRadius_ = 0.0;
  deltaGain_ = 0.0;
  deltaFrequency_ = 0.0;
  deltaRadius_ = 0.0;
  sweepState_ = 0.0;
  sweepRate_ = 0.002;
  dirty_ = false;

  this->b_.resize( 3, 0.0 );
  this->a_.resize( 3, 0.0 );
  this->a_[0] = 1.0;
  this->inputs_.resize( 3, 1, 0.0 );
  this->outputs_.resize( 3, 1, 0.0 );

  this->addSampleRateAlert( this );
}

template<typename T>
FormSwep<T>::~FormSwep()
{
  this->removeSampleRateAlert( this );
}

template<typename T>
void FormSwep<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( !this->ignoreSampleRateChange_ ) {
    oStream_ << "FormSwep::sampleRateChanged: you may need to recompute filter coefficients!";
    handleError( StkError::WARNING );
  }
}

template<typename T>
void FormSwep<T>::setResonance( T frequency, T radius )
{
#if defined(_STK_DEBUG_)
  if ( frequency < 0.0 || frequency > 0.5 * sampleRate() ) {
    oStream_ << "FormSwep::setResonance: frequency argument (" << frequency << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
  if ( radius < 0.0 || radius >= 1.0 ) {
    oStream_ << "FormSwep::setResonance: radius argument (" << radius << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  radius_ = radius;
  frequency_ = frequency;

  this->a_[2] = radius * radius;
  this->a_[1] = -2.0 * radius * cos( TWO_PI * frequency / sampleRate() );

  // Use zeros at +- 1 and normalize the filter peak gain.
  this->b_[0] = 0.5 - 0.5 * this->a_[2];
  this->b_[1] = 0.0;
  this->b_[2] = -this->b_[0];
}

template<typename T>
void FormSwep<T>::setStates( T frequency, T radius, T gain )
{
  dirty_ = false;

  if ( frequency_ != frequency || radius_ != radius )
    this->setResonance( frequency, radius );

  this->gain_ = gain;
  targetFrequency_ = frequency;
  targetRadius_ = radius;
  targetGain_ = gain;
}

template<typename T>
void FormSwep<T>::setTargets( T frequency, T radius, T gain )
{
  if ( frequency < 0.0 || frequency > 0.5 * sampleRate() ) {
    oStream_ << "FormSwep::setTargets: frequency argument (" << frequency << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
  if ( radius < 0.0 || radius >= 1.0 ) {
    oStream_ << "FormSwep::setTargets: radius argument (" << radius << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }

  dirty_ = true;
  startFrequency_ = frequency_;
  startRadius_ = radius_;
  startGain_ = this->gain_;
  targetFrequency_ = frequency;
  targetRadius_ = radius;
  targetGain_ = gain;
  deltaFrequency_ = frequency - frequency_;
  deltaRadius_ = radius - radius_;
  deltaGain_ = gain - this->gain_;
  sweepState_ = 0.0;
}

template<typename T>
void FormSwep<T>::setSweepRate( T rate )
{
  if ( rate < 0.0 || rate > 1.0 ) {
    oStream_ << "FormSwep::setSweepRate: argument (" << rate << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }

  sweepRate_ = rate;
}

template<typename T>
void FormSwep<T>::setSweepTime( T time )
{
  if ( time <= 0.0 ) {
    oStream_ << "FormSwep::setSweepTime: argument (" << time << ") must be > 0.0!";
    handleError( StkError::WARNING ); return;
  }

  this->setSweepRate( 1.0 / ( time * sampleRate() ) );
}

} // stk namespace

