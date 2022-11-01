#pragma once

#include "tfp/plugin/Tool.hpp"
#include <QVector>

class QwtPlotShapeItem;
class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class PoleZeroPlot : public Tool
{
public:
    explicit PoleZeroPlot(QWidget* parent=NULL);

    virtual void autoScale() OVERRIDE;
    virtual void replot() OVERRIDE;

protected:
    virtual void onSetSystem() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE;
    virtual void onSystemStructureChanged() OVERRIDE;

private:
    void onSystemChanged();

private:
    RealtimePlot* plot_;
    QVector<QwtPlotShapeItem*> poles_;
    QVector<QwtPlotShapeItem*> zeros_;
};

}
