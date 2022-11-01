#pragma once
#include "StkFilter.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class OneZero
  \brief STK one-zero filter class.

  This class implements a one-zero digital filter.  A method is
  provided for setting the zero position along the real axis of the
  z-plane while maintaining a constant filter gain.

  by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class OneZero : public Filter<T>
{
 public:

  //! The default constructor creates a low-pass filter (zero at z = -1.0).
  OneZero( T theZero = -1.0 );

  //! Class destructor.
  ~OneZero();

  //! Set the b[0] coefficient value.
  void setB0( T b0 ) { this->b_[0] = b0; };

  //! Set the b[1] coefficient value.
  void setB1( T b1 ) { this->b_[1] = b1; };

  //! Set all filter coefficients.
  void setCoefficients( T b0, T b1, bool clearState = false );

  //! Set the zero position in the z-plane.
  /*!
    This method sets the zero position along the real-axis of the
    z-plane and normalizes the coefficients for a maximum gain of one.
    A positive zero value produces a high-pass filter, while a
    negative zero value produces a low-pass filter.  This method does
    not affect the filter \e gain value.
  */
  void setZero( T theZero );

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
inline T OneZero<T>::tick( T input )
{
  this->inputs_[0] = this->gain_ * input;
  this->lastFrame_[0] = this->b_[1] * this->inputs_[1] + this->b_[0] * this->inputs_[0];
  this->inputs_[1] = this->inputs_[0];

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& OneZero<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    this->oStream_ << "OneZero::tick(): channel and StkFrames<T> arguments are incompatible!";
    this->handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[0] = this->gain_ * *samples;
    *samples = this->b_[1] * this->inputs_[1] + this->b_[0] * this->inputs_[0];
    this->inputs_[1] = this->inputs_[0];
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& OneZero<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    this->oStream_ << "OneZero::tick(): channel and StkFrames<T> arguments are incompatible!";
    this->handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    this->inputs_[0] = this->gain_ * *iSamples;
    *oSamples = this->b_[1] * this->inputs_[1] + this->b_[0] * this->inputs_[0];
    this->inputs_[1] = this->inputs_[0];
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

/***************************************************/
/*! \class OneZero
    \brief STK one-zero filter class.

    This class implements a one-zero digital filter.  A method is
    provided for setting the zero position along the real axis of the
    z-plane while maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



template<typename T>
OneZero<T>::OneZero( T theZero )
{
  this->b_.resize( 2 );
  this->inputs_.resize( 2, 1, 0.0 );

  this->setZero( theZero );
}

template<typename T>
OneZero<T>::~OneZero( void )
{
}

template<typename T>
void OneZero<T>::setZero( T theZero )
{
  // Normalize coefficients for unity gain.
  if ( theZero > 0.0 )
    this->b_[0] = 1.0 / ((T) 1.0 + theZero);
  else
    this->b_[0] = 1.0 / ((T) 1.0 - theZero);

  this->b_[1] = -theZero * this->b_[0];
}

template<typename T>
void OneZero<T>::setCoefficients( T b0, T b1, bool clearState )
{
  this->b_[0] = b0;
  this->b_[1] = b1;

  if ( clearState ) this->clear();
}

} // stk namespace


