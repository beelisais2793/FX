#pragma once

#include "tfp/plugin/Tool.hpp"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class BodePlot : public Tool
{
public:
    explicit BodePlot(QWidget* parent=NULL);

    virtual void replot() OVERRIDE;
    virtual void autoScale() OVERRIDE;

protected:
    virtual void onSetSystem() OVERRIDE {}
    virtual void onSystemStructureChanged() OVERRIDE;
    virtual void onSystemParametersChanged() OVERRIDE;

protected:
    RealtimePlot* ampPlot_;
    RealtimePlot* phasePlot_;
    QwtPlotCurve* amplitude_;
    QwtPlotCurve* phase_;
};

}
