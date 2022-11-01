#pragma once
#include "StkEffect.hpp"
#include "StkDelayL.hpp"
#include "StkSineWave.hpp"

namespace stk {

/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.  It takes a monophonic
    input signal and produces a stereo output signal.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Chorus : public Effect<T>
{
 public:
  //! Class constructor, taking the median desired delay length.
  /*!
    An StkError can be thrown if the rawwave path is incorrect.
  */
  Chorus( T baseDelay = 6000 );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set modulation depth in range 0.0 - 1.0.
  void setModDepth( T depth );

  //! Set modulation frequency.
  void setModFrequency( T frequency );

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

  DelayL<T> delayLine_[2];
  SineWave<T> mods_[2];
  T baseLength_;
  T modDepth_;

};

template<typename T>
inline T Chorus<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "Chorus::lastOut(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return this->lastFrame_[channel];
}

template<typename T>
inline T Chorus<T>::tick( T input, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "Chorus::tick(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  delayLine_[0].setDelay( baseLength_ * 0.707 * ( 1.0 + modDepth_ * mods_[0].tick() ) );
  delayLine_[1].setDelay( baseLength_  * 0.5 *  ( 1.0 - modDepth_ * mods_[1].tick() ) );
  this->lastFrame_[0] = this->effectMix_ * ( delayLine_[0].tick( input ) - input ) + input;
  this->lastFrame_[1] = this->effectMix_ * ( delayLine_[1].tick( input ) - input ) + input;
  return this->lastFrame_[channel];
}

template<typename T>
inline StkFrames<T>& Chorus<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() - 1 ) {
    oStream_ << "Chorus::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels() - 1;
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    delayLine_[0].setDelay( baseLength_ * 0.707 * ( 1.0 + modDepth_ * mods_[0].tick() ) );
    delayLine_[1].setDelay( baseLength_  * 0.5 *  ( 1.0 - modDepth_ * mods_[1].tick() ) );
    *samples = this->effectMix_ * ( delayLine_[0].tick( *samples ) - *samples ) + *samples;
    samples++;
    *samples = this->effectMix_ * ( delayLine_[1].tick( *samples ) - *samples ) + *samples;
  }

  this->lastFrame_[0] = *(samples-hop);
  this->lastFrame_[1] = *(samples-hop+1);
  return frames;
}

template<typename T>
inline StkFrames<T>& Chorus<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() - 1 ) {
    oStream_ << "Chorus::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    delayLine_[0].setDelay( baseLength_ * 0.707 * ( 1.0 + modDepth_ * mods_[0].tick() ) );
    delayLine_[1].setDelay( baseLength_  * 0.5 *  ( 1.0 - modDepth_ * mods_[1].tick() ) );
    *oSamples = this->effectMix_ * ( delayLine_[0].tick( *iSamples ) - *iSamples ) + *iSamples;
    *(oSamples+1) = this->effectMix_ * ( delayLine_[1].tick( *iSamples ) - *iSamples ) + *iSamples;
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  this->lastFrame_[1] = *(oSamples-oHop+1);
  return iFrames;
}

/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.  It takes a monophonic
    input signal and produces a stereo output signal.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Chorus<T>::Chorus( T baseDelay )
{
  this->lastFrame_.resize( 1, 2, 0.0 ); // resize this->lastFrame_ for stereo output
  delayLine_[0].setMaximumDelay( (unsigned long) (baseDelay * 1.414) + 2);
  delayLine_[0].setDelay( baseDelay );
  delayLine_[1].setMaximumDelay( (unsigned long) (baseDelay * 1.414) + 2);
  delayLine_[1].setDelay( baseDelay );
  baseLength_ = baseDelay;

  mods_[0].setFrequency( 0.2 );
  mods_[1].setFrequency( 0.222222 );
  modDepth_ = 0.05;
  this->effectMix_ = 0.5;
  this->clear();
}

template<typename T>
void Chorus<T>::clear( void )
{
  delayLine_[0].clear();
  delayLine_[1].clear();
  this->lastFrame_[0] = 0.0;
  this->lastFrame_[1] = 0.0;
}

template<typename T>
void Chorus<T>::setModDepth( T depth )
{
  if ( depth < 0.0 || depth > 1.0 ) {
    oStream_ << "Chorus::setModDepth(): depth argument must be between 0.0 - 1.0!";
    handleError( StkError::WARNING ); return;
  }

    modDepth_ = depth;
};

template<typename T>
void Chorus<T>::setModFrequency( T frequency )
{
  mods_[0].setFrequency( frequency );
  mods_[1].setFrequency( frequency * 1.1111 );
}

} // stk namespace
