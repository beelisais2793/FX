#pragma once
#include "StkEffect.hpp" 
#include "StkDelay.hpp" 
#include <iostream>

namespace stk {

/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements an echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Echo : public Effect<T>
{
 public:
  //! Class constructor, taking the longest desired delay length (one second default value).
  /*!
    The default delay value is set to 1/2 the maximum delay length.
  */
  Echo( unsigned long maximumDelay = (unsigned long) sampleRate() );

  //! Reset and clear all internal state.
  void clear();

  //! Set the maximum delay line length in samples.
  void setMaximumDelay( unsigned long delay );

  //! Set the delay line length in samples.
  void setDelay( unsigned long delay );

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

  Delay<T> delayLine_;
  unsigned long length_;

};

template<typename T>
inline T Echo<T>::tick( T input )
{
  this->lastFrame_[0] = this->effectMix_ * ( delayLine_.tick( input ) - input ) + input;
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Echo<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Echo::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples = this->effectMix_ * ( delayLine_.tick( *samples ) - *samples ) + *samples;
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& Echo<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "Echo::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    *oSamples = this->effectMix_ * ( delayLine_.tick( *iSamples ) - *iSamples ) + *iSamples;
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements an echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Echo<T>::Echo( unsigned long maximumDelay ) : Effect<T>()
{
  this->setMaximumDelay( maximumDelay );
  delayLine_.setDelay( length_ >> 1 );
  this->effectMix_ = 0.5;
  this->clear();
}

template<typename T>
void Echo<T>::clear( void )
{
  delayLine_.clear();
  this->lastFrame_[0] = 0.0;
}

template<typename T>
void Echo<T>::setMaximumDelay( unsigned long delay )
{
  if ( delay == 0 ) {
    oStream_ << "Echo::setMaximumDelay: parameter cannot be zero!";
    handleError( StkError::WARNING ); return;
  }

  length_ = delay;
  delayLine_.setMaximumDelay( delay );
}

template<typename T>
void Echo<T>::setDelay( unsigned long delay )
{
  if ( delay > length_ ) {
    oStream_ << "Echo::setDelay: parameter is greater than maximum delay length!";
    handleError( StkError::WARNING ); return;
  }

  delayLine_.setDelay( delay );
}

} // stk namespace


