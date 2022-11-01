#pragma once
#include "StkFunction.hpp"

namespace stk {

/***************************************************/
/*! \class ReedTable
    \brief STK reed table class.

    This class implements a simple one breakpoint,
    non-linear reed function, as described by
    Smith (1986).  This function is based on a
    memoryless non-linear spring model of the reed
    (the reed mass is ignored) which saturates when
    the reed collides with the mouthpiece facing.

    See McIntyre, Schumacher, & Woodhouse (1983),
    Smith (1986), Hirschman, Cook, Scavone, and
    others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class ReedTable : public Function<T>
{
public:
  //! Default constructor.
  ReedTable( void ) : offset_(0.6), slope_(-0.8) {};

  //! Set the table offset value.
  /*!
    The table offset roughly corresponds to the size
    of the initial reed tip opening (a greater offset
    represents a smaller opening).
  */
  void setOffset( T offset ) { offset_ = offset; };

  //! Set the table slope value.
  /*!
   The table slope roughly corresponds to the reed
   stiffness (a greater slope represents a harder
   reed).
  */
  void setSlope( T slope ) { slope_ = slope; };

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

protected:

  T offset_;
  T slope_;

};

template<typename T>
inline T ReedTable<T>::tick( T input )    
{
  // The input is differential pressure across the reed.
  this->lastFrame_[0] = offset_ + (slope_ * input);

  // If output is > 1, the reed has slammed shut and the
  // reflection function value saturates at 1.0.
  if ( this->lastFrame_[0] > 1.0) this->lastFrame_[0] = (T) 1.0;

  // This is nearly impossible in a physical system, but
  // a reflection function value of -1.0 corresponds to
  // an open end (and no discontinuity in bore profile).
  if ( this->lastFrame_[0] < -1.0) this->lastFrame_[0] = (T) -1.0;

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& ReedTable<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    this->oStream_ << "ReedTable::tick(): channel and StkFrames<T> arguments are incompatible!";
    this->handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples = offset_ + (slope_ * *samples);
    if ( *samples > 1.0) *samples = 1.0;
    if ( *samples < -1.0) *samples = -1.0;
  }

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
inline StkFrames<T>& ReedTable<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    this->oStream_ << "ReedTable::tick(): channel and StkFrames<T> arguments are incompatible!";
    this->handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    *oSamples = offset_ + (slope_ * *iSamples);
    if ( *oSamples > 1.0) *oSamples = 1.0;
    if ( *oSamples < -1.0) *oSamples = -1.0;
  }

  this->lastFrame_[0] = *(oSamples-oHop);
  return iFrames;
}

} // stk namespace
