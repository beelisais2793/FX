#include "qwt_plot_picker.h"

class QWidget;


namespace tfp {


class UnitTracker : public QwtPlotPicker
{
public:
    explicit UnitTracker(QWidget* canvas);

    void setXUnit(QString unit);
    void setYUnit(QString unit);

private:
    virtual QwtText trackerTextF(const QPointF &pos) const OVERRIDE;

    QString unitX_;
    QString unitY_;
};

} // namespace tfp
