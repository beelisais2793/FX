#pragma once

#include "tfp/config.hpp"
#include <QWidget>

class QLabel;

namespace tfp {

class TFP_PUBLIC_API AdjustmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdjustmentWidget(QWidget* parent = 0);

    void setName(const QString& name);

signals:
    void valueChanged(bool value);
    void valueChanged(long value);
    void valueChanged(double value);
    void valueChanged(const QString& value);

protected:
    QLabel* labelName_;
};

} // namespace tfp
