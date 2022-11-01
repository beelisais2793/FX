#pragma once
#include "StkFilter.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This class implements a two-pole digital filter.  A method is
    provided for creating a resonance in the frequency response while
    maintaining a nearly constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class TwoPole : public Filter<T>
{
 public:

  //! Default constructor creates a second-order pass-through filter.
  TwoPole( void );

  //! Class destructor.
  ~TwoPole();

  //! A function to enable/disable the automatic updating of class data when the STK sample rate changes.
  void ignoreSampleRateChange( bool ignore = true ) { this->ignoreSampleRateChange_ = ignore; };

  //! Set the b[0] coefficient value.
  void setB0( T b0 ) { this->b_[0] = b0; };

  //! Set the a[1] coefficient value.
  void setA1( T a1 ) { this->a_[1] = a1; };

  //! Set the a[2] coefficient value.
  void setA2( T a2 ) { this->a_[2] = a2; };

  //! Set all filter coefficients.
  void setCoefficients( T b0, T a1, T a2, bool clearState = false );

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  If \e normalize is true,
    the coefficients are then normalized to produce unity gain at \e
    frequency (the actual maximum filter gain tends to be slightly
    greater than unity when \e radius is not close to one).  The
    resulting filter frequency response has a resonance at the given
    \e frequency.  The closer the poles are to the unit-circle (\e
    radius close to one), the narrower the resulting resonance width.
    An unstable filter will result for \e radius >= 1.0.  The
    \e frequency value should be between zero and half the sample rate.
    For a better resonance filter, use a BiQuad filter. \sa BiQuad
    filter class
  */
  void setResonance(T frequency, T radius, bool normalize = false);

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Input one sample to the filter and return one output.
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
};


template<typename T>
inline T TwoPole<T>::tick( T input )
{
  this->inputs_[0] = this->gain_ * input;
  this->lastFrame_[0] = this->b_[0] * this->inputs_[0] - this->a_[1] * this->outputs_[1] - this->a_[2] * this->outputs_[2];
  this->outputs_[2] = this->outputs_[1];
  this->outputs_[1] = this->lastFrame_[0];

  return this->lastFrame_[0];
}


template<typename T>
inline StkFrames<T>& TwoPole<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "TwoPole::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[0] = this->gain_ * *samples;
    *samples = this->b_[0] * this->inputs_[0] - this->a_[1] * this->outputs_[1] - this->a_[2] * this->outputs_[2];
    this->outputs_[2] = this->outputs_[1];
    this->outputs_[1] = *samples;
  }

  this->lastFrame_[0] = this->outputs_[1];
  return frames;
}


template<typename T>
inline StkFrames<T>& TwoPole<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "TwoPole::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[0] = this->gain_ * *iSamples;
    *oSamples = this->b_[0] * this->inputs_[0] - this->a_[1] * this->outputs_[1] - this->a_[2] * this->outputs_[2];
    this->outputs_[2] = this->outputs_[1];
    this->outputs_[1] = *oSamples;
  }

  this->lastFrame_[0] = this->outputs_[1];
  return iFrames;
}

/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This class implements a two-pole digital filter.  A method is
    provided for creating a resonance in the frequency response while
    maintaining a nearly constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
TwoPole<T>::TwoPole( void )
{
  this->b_.resize( 1 );
  this->a_.resize( 3 );
  this->inputs_.resize( 1, 1, 0.0 );
  this->outputs_.resize( 3, 1, 0.0 );
  this->b_[0] = 1.0;
  this->a_[0] = 1.0;

  this->addSampleRateAlert( this );
}


template<typename T>
TwoPole<T>::~TwoPole()
{
  this->removeSampleRateAlert( this );
}


template<typename T>
void TwoPole<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( !this->ignoreSampleRateChange_ ) {
    oStream_ << "TwoPole::sampleRateChanged: you may need to recompute filter coefficients!";
    handleError( StkError::WARNING );
  }
}


template<typename T>
void TwoPole<T>::setResonance( T frequency, T radius, bool normalize )
{
#if defined(_STK_DEBUG_)
  if ( frequency < 0.0 || frequency > 0.5 * stk::sampleRate() ) {
    oStream_ << "TwoPole::setResonance: frequency argument (" << frequency << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
  if ( radius < 0.0 || radius >= 1.0 ) {
    oStream_ << "TwoPole::setResonance: radius argument (" << radius << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  this->a_[2] = radius * radius;
  this->a_[1] = (T) -2.0 * radius * cos(TWO_PI * frequency / stk::sampleRate());

  if ( normalize ) {
    // Normalize the filter gain ... not terribly efficient.
    T real = 1 - radius + (this->a_[2] - radius) * cos(TWO_PI * 2 * frequency / stk::sampleRate());
    T imag = (this->a_[2] - radius) * sin(TWO_PI * 2 * frequency / stk::sampleRate());
    this->b_[0] = sqrt( pow(real, 2) + pow(imag, 2) );
  }
}


template<typename T>
void TwoPole<T>::setCoefficients( T b0, T a1, T a2, bool clearState )
{
  this->b_[0] = b0;
  this->a_[1] = a1;
  this->a_[2] = a2;

  if ( clearState ) this->clear();
}

} // stk namespace

