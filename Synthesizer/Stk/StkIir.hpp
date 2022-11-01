#pragma once
#include "StkFilter.hpp"

namespace stk {

/***************************************************/
/*! \class Iir
    \brief STK general infinite impulse response filter class.

    This class provides a generic digital filter structure that can be
    used to implement IIR filters.  For filters containing only
    feedforward terms, the Fir class is slightly more efficient.

    In particular, this class implements the standard difference
    equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coeffcients are normalized
    by a[0].

    The \e gain parameter is applied at the filter input and does not
    affect the coefficient values.  The default gain value is 1.0.
    This structure results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Iir : public Filter<T>
{
public:
  //! Default constructor creates a zero-order pass-through "filter".
  Iir( void );

  //! Overloaded constructor which takes filter coefficients.
  /*!
    An StkError can be thrown if either of the coefficient vector
    sizes is zero, or if the a[0] coefficient is equal to zero.
  */
  Iir( std::vector<T> &bCoefficients, std::vector<T> &aCoefficients );

  //! Class destructor.
  ~Iir( void );

  //! Set filter coefficients.
  /*!
    An StkError can be thrown if either of the coefficient vector
    sizes is zero, or if the a[0] coefficient is equal to zero.  If
    a[0] is not equal to 1, the filter coeffcients are normalized by
    a[0].  The internal state of the filter is not cleared unless the
    \e clearState flag is \c true.
  */
  void setCoefficients( std::vector<T> &bCoefficients, std::vector<T> &aCoefficients, bool clearState = false );

  //! Set numerator coefficients.
  /*!
    An StkError can be thrown if coefficient vector is empty.  Any
    previously set denominator coefficients are left unaffected.  Note
    that the default constructor sets the single denominator
    coefficient a[0] to 1.0.  The internal state of the filter is not
    cleared unless the \e clearState flag is \c true.
  */
  void setNumerator( std::vector<T> &bCoefficients, bool clearState = false );

  //! Set denominator coefficients.
  /*!
    An StkError can be thrown if the coefficient vector is empty or
    if the a[0] coefficient is equal to zero.  Previously set
    numerator coefficients are unaffected unless a[0] is not equal to
    1, in which case all coeffcients are normalized by a[0].  Note
    that the default constructor sets the single numerator coefficient
    b[0] to 1.0.  The internal state of the filter is not cleared
    unless the \e clearState flag is \c true.
  */
  void setDenominator( std::vector<T> &aCoefficients, bool clearState = false );

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

};

template<typename T>
inline T Iir<T>::tick( T input )
{
  size_t i;

  this->outputs_[0] = 0.0;
  this->inputs_[0] = this->gain_ * input;
  for ( i=this->b_.size()-1; i>0; i-- ) {
    this->outputs_[0] += this->b_[i] * this->inputs_[i];
    this->inputs_[i] = this->inputs_[i-1];
  }
  this->outputs_[0] += this->b_[0] * this->inputs_[0];

  for ( i=this->a_.size()-1; i>0; i-- ) {
    this->outputs_[0] += -this->a_[i] * this->outputs_[i];
    this->outputs_[i] = this->outputs_[i-1];
  }

  this->lastFrame_[0] = this->outputs_[0];
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Iir<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Iir::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  size_t i;
  unsigned int hop = frames.channels();
  for ( unsigned int j=0; j<frames.frames(); j++, samples += hop ) {
    this->outputs_[0] = 0.0;
    this->inputs_[0] = this->gain_ * *samples;
    for ( i=this->b_.size()-1; i>0; i-- ) {
      this->outputs_[0] += this->b_[i] * this->inputs_[i];
      this->inputs_[i] = this->inputs_[i-1];
    }
    this->outputs_[0] += this->b_[0] * this->inputs_[0];

    for ( i=this->a_.size()-1; i>0; i-- ) {
      this->outputs_[0] += -this->a_[i] * this->outputs_[i];
      this->outputs_[i] = this->outputs_[i-1];
    }

    *samples = this->outputs_[0];
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& Iir<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "Iir::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  size_t i;
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int j=0; j<iFrames.frames(); j++, iSamples += iHop, oSamples += oHop ) {
    this->outputs_[0] = 0.0;
    this->inputs_[0] = this->gain_ * *iSamples;
    for ( i=this->b_.size()-1; i>0; i-- ) {
      this->outputs_[0] += this->b_[i] * this->inputs_[i];
      this->inputs_[i] = this->inputs_[i-1];
    }
    this->outputs_[0] += this->b_[0] * this->inputs_[0];

    for ( i=this->a_.size()-1; i>0; i-- ) {
      this->outputs_[0] += -this->a_[i] * this->outputs_[i];
      this->outputs_[i] = this->outputs_[i-1];
    }

    *oSamples = this->outputs_[0];
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

/***************************************************/
/*! \class Iir
    \brief STK general infinite impulse response filter class.

    This class provides a generic digital filter structure that can be
    used to implement IIR filters.  For filters containing only
    feedforward terms, the Fir class is slightly more efficient.

    In particular, this class implements the standard difference
    equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coeffcients are normalized
    by a[0].

    The \e gain parameter is applied at the filter input and does not
    affect the coefficient values.  The default gain value is 1.0.
    This structure results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Iir<T>::Iir()
{
  // The default constructor should setup for pass-through.
  this->b_.push_back( 1.0 );
  this->a_.push_back( 1.0 );

  this->inputs_.resize( 1, 1, 0.0 );
  this->outputs_.resize( 1, 1, 0.0 );
}

template<typename T>
Iir<T>::Iir( std::vector<T> &bCoefficients, std::vector<T> &aCoefficients )
{
  // Check the arguments.
  if ( bCoefficients.size() == 0 || aCoefficients.size() == 0 ) {
    oStream_ << "Iir: a and b coefficient vectors must both have size > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    oStream_ << "Iir: a[0] coefficient cannot == 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  this->gain_ = 1.0;
  this->b_ = bCoefficients;
  this->a_ = aCoefficients;

  this->inputs_.resize( this->b_.size(), 1, 0.0 );
  this->outputs_.resize( this->a_.size(), 1, 0.0 );
  this->clear();
}

template<typename T>
Iir<T>::~Iir()
{
}

template<typename T>
void Iir<T>::setCoefficients( std::vector<T> &bCoefficients, std::vector<T> &aCoefficients, bool clearState )
{
  this->setNumerator( bCoefficients, false );
  this->setDenominator( aCoefficients, false );

  if ( clearState ) this->clear();
}

template<typename T>
void Iir<T>::setNumerator( std::vector<T> &bCoefficients, bool clearState )
{
  // Check the argument.
  if ( bCoefficients.size() == 0 ) {
    oStream_ << "Iir::setNumerator: coefficient vector must have size > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( this->b_.size() != bCoefficients.size() ) {
    this->b_ = bCoefficients;
    this->inputs_.resize( this->b_.size(), 1, 0.0 );
  }
  else {
    for ( unsigned int i=0; i<this->b_.size(); i++ ) this->b_[i] = bCoefficients[i];
  }

  if ( clearState ) this->clear();
}

template<typename T>
void Iir<T>::setDenominator( std::vector<T> &aCoefficients, bool clearState )
{
  // Check the argument.
  if ( aCoefficients.size() == 0 ) {
    oStream_ << "Iir::setDenominator: coefficient vector must have size > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    oStream_ << "Iir::setDenominator: a[0] coefficient cannot == 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( this->a_.size() != aCoefficients.size() ) {
    this->a_ = aCoefficients;
    this->outputs_.resize( this->a_.size(), 1, 0.0 );
  }
  else {
    for ( unsigned int i=0; i<this->a_.size(); i++ ) this->a_[i] = aCoefficients[i];
  }

  if ( clearState ) this->clear();

  // Scale coefficients by a[0] if necessary
  if ( this->a_[0] != 1.0 ) {
    unsigned int i;
    for ( i=0; i<this->b_.size(); i++ ) this->b_[i] /= this->a_[0];
    for ( i=1; i<this->a_.size(); i++ )  this->a_[i] /= this->a_[0];
  }
}

} // stk namespace

