#pragma once
#include "StkFunction.hpp"

namespace stk {

/***************************************************/
/*! \class JetTable
    \brief STK jet table class.

    This class implements a flue jet non-linear
    function, computed by a polynomial calculation.
    Contrary to the name, this is not a "table".

    Consult Fletcher and Rossing, Karjalainen,
    Cook, and others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class JetTable : public Function<T>
{
public:

  //! Take one sample input and map to one sample of output.
  T tick( T input );

  //! Take a channel of the StkFrames<T> object as inputs to the table and replace with corresponding outputs.
  /*!
    The StkFrames<T> argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames<T> argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the table and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

};

template<typename T>
inline T JetTable<T>::tick( T input )
{
  // Perform "table lookup" using a polynomial
  // calculation (x^3 - x), which approximates
  // the jet sigmoid behavior.
  this->lastFrame_[0] = input * (input * input - 1.0);

  // Saturate at +/- 1.0.
  if ( this->lastFrame_[0] > 1.0 ) this->lastFrame_[0] = 1.0;
  if ( this->lastFrame_[0] < -1.0 ) this->lastFrame_[0] = -1.0; 
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& JetTable<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "JetTable::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples = *samples * (*samples * *samples - 1.0);
    if ( *samples > 1.0) *samples = 1.0;
    if ( *samples < -1.0) *samples = -1.0;
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& JetTable<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "JetTable::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    *oSamples = *oSamples * (*oSamples * *oSamples - 1.0);
    if ( *oSamples > 1.0) *oSamples = 1.0;
    if ( *oSamples < -1.0) *oSamples = -1.0;
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

} // stk namespace

