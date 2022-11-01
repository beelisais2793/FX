#pragma once
#include "StkFunction.hpp"
#include <cmath>

namespace stk 
{

/***************************************************/
/*! \class BowTable
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith
    (1986).  The output is an instantaneous
    reflection coefficient value.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class BowTable : public Function<T>
{
public:
  //! Default constructor.
  BowTable( void ) : offset_(0.0), slope_(0.1), minOutput_(0.01), maxOutput_(0.98) {};

  //! Set the table offset value.
  /*!
    The table offset is a bias which controls the
    symmetry of the friction.  If you want the
    friction to vary with direction, use a non-zero
    value for the offset.  The default value is zero.
  */
  void setOffset( T offset ) { offset_ = offset; };

  //! Set the table slope value.
  /*!
   The table slope controls the width of the friction
   pulse, which is related to bow force.
  */
  void setSlope( T slope ) { slope_ = slope; };

  //! Set the minimum table output value (0.0 - 1.0).
  void setMinOutput( T minimum ) { minOutput_ = minimum; };

  //! Set the maximum table output value (0.0 - 1.0).
  void setMaxOutput( T maximum ) { maxOutput_ = maximum; };

  //! Take one sample input and map to one sample of output.
  T tick( T input );

  //! Take a channel of the StkFrames object as inputs to the table and replace with corresponding outputs.
  /*!
    The StkFrames argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the table and write outputs to the \c oFrames object.
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

  T offset_;
  T slope_;
  T minOutput_;
  T maxOutput_;

};

template<typename T>
inline T BowTable<T>::tick( T input )
{
  // The input represents differential string vs. bow velocity.
  T sample  = input + offset_;  // add bias to input
  sample *= slope_;          // then scale it
  this->lastFrame_[0] = (T) fabs( (double) sample ) + (T) 0.75;
  this->lastFrame_[0] = (T) pow( this->lastFrame_[0], (T) -4.0 );

  // Set minimum threshold
  if ( this->lastFrame_[0] < minOutput_ ) this->lastFrame_[0] = minOutput_;

  // Set maximum threshold
  if ( this->lastFrame_[0] > maxOutput_ ) this->lastFrame_[0] = maxOutput_;

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& BowTable<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "BowTable::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples = *samples + offset_;
    *samples *= slope_;
    *samples = (T) fabs( (double) *samples ) + 0.75;
    *samples = (T) pow( *samples, (T) -4.0 );
    if ( *samples > 1.0) *samples = 1.0;
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& BowTable<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "BowTable::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    *oSamples = *iSamples + offset_;
    *oSamples *= slope_;
    *oSamples = (T) fabs( (double) *oSamples ) + 0.75;
    *oSamples = (T) pow( *oSamples, (T) -4.0 );
    if ( *oSamples > 1.0) *oSamples = 1.0;
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

} // stk namespace
