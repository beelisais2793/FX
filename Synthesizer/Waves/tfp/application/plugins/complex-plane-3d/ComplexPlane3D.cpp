#include "ComplexPlane3D.hpp"
#include "tfp/views/RealtimePlot3D.hpp"
#include "tfp/models/System.hpp"
#include "tfp/plugin/Tool.hpp"
#include <qwt3d_function.h>
#include <QVBoxLayout>

using namespace tfp;

class MagnitudeFunction : public Qwt3D::Function
{
public:
    MagnitudeFunction(TransferFunction<double>* tf) :
        tf_(tf)
    {
    }

    double operator()(double x, double y)
    {
        return std::log10(20 * std::abs(tf_->evaluate(Type<double>::Complex(x, y))));
    }

private:
    TransferFunction<double>* tf_;
};

// ----------------------------------------------------------------------------
ComplexPlane3D::ComplexPlane3D(QWidget* parent) :
    Tool(parent),
    plot_(new Qwt3D::GridPlot)
{
    setLayout(new QVBoxLayout);

    plot_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plot_->resize(200, 200);

    for (unsigned i=0; i!=plot_->coordinates()->axes.size(); ++i)
    {
        plot_->coordinates()->axes[i].setMajors(5);
        plot_->coordinates()->axes[i].setMinors(5);
    }

    plot_->setRotation(25, 0, 285);
    plot_->setScale(1, 1, 1);
    plot_->setShift(0, -0.4, 0);
    plot_->setZoom(2);
    plot_->setPlotStyle(Qwt3D::FILLED);
    //plot_->setDataColor(new Jet(plot_));
    plot_->setCoordinateStyle(Qwt3D::BOX);
    plot_->setOrtho(false);

    layout()->addWidget(plot_);
}

// ----------------------------------------------------------------------------
void ComplexPlane3D::replot()
{
    plot_->updateData();
    plot_->updateGL();
}

// ----------------------------------------------------------------------------
void ComplexPlane3D::autoScale()
{
}

// ----------------------------------------------------------------------------
void ComplexPlane3D::onSystemParametersChanged()
{
    const double minx = -5.5;
    const double maxx = 0.5;
    const double miny = -3;
    const double maxy = 3;
    const double minz = -4;
    const double maxz = 4;

    MagnitudeFunction func(system_);
    func.setMesh(100, 100);
    func.setDomain(minx, maxx, miny, maxy);
    func.setMinZ(minz);
    func.setMaxZ(maxz);
    func.create(*plot_);

    plot_->createCoordinateSystem(Qwt3D::Triple(minx, miny, minz), Qwt3D::Triple(maxx, maxy, maxz));
    plot_->updateGL();
}

// ----------------------------------------------------------------------------
void ComplexPlane3D::onSystemStructureChanged()
{
    const double minx = -5.5;
    const double maxx = 0.5;
    const double miny = -3;
    const double maxy = 3;
    const double minz = -4;
    const double maxz = 4;

    /*
    double minx = std::numeric_limits<double>::max();
    double maxx = -std::numeric_limits<double>::max();
    double miny = std::numeric_limits<double>::max();
    double maxy = -std::numeric_limits<double>::max();

    for (int i = 0; i != system_->numerator_.size(); ++i)
    {
        const typename Type<double>::Complex& c = system_->numerator_.root(i);
        if (maxx < c.real()) maxx = c.real();
        if (minx > c.real()) minx = c.real();
        if (maxy < c.imag()) maxy = c.imag();
        if (miny > c.imag()) miny = c.imag();
    }

    for (int i = 0; i != system_->denominator_.size(); ++i)
    {
        const typename Type<double>::Complex& c = system_->denominator_.root(i);
        if (maxx < c.real()) maxx = c.real();
        if (minx > c.real()) minx = c.real();
        if (maxy < c.imag()) maxy = c.imag();
        if (miny > c.imag()) miny = c.imag();
    }

    double dx = maxx - minx;
    double dy = maxy - miny;
    double dz = maxz - minz;
    minx -= dx;
    maxx += dx;
    miny -= dy;
    maxy += dy;

    if (minx == maxx)
    {
        minx = -2;
        maxx = 0.5;
        dx = maxx - minx;
    }
    if (miny == maxy)
    {
        miny = -2;
        maxy = 2;
        dy = maxy - miny;
    }

    double largestAxis = dx;
    if (dy > largestAxis) largestAxis = dy;
    if (dz > largestAxis) largestAxis = dz;*/

    MagnitudeFunction func(system_);
    func.setMesh(100, 100);
    func.setDomain(minx, maxx, miny, maxy);
    func.setMinZ(minz);
    func.setMaxZ(maxz);
    func.create(*plot_);
}
