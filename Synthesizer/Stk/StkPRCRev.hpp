#pragma once
#include "StkEffect.hpp"
#include "StkDelay.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class PRCRev
    \brief Perry's simple reverberator class.

    This class takes a monophonic input signal and produces a stereo
    output signal.  It is based on some of the famous Stanford/CCRMA
    reverbs (NRev, KipRev), which were based on the
    Chowning/Moorer/Schroeder reverberators using networks of simple
    allpass and comb delay filters.  This class implements two series
    allpass units and two parallel comb filters.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class PRCRev : public Effect<T>
{
public:
  //! Class constructor taking a T60 decay time argument (one second default value).
  PRCRev( T T60 = 1.0 );

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

  Delay<T>    allpassDelays_[2];
  Delay<T>    combDelays_[2];
  T allpassCoefficient_;
  T combCoefficient_[2];

};

template<typename T>
inline T PRCRev<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "PRCRev::lastOut(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return this->lastFrame_[channel];
}


template<typename T>
inline T PRCRev<T>::tick( T input, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "PRCRev::tick(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T temp, temp0, temp1, temp2, temp3;

  temp = allpassDelays_[0].lastOut();
  temp0 = allpassCoefficient_ * temp;
  temp0 += input;
  allpassDelays_[0].tick(temp0);
  temp0 = -(allpassCoefficient_ * temp0) + temp;
    
  temp = allpassDelays_[1].lastOut();
  temp1 = allpassCoefficient_ * temp;
  temp1 += temp0;
  allpassDelays_[1].tick(temp1);
  temp1 = -(allpassCoefficient_ * temp1) + temp;
    
  temp2 = temp1 + ( combCoefficient_[0] * combDelays_[0].lastOut() );
  temp3 = temp1 + ( combCoefficient_[1] * combDelays_[1].lastOut() );

  this->lastFrame_[0] = this->effectMix_ * (combDelays_[0].tick(temp2));
  this->lastFrame_[1] = this->effectMix_ * (combDelays_[1].tick(temp3));
  temp = (1.0 - this->effectMix_) * input;
  this->lastFrame_[0] += temp;
  this->lastFrame_[1] += temp;

  return this->lastFrame_[channel];
}

/***************************************************/
/*! \class PRCRev
    \brief Perry's simple reverberator class.

    This class is based on some of the famous
    Stanford/CCRMA reverbs (NRev, KipRev), which
    were based on the Chowning/Moorer/Schroeder
    reverberators using networks of simple allpass
    and comb delay filters.  This class implements
    two series allpass units and two parallel comb
    filters.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
PRCRev<T>::PRCRev( T T60 )
{
  if ( T60 <= 0.0 ) {
    oStream_ << "PRCRev::PRCRev: argument (" << T60 << ") must be positive!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  this->lastFrame_.resize( 1, 2, 0.0 ); // resize this->lastFrame_ for stereo output

  // Delay lengths for 44100 Hz sample rate.
  int lengths[4]= {341, 613, 1557, 2137};
  double scaler = stk::sampleRate() / 44100.0;

  // Scale the delay lengths if necessary.
  int delay, i;
  if ( scaler != 1.0 ) {
    for (i=0; i<4; i++)	{
      delay = (int) floor(scaler * lengths[i]);
      if ( (delay & 1) == 0) delay++;
      while ( !this->isPrime(delay) ) delay += 2;
      lengths[i] = delay;
    }
  }

  for ( i=0; i<2; i++ )	{
	  allpassDelays_[i].setMaximumDelay( lengths[i] );
	  allpassDelays_[i].setDelay( lengths[i] );

    combDelays_[i].setMaximumDelay( lengths[i+2] );
    combDelays_[i].setDelay( lengths[i+2] );
  }

  this->setT60( T60 );
  allpassCoefficient_ = 0.7;
  this->effectMix_ = 0.5;
  this->clear();
}

template<typename T>
void PRCRev<T>::clear( void )
{
  allpassDelays_[0].clear();
  allpassDelays_[1].clear();
  combDelays_[0].clear();
  combDelays_[1].clear();
  this->lastFrame_[0] = 0.0;
  this->lastFrame_[1] = 0.0;
}

template<typename T>
void PRCRev<T>::setT60( T T60 )
{
  if ( T60 <= 0.0 ) {
    oStream_ << "PRCRev::setT60: argument (" << T60 << ") must be positive!";
    handleError( StkError::WARNING ); return;
  }

  combCoefficient_[0] = pow(10.0, (-3.0 * combDelays_[0].getDelay() / (T60 * stk::sampleRate())));
  combCoefficient_[1] = pow(10.0, (-3.0 * combDelays_[1].getDelay() / (T60 * stk::sampleRate())));
}

template<typename T>
StkFrames<T>& PRCRev<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() - 1 ) {
    oStream_ << "PRCRev::tick(): channel and StkFrames<T> arguments are incompatible!";
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
StkFrames<T>& PRCRev<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() - 1 ) {
    oStream_ << "PRCRev::tick(): channel and StkFrames<T> arguments are incompatible!";
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
