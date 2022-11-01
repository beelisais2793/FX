#pragma once
#include "StkGenerator.hpp"
#include <stdlib.h>

namespace stk {

/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Noise : public Generator<T>
{
public:

  //! Default constructor that can also take a specific seed value.
  /*!
    If the seed value is zero (the default value), the random number generator is
    seeded with the system time.
  */
  Noise( unsigned int seed = 0 );

  //! Seed the random number generator with a specific seed value.
  /*!
    If no seed is provided or the seed value is zero, the random
    number generator is seeded with the current system time.
  */
  void setSeed( unsigned int seed = 0 );

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Compute and return one output sample.
  T tick( void );

  //! Fill a channel of the StkFrames<T> object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

protected:

};

template<typename T>
inline T Noise<T>::tick( void )
{
  return this->lastFrame_[0] = (T) ( 2.0 * rand() / (RAND_MAX + 1.0) - 1.0 );
}


template<typename T>
inline StkFrames<T>& Noise<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Noise::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = (T) ( 2.0 * rand() / (RAND_MAX + 1.0) - 1.0 );

  this->lastFrame_[0] = *(samples-hop);
  return frames;
}

template<typename T>
Noise<T> :: Noise( unsigned int seed )
{
  // Seed the random number generator
  this->setSeed( seed );
}

template<typename T>
void Noise<T> :: setSeed( unsigned int seed )
{
  if ( seed == 0 )
    srand( (unsigned int) time( NULL ) );
  else
    srand( seed );
}


} // stk namespace

