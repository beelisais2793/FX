#pragma once
#include "Stk.hpp"

namespace stk {

/***************************************************/
/*! \class WvIn
    \brief STK audio input abstract base class.

    This class provides common functionality for a variety of audio
    data input subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class WvIn : public Stk
{
public:
  //! Return the number of audio channels in the data or stream.
  unsigned int channelsOut( void ) const { return data_.channels(); };

  //! Return an StkFrames reference to the last computed sample frame.
  /*!
    If no file data is loaded, an empty container is returned.
   */
  const StkFrames<T>& lastFrame( void ) const { return lastFrame_; };

  //! Compute one sample frame and return the specified \c channel value.
  virtual T tick( unsigned int channel = 0 ) = 0;

  //! Fill the StkFrames object with computed sample frames, starting at the specified channel and return the same reference.
  virtual StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 ) = 0;

protected:

  StkFrames<T> data_;
  StkFrames<T> lastFrame_;

};

} // stk namespace
