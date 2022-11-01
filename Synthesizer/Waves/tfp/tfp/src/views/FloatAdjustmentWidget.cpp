#include "tfp/views/FloatAdjustmentWidget.hpp"
#include "tfp/views/InfiniSliderWidget.hpp"
#include <QLineEdit>
#include <QSlider>
#include <QLayout>
#include <QLabel>
#include <cmath>

namespace tfp {

// ----------------------------------------------------------------------------
FloatAdjustmentWidget::FloatAdjustmentWidget(QWidget* parent) :
    AdjustmentWidget(parent),
    lineEditValue_(NULL),
    slider_(NULL)
{
    slider_ = new InfiniSliderWidget;
    slider_->setLogarithmic(true);
    layout()->addWidget(slider_);

    lineEditValue_ = new QLineEdit;
    lineEditValue_->setValidator(new QDoubleValidator(this));
    layout()->addWidget(lineEditValue_);

    connect(slider_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    connect(lineEditValue_, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
}

// ----------------------------------------------------------------------------
void FloatAdjustmentWidget::setValue(double value)
{
    QString number = QString::number(value);
    lineEditValue_->setText(number);
    slider_->setValue(value);
}

// ----------------------------------------------------------------------------
void FloatAdjustmentWidget::onValueChanged(double value)
{
    QString number = QString::number(value);
    lineEditValue_->setText(number);
    emit valueChanged(value);
}

// ----------------------------------------------------------------------------
void FloatAdjustmentWidget::onTextEdited(const QString& text)
{
    bool ok;
    double value = text.toDouble(&ok);
    if(!ok)
        return;

    slider_->setValue(value);
}

} // namespace tfp
