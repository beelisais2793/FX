#pragma once

#include "tfp/config.hpp"
#include <qwt3d_gridplot.h>

namespace Qwt3D {
    class Function;
}

namespace tfp {

class TFP_PUBLIC_API RealtimePlot3D : public Qwt3D::GridPlot
{
public:
    explicit RealtimePlot3D(QWidget* parent=NULL);
    void efficientlyUpdateData(Qwt3D::Function* function);
};

}
