#pragma once

#include "tfp/config.hpp"
#include "tfp/views/AdjustmentWidget.hpp"

class QLineEdit;

namespace tfp {

class InfiniSliderWidget;

class TFP_PUBLIC_API FloatAdjustmentWidget : public AdjustmentWidget
{
    Q_OBJECT

public:
    explicit FloatAdjustmentWidget(QWidget* parent = 0);

    void setValue(double value);

private slots:
    void onValueChanged(double value);
    void onTextEdited(const QString& text);

protected:
    QLineEdit* lineEditValue_;
    InfiniSliderWidget* slider_;
};

} // namespace tfp
