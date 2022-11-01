#pragma once
#include "StkEffect.hpp"
#include "StkDelay.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class NRev
    \brief CCRMA's NRev reverberator class.

    This class takes a monophonic input signal and produces a stereo
    output signal.  It is derived from the CLM NRev function, which is
    based on the use of networks of simple allpass and comb delay
    filters.  This particular arrangement consists of 6 comb filters
    in parallel, followed by 3 allpass filters, a lowpass filter, and
    another allpass in series, followed by two allpass filters in
    parallel with corresponding right and left outputs.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class NRev : public Effect<T>
{
 public:
  //! Class constructor taking a T60 decay time argument (one second default value).
  NRev( T T60 = 1.0 );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set the reverberation T60 decay time.
  void setT60( T T60 );

  //! Return the specified channel value of the last computed stereo frame.
  /*!
    Use the lastFrame() function to get both values of the last
    computed stereo frame.  The \c channel argument must be 0 or 1
    (the first channel is specified by 0).  However, range checking is
    only performed if _STK_DEBUG_ is defined during compilation, in
    which case an out-of-range value will trigger an StkError
    exception.
  */
  T lastOut( unsigned int channel = 0 );

  //! Input one sample to the effect and return the specified \c channel value of the computed stereo frame.
  /*!
    Use the lastFrame() function to get both values of the computed
    stereo output frame. The \c channel argument must be 0 or 1 (the
    first channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  T tick( T input, unsigned int channel = 0 );

  //! Take a channel of the StkFrames<T> object as inputs to the effect and replace with stereo outputs.
  /*!
    The StkFrames<T> argument reference is returned.  The stereo
    outputs are written to the StkFrames<T> argument starting at the
    specified \c channel.  Therefore, the \c channel argument must be
    less than ( channels() - 1 ) of the StkFrames<T> argument (the first
    channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the effect and write stereo outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  The \c iChannel
    argument must be less than the number of channels in the \c
    iFrames argument (the first channel is specified by 0).  The \c
    oChannel argument must be less than ( channels() - 1 ) of the \c
    oFrames argument.  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

 protected:

  Delay<T> allpassDelays_[8];
  Delay<T> combDelays_[6];
  T allpassCoefficient_;
  T combCoefficient_[6];
	T lowpassState_;

};

template<typename T>
inline T NRev<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "NRev::lastOut(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return this->lastFrame_[channel];
}


template<typename T>
inline T NRev<T>::tick( T input, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "NRev::tick(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T temp, temp0, temp1, temp2, temp3;
  int i;

  temp0 = 0.0;
  for ( i=0; i<6; i++ ) {
    temp = input + (combCoefficient_[i] * combDelays_[i].lastOut());
    temp0 += combDelays_[i].tick(temp);
  }

  for ( i=0; i<3; i++ )	{
    temp = allpassDelays_[i].lastOut();
    temp1 = allpassCoefficient_ * temp;
    temp1 += temp0;
    allpassDelays_[i].tick(temp1);
    temp0 = -(allpassCoefficient_ * temp1) + temp;
  }

	// One-pole lowpass filter.
  lowpassState_ = 0.7 * lowpassState_ + 0.3 * temp0;
  temp = allpassDelays_[3].lastOut();
  temp1 = allpassCoefficient_ * temp;
  temp1 += lowpassState_;
  allpassDelays_[3].tick( temp1 );
  temp1 = -( allpassCoefficient_ * temp1 ) + temp;
    
  temp = allpassDelays_[4].lastOut();
  temp2 = allpassCoefficient_ * temp;
  temp2 += temp1;
  allpassDelays_[4].tick( temp2 );
  this->lastFrame_[0] = this->effectMix_*( -( allpassCoefficient_ * temp2 ) + temp );
    
  temp = allpassDelays_[5].lastOut();
  temp3 = allpassCoefficient_ * temp;
  temp3 += temp1;
  allpassDelays_[5].tick( temp3 );
  this->lastFrame_[1] = this->effectMix_*( - ( allpassCoefficient_ * temp3 ) + temp );

  temp = ( 1.0 - this->effectMix_ ) * input;
  this->lastFrame_[0] += temp;
  this->lastFrame_[1] += temp;
    
  return this->lastFrame_[channel];
}

/***************************************************/
/*! \class NRev
    \brief CCRMA's NRev reverberator class.

    This class takes a monophonic input signal and produces a stereo
    output signal.  It is derived from the CLM NRev function, which is
    based on the use of networks of simple allpass and comb delay
    filters.  This particular arrangement consists of 6 comb filters
    in parallel, followed by 3 allpass filters, a lowpass filter, and
    another allpass in series, followed by two allpass filters in
    parallel with corresponding right and left outputs.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
NRev<T>::NRev( T T60 )
{
  if ( T60 <= 0.0 ) {
    oStream_ << "NRev::NRev: argument (" << T60 << ") must be positive!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  this->lastFrame_.resize( 1, 2, 0.0 ); // resize this->lastFrame_ for stereo output

  int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19};
  double scaler = stk::sampleRate() / 25641.0;

  int delay, i;
  for ( i=0; i<15; i++ ) {
    delay = (int) floor(scaler * lengths[i]);
    if ( (delay & 1) == 0) delay++;
    while ( !this->isPrime(delay) ) delay += 2;
    lengths[i] = delay;
  }

  for ( i=0; i<6; i++ ) {
    combDelays_[i].setMaximumDelay( lengths[i] );
    combDelays_[i].setDelay( lengths[i] );
    combCoefficient_[i] = pow(10.0, (-3 * lengths[i] / (T60 * stk::sampleRate())));
  }

  for ( i=0; i<8; i++ ) {
	  allpassDelays_[i].setMaximumDelay( lengths[i+6] );
	  allpassDelays_[i].setDelay( lengths[i+6] );
  }

  this->setT60( T60 );
  allpassCoefficient_ = 0.7;
  this->effectMix_ = 0.3;
  this->clear();
}


template<typename T>
void NRev<T>::clear()
{
  int i;
  for (i=0; i<6; i++) combDelays_[i].clear();
  for (i=0; i<8; i++) allpassDelays_[i].clear();
  this->lastFrame_[0] = 0.0;
  this->lastFrame_[1] = 0.0;
  lowpassState_ = 0.0;
}


template<typename T>
void NRev<T>::setT60( T T60 )
{
  if ( T60 <= 0.0 ) {
    oStream_ << "NRev::setT60: argument (" << T60 << ") must be positive!";
    handleError( StkError::WARNING ); return;
  }

  for ( int i=0; i<6; i++ )
    combCoefficient_[i] = pow(10.0, (-3.0 * combDelays_[i].getDelay() / (T60 * stk::sampleRate())));
}

template<typename T>
StkFrames<T>& NRev<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() - 1 ) {
    oStream_ << "NRev::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples = tick( *samples );
    *(samples+1) = this->lastFrame_[1];
  }

  return frames;
}

template<typename T>
StkFrames<T>& NRev<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() - 1 ) {
    oStream_ << "NRev::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    *oSamples = tick( *iSamples );
    *(oSamples+1) = this->lastFrame_[1];
  }

  return iFrames;
}

} // stk namespace
