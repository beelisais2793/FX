#pragma once
#include "StkFilter.hpp"

namespace stk {

/***************************************************/
/*! \class OnePole
    \brief STK one-pole filter class.

    This class implements a one-pole digital filter.  A method is
    provided for setting the pole position along the real axis of the
    z-plane while maintaining a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class OnePole : public Filter<T>
{
public:

  //! The default constructor creates a low-pass filter (pole at z = 0.9).
  OnePole( T thePole = 0.9 );

  //! Class destructor.
  ~OnePole();

  //! Set the b[0] coefficient value.
  void setB0( T b0 ) { this->b_[0] = b0; };

  //! Set the a[1] coefficient value.
  void setA1( T a1 ) { this->a_[1] = a1; };

  //! Set all filter coefficients.
  void setCoefficients( T b0, T a1, bool clearState = false );

  //! Set the pole position in the z-plane.
  /*!
    This method sets the pole position along the real-axis of the
    z-plane and normalizes the coefficients for a maximum gain of one.
    A positive pole value produces a low-pass filter, while a negative
    pole value produces a high-pass filter.  This method does not
    affect the filter \e gain value. The argument magnitude should be
    less than one to maintain filter stability.
  */
  void setPole( T thePole );

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

};

template<typename T>
inline T OnePole<T>::tick( T input )
{
  this->inputs_[0] = this->gain_ * input;
  this->lastFrame_[0] = this->b_[0] * this->inputs_[0] - this->a_[1] * this->outputs_[1];
  this->outputs_[1] = this->lastFrame_[0];

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& OnePole<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "OnePole::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[0] = this->gain_ * *samples;
    *samples = this->b_[0] * this->inputs_[0] - this->a_[1] * this->outputs_[1];
    this->outputs_[1] = *samples;
  }

  this->lastFrame_[0] = this->outputs_[1];
  return frames;
}

template<typename T>
inline StkFrames<T>& OnePole<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "OnePole::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[0] = this->gain_ * *iSamples;
    *oSamples = this->b_[0] * this->inputs_[0] - this->a_[1] * this->outputs_[1];
    this->outputs_[1] = *oSamples;
  }

  this->lastFrame_[0] = this->outputs_[1];
  return iFrames;
}

/***************************************************/
/*! \class OnePole
    \brief STK one-pole filter class.

    This class implements a one-pole digital filter.  A method is
    provided for setting the pole position along the real axis of the
    z-plane while maintaining a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
OnePole<T>::OnePole( T thePole )
{
  this->b_.resize( 1 );
  this->a_.resize( 2 );
  this->a_[0] = 1.0;
  this->inputs_.resize( 1, 1, 0.0 );
  this->outputs_.resize( 2, 1, 0.0 );

  this->setPole( thePole );
}

template<typename T>
OnePole<T>::~OnePole()    
{
}

template<typename T>
void OnePole<T>::setPole( T thePole )
{
  if ( std::abs( thePole ) >= 1.0 ) {
    oStream_ << "OnePole::setPole: argument (" << thePole << ") should be less than 1.0!";
    handleError( StkError::WARNING ); return;
  }

  // Normalize coefficients for peak unity gain.
  if ( thePole > 0.0 )
    this->b_[0] = (T) (1.0 - thePole);
  else
    this->b_[0] = (T) (1.0 + thePole);

  this->a_[1] = -thePole;
}

template<typename T>
void OnePole<T>::setCoefficients( T b0, T a1, bool clearState )
{
  if ( std::abs( a1 ) >= 1.0 ) {
    oStream_ << "OnePole::setCoefficients: a1 argument (" << a1 << ") should be less than 1.0!";
    handleError( StkError::WARNING ); return;
  }

  this->b_[0] = b0;
  this->a_[1] = a1;

  if ( clearState ) this->clear();
}

} // stk namespace
