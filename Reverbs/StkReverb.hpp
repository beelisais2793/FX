//////////////////////////////////////////////////
// Stk Reverb
//////////////////////////////////////////////////
#ifndef STK_PRCREV_H
#define STK_PRCREV_H

#include "Effect.h"
#include "Delay.h"

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

class PRCRev : public Effect
{
public:
  //! Class constructor taking a T60 decay time argument (one second default value).
  PRCRev( StkFloat T60 = 1.0 );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set the reverberation T60 decay time.
  void setT60( StkFloat T60 );

  //! Return the specified channel value of the last computed stereo frame.
  /*!
    Use the lastFrame() function to get both values of the last
    computed stereo frame.  The \c channel argument must be 0 or 1
    (the first channel is specified by 0).  However, range checking is
    only performed if _STK_DEBUG_ is defined during compilation, in
    which case an out-of-range value will trigger an StkError
    exception.
  */
  StkFloat lastOut( unsigned int channel = 0 );

  //! Input one sample to the effect and return the specified \c channel value of the computed stereo frame.
  /*!
    Use the lastFrame() function to get both values of the computed
    stereo output frame. The \c channel argument must be 0 or 1 (the
    first channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  StkFloat tick( StkFloat input, unsigned int channel = 0 );

  //! Take a channel of the StkFrames object as inputs to the effect and replace with stereo outputs.
  /*!
    The StkFrames argument reference is returned.  The stereo
    outputs are written to the StkFrames argument starting at the
    specified \c channel.  Therefore, the \c channel argument must be
    less than ( channels() - 1 ) of the StkFrames argument (the first
    channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

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
  StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

protected:

  Delay    allpassDelays_[2];
  Delay    combDelays_[2];
  StkFloat allpassCoefficient_;
  StkFloat combCoefficient_[2];

};

inline StkFloat PRCRev :: lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "PRCRev::lastOut(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return lastFrame_[channel];
}

 inline StkFloat PRCRev :: tick( StkFloat input, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "PRCRev::tick(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat temp, temp0, temp1, temp2, temp3;

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

  lastFrame_[0] = effectMix_ * (combDelays_[0].tick(temp2));
  lastFrame_[1] = effectMix_ * (combDelays_[1].tick(temp3));
  temp = (1.0 - effectMix_) * input;
  lastFrame_[0] += temp;
  lastFrame_[1] += temp;

  return lastFrame_[channel];
}

} // stk namespace

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

#include "NRev.h"
#include <cmath>



NRev :: NRev( StkFloat T60 )
{
  if ( T60 <= 0.0 ) {
    oStream_ << "NRev::NRev: argument (" << T60 << ") must be positive!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  lastFrame_.resize( 1, 2, 0.0 ); // resize lastFrame_ for stereo output

  int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19};
  double scaler = Stk::sampleRate() / 25641.0;

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
    combCoefficient_[i] = pow(10.0, (-3 * lengths[i] / (T60 * Stk::sampleRate())));
  }

  for ( i=0; i<8; i++ ) {
	  allpassDelays_[i].setMaximumDelay( lengths[i+6] );
	  allpassDelays_[i].setDelay( lengths[i+6] );
  }

  this->setT60( T60 );
  allpassCoefficient_ = 0.7;
  effectMix_ = 0.3;
  this->clear();
}

void NRev :: clear()
{
  int i;
  for (i=0; i<6; i++) combDelays_[i].clear();
  for (i=0; i<8; i++) allpassDelays_[i].clear();
  lastFrame_[0] = 0.0;
  lastFrame_[1] = 0.0;
  lowpassState_ = 0.0;
}

void NRev :: setT60( StkFloat T60 )
{
  if ( T60 <= 0.0 ) {
    oStream_ << "NRev::setT60: argument (" << T60 << ") must be positive!";
    handleError( StkError::WARNING ); return;
  }

  for ( int i=0; i<6; i++ )
    combCoefficient_[i] = pow(10.0, (-3.0 * combDelays_[i].getDelay() / (T60 * Stk::sampleRate())));
}

StkFrames& NRev :: tick( StkFrames& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() - 1 ) {
    oStream_ << "NRev::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples = tick( *samples );
    *(samples+1) = lastFrame_[1];
  }

  return frames;
}

StkFrames& NRev :: tick( StkFrames& iFrames, StkFrames& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() - 1 ) {
    oStream_ << "NRev::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *iSamples = &iFrames[iChannel];
  StkFloat *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    *oSamples = tick( *iSamples );
    *(oSamples+1) = lastFrame_[1];
  }

  return iFrames;
}
