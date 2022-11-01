#include "tfp/plot/UnitTracker.hpp"
#include "tfp/util/Parameter.hpp"


namespace tfp {

// ----------------------------------------------------------------------------
UnitTracker::UnitTracker(QWidget* canvas) :
    QwtPlotPicker(canvas)
{
    setTrackerMode(AlwaysOn);
}

// ----------------------------------------------------------------------------
void UnitTracker::setXUnit(QString unit)
{
    unitX_   = unit;
}

// ----------------------------------------------------------------------------
void UnitTracker::setYUnit(QString unit)
{
    unitY_   = unit;
}

// ----------------------------------------------------------------------------
QwtText UnitTracker::trackerTextF(const QPointF &pos) const
{
    QString text;

    switch (rubberBand())
    {
        case HLineRubberBand:
            text = QString(ParameterD(pos.y(), Metric::UNITY, unitY_.toStdString()).toHumanReadable(4).c_str());
            break;
        case VLineRubberBand:
            text = QString(ParameterD(pos.x(), Metric::UNITY, unitX_.toStdString()).toHumanReadable(4).c_str());
            break;
        default:
            text  = QString(ParameterD(pos.x(), Metric::UNITY, unitX_.toStdString()).toHumanReadable(4).c_str());
            text += ", ";
            text += QString(ParameterD(pos.y(), Metric::UNITY, unitY_.toStdString()).toHumanReadable(4).c_str());
    }

    return QwtText(text);
}

} // namespace tfp
