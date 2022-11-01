#pragma once
#include "StkFilter.hpp"

namespace stk {

/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This class implements a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass filter with a given
    coefficient.  Another method is provided to create a DC blocking
    filter.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class PoleZero : public Filter<T>
{
 public:

  //! Default constructor creates a first-order pass-through filter.
  PoleZero();

  //! Class destructor.
  ~PoleZero();

  //! Set the b[0] coefficient value.
  void setB0( T b0 ) { this->b_[0] = b0; };

  //! Set the b[1] coefficient value.
  void setB1( T b1 ) { this->b_[1] = b1; };

  //! Set the a[1] coefficient value.
  void setA1( T a1 ) { this->a_[1] = a1; };

  //! Set all filter coefficients.
  void setCoefficients( T b0, T b1, T a1, bool clearState = false );

  //! Set the filter for allpass behavior using \e coefficient.
  /*!
    This method uses \e coefficient to create an allpass filter,
    which has unity gain at all frequencies.  Note that the
    \e coefficient magnitude must be less than one to maintain
    filter stability.
  */
  void setAllpass( T coefficient );

  //! Create a DC blocking filter with the given pole position in the z-plane.
  /*!
    This method sets the given pole position, together with a zero
    at z=1, to create a DC blocking filter.  The argument magnitude
    should be close to (but less than) one to minimize low-frequency
    attenuation.
  */
  void setBlockZero( T thePole = 0.99 );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Input one sample to the filter and return one output.
  T tick( T input );

  //! Take a channel of the StkFrames<T> object as inputs to the filter and replace with corresponding outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

};

template<typename T>
inline T PoleZero<T>::tick( T input )
{
  this->inputs_[0] = this->gain_ * input;
  this->lastFrame_[0] = this->b_[0] * this->inputs_[0] + this->b_[1] * this->inputs_[1] - this->a_[1] * this->outputs_[1];
  this->inputs_[1] = this->inputs_[0];
  this->outputs_[1] = this->lastFrame_[0];

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& PoleZero<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "PoleZero::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    this->inputs_[0] = this->gain_ * *samples;
    *samples = this->b_[0] * this->inputs_[0] + this->b_[1] * this->inputs_[1] - this->a_[1] * this->outputs_[1];
    this->inputs_[1] = this->inputs_[0];
    this->outputs_[1] = *samples;
  }

  this->lastFrame_[0] = this->outputs_[1];
  return frames;
}

/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This class implements a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass filter with a given
    coefficient.  Another method is provided to create a DC blocking
    filter.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
PoleZero<T>::PoleZero()
{
  // Default setting for pass-through.
  this->b_.resize( 2, 0.0 );
  this->a_.resize( 2, 0.0 );
  this->b_[0] = 1.0;
  this->a_[0] = 1.0;
  this->inputs_.resize( 2, 1, 0.0 );
  this->outputs_.resize( 2, 1, 0.0 );
}

template<typename T>
PoleZero<T>::~PoleZero()
{
}

template<typename T>
void PoleZero<T>::setCoefficients( T b0, T b1, T a1, bool clearState )
{
  if ( std::abs( a1 ) >= 1.0 ) {
    oStream_ << "PoleZero::setCoefficients: a1 argument (" << a1 << ") should be less than 1.0!";
    handleError( StkError::WARNING ); return;
  }

  this->b_[0] = b0;
  this->b_[1] = b1;
  this->a_[1] = a1;

  if ( clearState ) this->clear();
}

template<typename T>
void PoleZero<T>::setAllpass( T coefficient )
{
  if ( std::abs( coefficient ) >= 1.0 ) {
    oStream_ << "PoleZero::setAllpass: argument (" << coefficient << ") makes filter unstable!";
    handleError( StkError::WARNING ); return;
  }

  this->b_[0] = coefficient;
  this->b_[1] = 1.0;
  this->a_[0] = 1.0; // just in case
  this->a_[1] = coefficient;
}

template<typename T>
void PoleZero<T>::setBlockZero( T thePole )
{
  if ( std::abs( thePole ) >= 1.0 ) {
    oStream_ << "PoleZero::setBlockZero: argument (" << thePole << ") makes filter unstable!";
    handleError( StkError::WARNING ); return;
  }

  this->b_[0] = 1.0;
  this->b_[1] = -1.0;
  this->a_[0] = 1.0; // just in case
  this->a_[1] = -thePole;
}

} // stk namespace
