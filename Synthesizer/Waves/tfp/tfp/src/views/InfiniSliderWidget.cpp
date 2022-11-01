#include "tfp/views/InfiniSliderWidget.hpp"
#include "tfp/math/Math.hpp"

#include <QMouseEvent>

#include <cmath>


namespace tfp {

// ----------------------------------------------------------------------------
InfiniSliderWidget::InfiniSliderWidget(QWidget* parent) :
    QSlider(Qt::Horizontal, parent),
    isLogarithmic_(true)
{
    setValue(1.0);
}

// ----------------------------------------------------------------------------
void InfiniSliderWidget::setValue(double value)
{
    value_ = value;

    if(isLogarithmic_)
    {
        isNegative_ = false;
        if(value_ < 0.0)
        {
            isNegative_ = true;
            value_ = -value_;
        }

        if(value_ != 0.0)
        {
            double lower = std::log10(value_ / 2);
            double upper = std::log10(value_ * 2);
            double current = std::log10(value_);

            if(fpu_error(lower) || fpu_error(upper))
                return;

            scale_ = 100.0 / (upper - lower);

            QSlider::setRange(int(lower*scale_), int(upper*scale_));
            QSlider::setValue(int(current*scale_));
        }
        else
        {
            scale_ = 100;
            QSlider::setRange(int(-50), int(50));
            QSlider::setValue(int(0));
        }
    }
    else
    {
        if(value_ != 0.0)
        {
            double offset = std::abs(value);
            double lower = value - offset;
            double upper = value + offset;

            isNegative_ = false;
            scale_ = 100.0 / (upper - lower);

            QSlider::setRange(int(lower*scale_), int(upper*scale_));
            QSlider::setValue(int(value*scale_));
        }
        else
        {
            scale_ = 1;
            QSlider::setRange(int(-50), int(50));
            QSlider::setValue(int(0));
        }
    }

    emit valueChanged(value);
}

// ----------------------------------------------------------------------------
void InfiniSliderWidget::setLogarithmic(bool flag)
{
    isLogarithmic_ = flag;
}

// ----------------------------------------------------------------------------
void InfiniSliderWidget::mouseMoveEvent(QMouseEvent* ev)
{
    // Doesn't take into account width of the sliding knob
    int newPosition = ev->pos().x() * (maximum() - minimum()) / geometry().width() + minimum();

    double newValue;
    if(isLogarithmic_)
        newValue = std::pow(10, newPosition/scale_);
    else
        newValue = newPosition/scale_;

    if(!fpu_error(newValue))
    {
        if(value_ == 0 && isLogarithmic_ && newValue < 1.0)
            isNegative_ = true;

        value_ = newValue;

        if(isNegative_)
            emit valueChanged(-value_);
        else
            emit valueChanged(value_);
    }

    QSlider::mouseMoveEvent(ev);
}

// ----------------------------------------------------------------------------
void InfiniSliderWidget::mousePressEvent(QMouseEvent* ev)
{
    QSlider::mousePressEvent(ev);
}

// ----------------------------------------------------------------------------
void InfiniSliderWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    if(isNegative_)
        setValue(-value_);
    else
        setValue(value_);
    QSlider::mouseReleaseEvent(ev);
}

} // namespace tfp
