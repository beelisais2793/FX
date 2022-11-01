#include "tfp/views/RealtimePlot3D.hpp"
#include <qwt3d_function.h>
#include <qwt3d_color.h>

namespace tfp {

/*
class Jet : public Qwt3D::StandardColor
{
public:
    explicit Jet(Qwt3D::Plot3D* data, unsigned size = 100) : Qwt3D::StandardColor(data, size) {}
    void reset(unsigned size)
    {
        colors_ = Qwt3D::ColorVector(size);
        Qwt3D::RGBA elem;

        double dsize = size;

        for (unsigned int i=0; i!=size; ++i)
        {
            elem.r = red(i/dsize);
            elem.g = green(i/dsize);
            elem.b = blue(i/dsize);
            elem.a = 1.0;
            colors_[i] = elem;
        }
    }

    double interpolate( double val, double y0, double x0, double y1, double x1 ) {
        return (val-x0)*(y1-y0)/(x1-x0) + y0;
    }

    double base( double val ) {
        if ( val <= -0.75 ) return 0;
        else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
        else if ( val <= 0.25 ) return 1.0;
        else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
        else return 0.0;
    }

    double red( double gray ) {
        return base( gray - 0.5 );
    }
    double green( double gray ) {
        return base( gray );
    }
    double blue( double gray ) {
        return base( gray + 0.5 );
    }
};*/

// ----------------------------------------------------------------------------
RealtimePlot3D::RealtimePlot3D(QWidget* parent) :
    GridPlot(parent)
{
}

// ----------------------------------------------------------------------------
void RealtimePlot3D::efficientlyUpdateData(Qwt3D::Function* function)
{
    if (hasData() == false)
        return;


}

}
