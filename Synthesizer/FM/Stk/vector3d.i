%{
#include "Vector3D.h"
%}

namespace stk
{
    class Vector3D : public Stk
    {
    public:


        Vector3D( StkFloat x = 0.0, StkFloat y = 0.0, StkFloat z = 0.0 );
        
        StkFloat getX( void );
        StkFloat getY( void );
        StkFloat getZ( void );
        StkFloat getLength( void );

        void setXYZ( StkFloat x, StkFloat y, StkFloat z );
        void setX( StkFloat x );
        void setY( StkFloat y );
        void setZ( StkFloat z );
   };
}