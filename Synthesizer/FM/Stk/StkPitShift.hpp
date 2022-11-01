#pragma once
#include "StkEffect.hpp"
#include "StkDelayL.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

const int maxDelay = 5024;

template<typename T>
class PitShift : public Effect<T>
{
 public:
  //! Class constructor.
  PitShift( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set the pitch shift factor (1.0 produces no shift).
  void setShift( T shift );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Input one sample to the effect and return one output.
  T tick( T input );

  //! Take a channel of the StkFrames<T> object as inputs to the effect and replace with corresponding outputs.
  /*!
    The StkFrames<T> argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames<T> argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the effect and write outputs to the \c oFrames object.
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

  DelayL<T> delayLine_[2];
  T delay_[2];
  T env_[2];
  T rate_;
  unsigned long delayLength_;
  unsigned long halfLength_;

};


template<typename T>
inline T PitShift<T>::tick( T input )
{
  // Calculate the two delay length values, keeping them within the
  // range 12 to maxDelay-12.
  delay_[0] += rate_;
  while ( delay_[0] > maxDelay-12 ) delay_[0] -= delayLength_;
  while ( delay_[0] < 12 ) delay_[0] += delayLength_;

  delay_[1] = delay_[0] + halfLength_;
  while ( delay_[1] > maxDelay-12 ) delay_[1] -= delayLength_;
  while ( delay_[1] < 12 ) delay_[1] += delayLength_;

  // Set the new delay line lengths.
  delayLine_[0].setDelay( delay_[0] );
  delayLine_[1].setDelay( delay_[1] );

  // Calculate a triangular envelope.
  env_[1] = fabs( ( delay_[0] - halfLength_ + 12 ) * ( 1.0 / (halfLength_ + 12 ) ) );
  env_[0] = 1.0 - env_[1];

  // Delay input and apply envelope.
  this->lastFrame_[0] =  env_[0] * delayLine_[0].tick( input );
  this->lastFrame_[0] += env_[1] * delayLine_[1].tick( input );

  // Compute effect mix and output.
  this->lastFrame_[0] *= this->effectMix_;
  this->lastFrame_[0] += ( 1.0 - this->effectMix_ ) * input;

  return this->lastFrame_[0];
}

/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
PitShift<T>::PitShift( void )
{
  delayLength_ = maxDelay - 24;
  halfLength_ = delayLength_ / 2;
  delay_[0] = 12;
  delay_[1] = maxDelay / 2;

  delayLine_[0].setMaximumDelay( maxDelay );
  delayLine_[0].setDelay( delay_[0] );
  delayLine_[1].setMaximumDelay( maxDelay );
  delayLine_[1].setDelay( delay_[1] );
  this->effectMix_ = 0.5;
  rate_ = 1.0;
}

template<typename T>
void PitShift<T>::clear()
{
  delayLine_[0].clear();
  delayLine_[1].clear();
  this->lastFrame_[0] = 0.0;
}

template<typename T>
void PitShift<T>::setShift( T shift )
{
  if ( shift < 1.0 ) {
    rate_ = 1.0 - shift; 
  }
  else if ( shift > 1.0 ) {
    rate_ = 1.0 - shift;
  }
  else {
    rate_ = 0.0;
    delay_[0] = halfLength_ + 12;
  }
}

template<typename T>
StkFrames<T>& PitShift<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "PitShift::tick(): channel and StkFrames<T> arguments are incompatible!";
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
StkFrames<T>& PitShift<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "PitShift::tick(): channel and StkFrames<T> arguments are incompatible!";
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

} // stk namespace
