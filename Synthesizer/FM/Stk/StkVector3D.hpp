#pragma once

#include "Stk.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995--2021.
*/
/***************************************************/

template<typename T>
class Vector3D : public Stk
{

public:
  //! Default constructor taking optional initial X, Y, and Z values.
  Vector3D( T x = 0.0, T y = 0.0, T z = 0.0 ) { setXYZ( x, y, z ); };

  //! Get the current X value.
  T getX( void ) { return X_; };

  //! Get the current Y value.
  T getY( void ) { return Y_; };

  //! Get the current Z value.
  T getZ( void ) { return Z_; };

  //! Calculate the vector length.
  T getLength( void );

  //! Set the X, Y, and Z values simultaniously.
  void setXYZ( T x, T y, T z ) { X_ = x; Y_ = y; Z_ = z; };

  //! Set the X value.
  void setX( T x ) { X_ = x; };

  //! Set the Y value.
  void setY( T y ) { Y_ = y; };

  //! Set the Z value.
  void setZ( T z ) { Z_ = z; };

protected:
  T X_;
  T Y_;
  T Z_;
};

template<typename T>
inline T Vector3D<T> :: getLength( void )
{
  T temp;
  temp = X_ * X_;
  temp += Y_ * Y_;
  temp += Z_ * Z_;
  temp = sqrt( temp );
  return temp;
}

} // stk namespace
