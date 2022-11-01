%{
#include "Sphere.h"
%}
namespace stk {

    /***************************************************/
    /*! \class Sphere
        \brief STK sphere class.
        This class implements a spherical ball with
        radius, mass, position, and velocity parameters.
        by Perry R. Cook, 1995--2021.
    */
    /***************************************************/

    class Sphere : public Stk
    {
    public:


        Sphere( StkFloat radius = 1.0 );
        void setPosition( StkFloat x, StkFloat y, StkFloat z );
        void setVelocity( StkFloat x, StkFloat y, StkFloat z );
        void setRadius( StkFloat radius );

        void setMass( StkFloat mass );
        Vector3D* getPosition( void );
        Vector3D* getRelativePosition( Vector3D *position );        
        StkFloat getVelocity( Vector3D* velocity );
        StkFloat isInside( Vector3D *position );
        StkFloat getRadius( void );
        StkFloat getMass( void );
        void addVelocity( StkFloat x, StkFloat y, StkFloat z );
        void tick( StkFloat timeIncrement );
    };

} // stk namespace