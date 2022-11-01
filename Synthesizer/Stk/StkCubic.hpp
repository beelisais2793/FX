#pragma once
#include "StkFunction.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Cubic
    \brief STK cubic non-linearity class.

    This class implements the cubic non-linearity
    that was used in SynthBuilder.

    The formula implemented is:
 
    \code
    output = gain * (a1 * input + a2 * input^2 + a3 * input^3)
    \endcode

    followed by a limiter for values outside +-threshold.

    Ported to STK by Nick Porcaro, 2007. Updated for inclusion
    in STK distribution by Gary Scavone, 2011.
*/
/***************************************************/

template<typename T>
class Cubic : public Function<T>
{
public:
  //! Default constructor.
  Cubic( void ) : a1_(0.5), a2_(0.5), a3_(0.5), gain_(1.0), threshold_(1.0) {};

  //! Set the a1 coefficient value.
  void setA1( T a1 ) { a1_ = a1; };

  //! Set the a2 coefficient value.
  void setA2( T a2 )  { a2_ = a2; };

  //! Set the a3 coefficient value.
  void setA3( T a3 )  { a3_ = a3; };

  //! Set the gain value.
  void setGain( T gain ) { gain_ = gain; };

  //! Set the threshold value.
  void setThreshold( T threshold ) { threshold_ = threshold; };

  //! Input one sample to the function and return one output.
  T tick( T input );

  //! Take a channel of the StkFrames<T> object as inputs to the function and replace with corresponding outputs.
  /*!
    The StkFrames<T> argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames<T> argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the function and write outputs to the \c oFrames object.
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

  T a1_;
  T a2_;
  T a3_;
  T gain_; 
  T threshold_;
};

template<typename T>
inline T Cubic<T>::tick( T input )
{
  T inSquared = input * input;
  T inCubed = inSquared * input;

  this->lastFrame_[0] = gain_ * (a1_ * input + a2_ * inSquared + a3_ * inCubed);

  // Apply threshold if we are out of range.
  if ( fabs( this->lastFrame_[0] ) > threshold_ ) {
    this->lastFrame_[0] = ( this->lastFrame_[0] < 0 ? -threshold_ : threshold_ );
  }

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Cubic<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Cubic::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = tick( *samples );

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& Cubic<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "Cubic::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop )
    *oSamples = tick( *iSamples );

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

} // stk namespace

