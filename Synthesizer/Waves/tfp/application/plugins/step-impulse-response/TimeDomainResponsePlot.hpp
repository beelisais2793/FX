#pragma once

#include "tfp/config.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/math/TransferFunction.hxx"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class TimeDomainResponsePlot : public Tool
{
public:
    explicit TimeDomainResponsePlot(QWidget* parent=NULL);

    virtual void replot() OVERRIDE;
    virtual void autoScale() OVERRIDE;

protected:
    virtual TransferFunction<double>::PFEResultData doPartialFractionExpansion() = 0;
    virtual void onSetSystem() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE;
    virtual void onSystemStructureChanged() OVERRIDE;

private:
    RealtimePlot* plot_;
    QwtPlotCurve* curve_;
};

}
