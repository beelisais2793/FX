#pragma once
#include "Stk.hpp"

namespace stk 
{

/***************************************************/
/*! \class Function
    \brief STK abstract function parent class.

    This class provides common functionality for STK classes that
    implement tables or other types of input to output function
    mappings.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Function : public Stk
{
 public:
  //! Class constructor.
  Function( void ) { lastFrame_.resize( 1, 1, 0.0 ); };

  //! Return the last computed output sample.
  T lastOut( void ) const { return  lastFrame_[0]; };

  //! Take one sample input and compute one sample of output.
  virtual T tick( T input ) = 0;

 protected:

  StkFrames<T> lastFrame_;

};

} // stk namespace

