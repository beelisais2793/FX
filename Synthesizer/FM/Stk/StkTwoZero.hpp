#pragma once
#include "StkFilter.hpp"
#include <cmath>

namespace stk 
{

/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This class implements a two-zero digital filter.  A method is
    provided for creating a "notch" in the frequency response while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class TwoZero : public Filter<T>
{
 public:
  //! Default constructor creates a second-order pass-through filter.
  TwoZero();

  //! Class destructor.
  ~TwoZero();

  //! A function to enable/disable the automatic updating of class data when the STK sample rate changes.
  void ignoreSampleRateChange( bool ignore = true ) { this->ignoreSampleRateChange_ = ignore; };

  //! Set the b[0] coefficient value.
  void setB0( T b0 ) { this->b_[0] = b0; };

  //! Set the b[1] coefficient value.
  void setB1( T b1 ) { this->b_[1] = b1; };

  //! Set the b[2] coefficient value.
  void setB2( T b2 ) { this->b_[2] = b2; };

  //! Set all filter coefficients.
  void setCoefficients( T b0, T b1, T b2, bool clearState = false );

  //! Sets the filter coefficients for a "notch" at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate zeros with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  The coefficients are then
    normalized to produce a maximum filter gain of one (independent of
    the filter \e gain parameter).  The resulting filter frequency
    response has a "notch" or anti-resonance at the given \e
    frequency.  The closer the zeros are to the unit-circle (\e radius
    close to or equal to one), the narrower the resulting notch width.
    The \e frequency value should be between zero and half the sample
    rate.  The \e radius value should be positive.
  */
  void setNotch( T frequency, T radius );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Input one sample to the filter and return one output.
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

  void sampleRateChanged( T newRate, T oldRate );
};

template<typename T>
inline T TwoZero<T>::tick( T input )
{
  this->inputs_[0] = this->gain_ * input;
  this->lastFrame_[0] = this->b_[2] * this->inputs_[2] + this->b_[1] * this->inputs_[1] + this->b_[0] * this->inputs_[0];
  this->inputs_[2] = this->inputs_[1];
  this->inputs_[1] = this->inputs_[0];

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& TwoZero<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "TwoZero::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[0] = this->gain_ * *samples;
    *samples = this->b_[2] * this->inputs_[2] + this->b_[1] * this->inputs_[1] + this->b_[0] * this->inputs_[0];
    this->inputs_[2] = this->inputs_[1];
    this->inputs_[1] = this->inputs_[0];
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& TwoZero<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "TwoZero::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[0] = this->gain_ * *iSamples;
    *oSamples = this->b_[2] * this->inputs_[2] + this->b_[1] * this->inputs_[1] + this->b_[0] * this->inputs_[0];
    this->inputs_[2] = this->inputs_[1];
    this->inputs_[1] = this->inputs_[0];
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This class implements a two-zero digital filter.  A method is
    provided for creating a "notch" in the frequency response while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
TwoZero<T>::TwoZero( void )
{
  this->b_.resize( 3, 0.0 );
  this->inputs_.resize( 3, 1, 0.0 );
  this->b_[0] = 1.0;
  this->addSampleRateAlert( this );
}

template<typename T>
TwoZero<T>::~TwoZero()
{
  this->removeSampleRateAlert( this );
}

template<typename T>
void TwoZero<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( ! this->ignoreSampleRateChange_ ) {
    oStream_ << "TwoZero::sampleRateChanged: you may need to recompute filter coefficients!";
    handleError( StkError::WARNING );
  }
}

template<typename T>
void TwoZero<T>::setCoefficients( T b0, T b1, T b2, bool clearState )
{
  this->b_[0] = b0;
  this->b_[1] = b1;
  this->b_[2] = b2;

  if ( clearState ) this->clear();
}

template<typename T>
void TwoZero<T>::setNotch( T frequency, T radius )
{
#if defined(_STK_DEBUG_)
  if ( frequency < 0.0 || frequency > 0.5 * Stk::sampleRate() ) {
    oStream_ << "TwoZero::setNotch: frequency argument (" << frequency << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
  if ( radius < 0.0 ) {
    oStream_ << "TwoZero::setNotch: radius argument (" << radius << ") is negative!";
    handleError( StkError::WARNING ); return;
  }
#endif

  this->b_[2] = radius * radius;
  this->b_[1] = -2.0 * radius * cos(TWO_PI * frequency / sampleRate());

  // Normalize the filter gain.
  if ( this->b_[1] > 0.0 ) // Maximum at z = 0.
    this->b_[0] = 1.0 / ( 1.0 + this->b_[1] + this->b_[2] );
  else            // Maximum at z = -1.
    this->b_[0] = 1.0 / ( 1.0 - this->b_[1] + this->b_[2] );
  this->b_[1] *= this->b_[0];
  this->b_[2] *= this->b_[0];
}

} // stk namespace
