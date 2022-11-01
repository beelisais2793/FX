#pragma once
#include "Stk.hpp"
#include "StkVector3D.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995--2021.
*/
/***************************************************/

template<typename T>
class Sphere : public Stk
{
public:
  //! Constructor taking an initial radius value.
  Sphere( T radius = 1.0 ) { radius_ = radius; mass_ = 1.0; };

  //! Set the 3D center position of the sphere.
  void setPosition( T x, T y, T z ) { position_.setXYZ(x, y, z); };

  //! Set the 3D velocity of the sphere.
  void setVelocity( T x, T y, T z ) { velocity_.setXYZ(x, y, z); };

  //! Set the radius of the sphere.
  void setRadius( T radius ) { radius_ = radius; };

  //! Set the mass of the sphere.
  void setMass( T mass ) { mass_ = mass; };

  //! Get the current position of the sphere as a 3D vector.
  Vector3D<T>* getPosition( void ) { return &position_; };

  //! Get the relative position of the given point to the sphere as a 3D vector.
  Vector3D<T>* getRelativePosition( Vector3D<T> *position );

  //! Set the velocity of the sphere as a 3D vector.
  T getVelocity( Vector3D<T>* velocity );

  //! Returns the distance from the sphere boundary to the given position (< 0 if inside).
  T isInside( Vector3D<T> *position );

  //! Get the current sphere radius.
  T getRadius( void ) { return radius_; };

  //! Get the current sphere mass.
  T getMass( void ) { return mass_; };

  //! Increase the current sphere velocity by the given 3D components.
  void addVelocity( T x, T y, T z );

  //! Move the sphere for the given time increment.
  void tick( T timeIncrement );
   
private:
  Vector3D<T> position_;
  Vector3D<T> velocity_;
  Vector3D<T> workingVector_;
  T radius_;
  T mass_;
};

template<typename T>
inline void Sphere<T>::tick( T timeIncrement )
{
  position_.setX(position_.getX() + (timeIncrement * velocity_.getX()));
  position_.setY(position_.getY() + (timeIncrement * velocity_.getY()));
  position_.setZ(position_.getZ() + (timeIncrement * velocity_.getZ()));
};

/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995--2021.
*/
/***************************************************/


template<typename T>
Vector3D<T>* Sphere<T>::getRelativePosition( Vector3D<T>* position )
{
  workingVector_.setXYZ(position->getX() - position_.getX(),
                        position->getY() - position_.getY(),  
                        position->getZ() - position_.getZ());
  return &workingVector_;
};

template<typename T>
T Sphere<T>::getVelocity( Vector3D<T>* velocity )
{
  velocity->setXYZ( velocity_.getX(), velocity_.getY(), velocity_.getZ() );
  return velocity_.getLength();
};

template<typename T>
T Sphere<T>::isInside( Vector3D<T> *position )
{
  // Return directed distance from aPosition to spherical boundary ( <
  // 0 if inside).
  T distance;
  Vector3D<T> *tempVector;

  tempVector = this->getRelativePosition( position );
  distance = tempVector->getLength();
  return distance - radius_;
};

template<typename T>
void Sphere<T>::addVelocity(T x, T y, T z)
{
  velocity_.setX(velocity_.getX() + x);
  velocity_.setY(velocity_.getY() + y);
  velocity_.setZ(velocity_.getZ() + z);
}

} // stk namespace
