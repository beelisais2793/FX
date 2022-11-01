#pragma once

#include "tfp/plugin/Tool.hpp"

class QSpinBox;

namespace tfp {

class FloatAdjustmentWidget;

class ChebyshevFilter : public Tool
{
    Q_OBJECT

public:
    explicit ChebyshevFilter(QWidget* parent=NULL);

public slots:
    void setOrder(int n);
    void setScale(double k);
    void setCutoffFrequency(double wc);
    void setEpsilon(double epsilon);

protected:
    virtual void onSetSystem() OVERRIDE;
    virtual void onSystemStructureChanged() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE {}
    virtual void replot() OVERRIDE {}
    virtual void autoScale() OVERRIDE {}

private:
    void updateParameters();

private:
    double n_, k_, wc_, epsilon_;
};

}
