#pragma once
#include "Stk.hpp"

namespace stk {

/***************************************************/
/*! \class WvOut
    \brief STK audio output abstract base class.

    This class provides common functionality for a variety of audio
    data output subclasses.

    Currently, WvOut is non-interpolating and the output rate is
    always stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class WvOut : public Stk
{
 public:

  //! Default constructor.
  WvOut( void ) : frameCounter_(0), clipping_(false) {};

  //! Return the number of sample frames output.
  unsigned long getFrameCount( void ) const { return frameCounter_; };

  //! Return the number of seconds of data output.
  T getTime( void ) const { return (T) frameCounter_ / stk::sampleRate(); };

  //! Returns \c true if clipping has been detected during output since instantiation or the last reset.
  bool clipStatus( void ) { return clipping_; };

  //! Reset the clipping status to \c false.
  void resetClipStatus( void ) { clipping_ = false; };

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if an output error occurs.
  */
  virtual void tick( const T sample ) = 0;

  //! Output the StkFrames<T> data.
  virtual void tick( const StkFrames<T>& frames ) = 0;

 protected:

  // Check for sample clipping and clamp.
  T& clipTest( T& sample );

  StkFrames<T> data_;
  unsigned long frameCounter_;
  bool clipping_;

};

template<typename T>
inline T& WvOut<T>::clipTest( T& sample )
{
  bool clip = false;
  if ( sample > 1.0 ) {
    sample = 1.0;
    clip = true;
  }
  else if ( sample < -1.0 ) {
    sample = -1.0;
    clip = true;
  }

  if ( clip == true && clipping_ == false ) {
    // First occurrence of clipping since instantiation or reset.
    clipping_ = true;
    oStream_ << "WvOut: data value(s) outside +-1.0 detected ... clamping at outer bound!";
    handleError( StkError::WARNING );
  }

  return sample;
}

} // stk namespace
