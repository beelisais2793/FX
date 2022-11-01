#include "PoleZeroPlot.hpp"
#include "tfp/views/RealtimePlot.hpp"
#include "tfp/models/System.hpp"
#include "tfp/math/TransferFunction.hpp"
#include "tfp/math/Math.hpp"
#include <QVBoxLayout>
#include <QPainterPath>
#include <qwt_plot_shapeitem.h>
#include <qwt_plot_curve.h>
#include <QtCore/QtGlobal>

using namespace tfp;

// ----------------------------------------------------------------------------
PoleZeroPlot::PoleZeroPlot(QWidget* parent) :
    Tool(parent),
    plot_(new RealtimePlot)
{
    plot_->enableRectangleZoom();
    setLayout(new QVBoxLayout);
    layout()->addWidget(plot_);

    QwtPlotShapeItem* unitCircle = new QwtPlotShapeItem;
    QPainterPath unitCirclePath;
    unitCirclePath.lineTo(-2, 0);
    unitCirclePath.moveTo(0, 2);
    unitCirclePath.lineTo(0, -2);
    unitCirclePath.moveTo(0, 1);
    unitCirclePath.arcTo(-1, -1, 2, 2, 90, 180);
    QPen pen;
    pen.setColor(QColor(140, 140, 140));
    pen.setStyle(Qt::DashLine);
    unitCircle->setShape(unitCirclePath);
    unitCircle->setPen(pen);
    unitCircle->attach(plot_);
}

// ----------------------------------------------------------------------------
void PoleZeroPlot::autoScale()
{
    plot_->autoScale();
}

// ----------------------------------------------------------------------------
void PoleZeroPlot::replot()
{
    plot_->replot();
}

// ----------------------------------------------------------------------------
void PoleZeroPlot::onSystemParametersChanged()
{
    onSystemChanged();
}

// ----------------------------------------------------------------------------
void PoleZeroPlot::onSystemStructureChanged()
{
    while (system_->numerator().size() < zeros_.size())
    {
        zeros_.last()->detach();
        delete zeros_.last();
        zeros_.pop_back();
    }
    while (system_->numerator().size() > zeros_.size())
    {
        zeros_.push_back(new QwtPlotShapeItem);
        zeros_.back()->attach(plot_);
    }
    while (system_->denominator().size() < poles_.size())
    {
        poles_.last()->detach();
        delete poles_.last();
        poles_.pop_back();
    }
    while (system_->denominator().size() > poles_.size())
    {
        poles_.push_back(new QwtPlotShapeItem);
        poles_.back()->attach(plot_);
    }

    autoScale();
}

// ----------------------------------------------------------------------------
void PoleZeroPlot::onSystemChanged()
{
    const double radius = 0.02;

    for (int i = 0; i < system_->numerator().size(); ++i)
    {
        const Type<double>::Complex& root = system_->numerator().root(i);

        QPainterPath path;
        path.addEllipse(root.real()-radius, root.imag()-radius, radius*2, radius*2);
        zeros_[i]->setShape(path);
    }

    for (int i = 0; i < system_->denominator().size(); ++i)
    {
        const Type<double>::Complex& root = system_->denominator().root(i);

        QPainterPath path;
        path.moveTo(root.real()-radius, root.imag()+radius);
        path.lineTo(root.real()+radius, root.imag()-radius);
        path.moveTo(root.real()-radius, root.imag()-radius);
        path.lineTo(root.real()+radius, root.imag()+radius);
        poles_[i]->setShape(path);
    }

    replot();
    if (plot_->lastScaleWasAutomatic())
        autoScale();
}
