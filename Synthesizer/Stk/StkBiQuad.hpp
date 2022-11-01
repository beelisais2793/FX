#pragma once

#include "StkFilter.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class BiQuad
    \brief STK BiQuad (two-pole, two-zero) filter class.

    This class implements a two-pole, two-zero digital filter.
    Methods are provided for creating a resonance or notch in the
    frequency response while maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class BiQuad : public Filter<T>
{
public:

  //! Default constructor creates a second-order pass-through filter.
  BiQuad();

  //! Class destructor.
  ~BiQuad();

  //! A function to enable/disable the automatic updating of class data when the STK sample rate changes.
  void ignoreSampleRateChange( bool ignore = true ) { this->ignoreSampleRateChange_ = ignore; };

  //! Set all filter coefficients.
  void setCoefficients( T b0, T b1, T b2, T a1, T a2, bool clearState = false );

  //! Set the b[0] coefficient value.
  void setB0( T b0 ) { this->b_[0] = b0; };

  //! Set the b[1] coefficient value.
  void setB1( T b1 ) { this->b_[1] = b1; };

  //! Set the b[2] coefficient value.
  void setB2( T b2 ) { this->b_[2] = b2; };

  //! Set the a[1] coefficient value.
  void setA1( T a1 ) { this->a_[1] = a1; };

  //! Set the a[2] coefficient value.
  void setA2( T a2 ) { this->a_[2] = a2; };

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  If \e normalize is true,
    the filter zeros are placed at z = 1, z = -1, and the coefficients
    are then normalized to produce a constant unity peak gain
    (independent of the filter \e gain parameter).  The resulting
    filter frequency response has a resonance at the given \e
    frequency.  The closer the poles are to the unit-circle (\e radius
    close to one), the narrower the resulting resonance width.
    An unstable filter will result for \e radius >= 1.0.  The
    \e frequency value should be between zero and half the sample rate.
  */
  void setResonance( T frequency, T radius, bool normalize = false );

  //! Set the filter coefficients for a notch at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate zeros with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  No filter normalization is
    attempted.  The \e frequency value should be between zero and half
    the sample rate.  The \e radius value should be positive.
  */
  void setNotch( T frequency, T radius );

  //! Sets the filter zeroes for equal resonance gain.
  /*!
    When using the filter as a resonator, zeroes places at z = 1, z
    = -1 will result in a constant gain at resonance of 1 / (1 - R),
    where R is the pole radius setting.

  */
  void setEqualGainZeroes( void );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Input one sample to the filter and return a reference to one output.
  T tick( T input );

  //! Take a channel of the StkFrames object as inputs to the filter and replace with corresponding outputs.
  /*!
    The StkFrames argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the filter and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

 protected:

  virtual void sampleRateChanged( T newRate, T oldRate );
};

template<typename T>
inline T BiQuad<T>::tick( T input )
{
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
inline StkFrames<T>& BiQuad<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "BiQuad::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[0] = this->gain_ * *samples;
    *samples = this->b_[0] * this->inputs_[0] + this->b_[1] * this->inputs_[1] + this->b_[2] * this->inputs_[2];
    *samples -= this->a_[2] * this->outputs_[2] + this->a_[1] * this->outputs_[1];
    this->inputs_[2] = this->inputs_[1];
    this->inputs_[1] = this->inputs_[0];
    this->outputs_[2] = this->outputs_[1];
    this->outputs_[1] = *samples;
  }

  this->lastFrame_[0] = this->outputs_[1];
  return frames;
}

template<typename T>
inline StkFrames<T>& BiQuad<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "BiQuad::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[0] = this->gain_ * *iSamples;
    *oSamples = this->b_[0] * this->inputs_[0] + this->b_[1] * this->inputs_[1] + this->b_[2] * this->inputs_[2];
    *oSamples -= this->a_[2] * this->outputs_[2] + this->a_[1] * this->outputs_[1];
    this->inputs_[2] = this->inputs_[1];
    this->inputs_[1] = this->inputs_[0];
    this->outputs_[2] = this->outputs_[1];
    this->outputs_[1] = *oSamples;
  }

  this->lastFrame_[0] = this->outputs_[1];
  return iFrames;
}

/***************************************************/
/*! \class BiQuad
    \brief STK BiQuad (two-pole, two-zero) filter class.

    This class implements a two-pole, two-zero digital filter.
    Methods are provided for creating a resonance or notch in the
    frequency response while maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/




template<typename T>
BiQuad<T>::BiQuad() : Filter<T>()
{
  this->b_.resize( 3, 0.0 );
  this->a_.resize( 3, 0.0 );
  this->b_[0] = 1.0;
  this->a_[0] = 1.0;
  this->inputs_.resize( 3, 1, 0.0 );
  this->outputs_.resize( 3, 1, 0.0 );

  this->addSampleRateAlert( this );
}

template<typename T>
BiQuad<T>::~BiQuad()
{
  this->removeSampleRateAlert( this );
}

template<typename T>
void BiQuad<T>::setCoefficients( T b0, T b1, T b2, T a1, T a2, bool clearState )
{
  this->b_[0] = b0;
  this->b_[1] = b1;
  this->b_[2] = b2;
  this->a_[1] = a1;
  this->a_[2] = a2;

  if ( clearState ) this->clear();
}

template<typename T>
void BiQuad<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( ! this-> ignoreSampleRateChange_ ) {
    oStream_ << "BiQuad::sampleRateChanged: you may need to recompute filter coefficients!";
    handleError( StkError::WARNING );
  }
}

template<typename T>
void BiQuad<T>::setResonance( T frequency, T radius, bool normalize )
{
#if defined(_STK_DEBUG_)
  if ( frequency < 0.0 || frequency > 0.5 * Stk::sampleRate() ) {
    oStream_ << "BiQuad::setResonance: frequency argument (" << frequency << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
  if ( radius < 0.0 || radius >= 1.0 ) {
    oStream_ << "BiQuad::setResonance: radius argument (" << radius << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  this->a_[2] = radius * radius;
  this->a_[1] = -2.0 * radius * cos( TWO_PI * frequency / sampleRate() );

  if ( normalize ) {
    // Use zeros at +- 1 and normalize the filter peak gain.
    this->b_[0] = 0.5 - 0.5 * this->a_[2];
    this->b_[1] = 0.0;
    this->b_[2] = - this->b_[0];
  }
}

template<typename T>
void BiQuad<T>::setNotch( T frequency, T radius )
{
#if defined(_STK_DEBUG_)
  if ( frequency < 0.0 || frequency > 0.5 * Stk::sampleRate() ) {
    oStream_ << "BiQuad::setNotch: frequency argument (" << frequency << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
  if ( radius < 0.0 ) {
    oStream_ << "BiQuad::setNotch: radius argument (" << radius << ") is negative!";
    handleError( StkError::WARNING ); return;
  }
#endif

  // This method does not attempt to normalize the filter gain.
  this->b_[2] = radius * radius;
  this->b_[1] = (T) -2.0 * radius * cos( TWO_PI * (double) frequency / sampleRate() );
}

template<typename T>
void BiQuad<T>::setEqualGainZeroes( void )
{
  this->b_[0] = 1.0;
  this->b_[1] = 0.0;
  this->b_[2] = -1.0;
}

} // stk namespace

