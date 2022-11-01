#include "TimeDomainResponsePlot.hpp"
#include "tfp/views/RealtimePlot.hpp"
#include "tfp/math/TransferFunction.hpp"
#include "tfp/models/System.hpp"
#include <qwt_plot_curve.h>
#include <QVBoxLayout>

namespace tfp {

// ----------------------------------------------------------------------------
TimeDomainResponsePlot::TimeDomainResponsePlot(QWidget* parent) :
    Tool(parent),
    plot_(new RealtimePlot),
    curve_(new QwtPlotCurve)
{
    setLayout(new QVBoxLayout);

    plot_->enableRectangleZoom();
    layout()->addWidget(plot_);

    curve_->attach(plot_);
}

// ----------------------------------------------------------------------------
void TimeDomainResponsePlot::replot()
{
    plot_->replot();
}

// ----------------------------------------------------------------------------
void TimeDomainResponsePlot::autoScale()
{
    plot_->autoScale();
}

// ----------------------------------------------------------------------------
void TimeDomainResponsePlot::onSystemParametersChanged()
{
    double xData[1000];
    double yData[1000];
    double begin = 0;
    double end = 50;

    system_->interestingTimeInterval(&begin, &end);
    system_->inverseLaplaceTransform(doPartialFractionExpansion(), xData, yData, begin, end, 1000);
    curve_->setSamples(xData, yData, 1000);

    replot();
    if (plot_->lastScaleWasAutomatic())
        autoScale();
}

// ----------------------------------------------------------------------------
void TimeDomainResponsePlot::onSystemStructureChanged()
{
    autoScale();
}

}
