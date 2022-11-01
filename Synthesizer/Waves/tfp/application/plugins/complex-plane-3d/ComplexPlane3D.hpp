#pragma once

#include "tfp/plugin/Tool.hpp"

namespace Qwt3D {
    class GridPlot;
}

namespace tfp {

class ComplexPlane3D : public Tool
{
public:
    explicit ComplexPlane3D(QWidget* parent=NULL);

    virtual void replot() OVERRIDE;
    virtual void autoScale() OVERRIDE;

protected:
    virtual void onSetSystem() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE;
    virtual void onSystemStructureChanged() OVERRIDE;

private:
    void normaliseAxes();

private:
    Qwt3D::GridPlot* plot_;
};

}
