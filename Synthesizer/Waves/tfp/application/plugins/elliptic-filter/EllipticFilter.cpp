#include "EllipticFilter.hpp"
#include "tfp/views/FloatAdjustmentWidget.hpp"
#include "tfp/math/Math.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <complex>

using namespace tfp;

// ----------------------------------------------------------------------------
EllipticFilter::EllipticFilter(QWidget* parent) :
    Tool(parent),
    n_(5), k_(1.0), wc_(1.0), epsilon_(0.1)
{
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QWidget* nadj = new QWidget;
    nadj->setLayout(new QHBoxLayout);
    nadj->layout()->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(nadj);

    QLabel* nadjLabel = new QLabel;
    nadjLabel->setText("Order");
    nadj->layout()->addWidget(nadjLabel);

    QSpinBox* nadjSpinBox = new QSpinBox;
    nadjSpinBox->setRange(1, 100);
    nadjSpinBox->setValue(n_);
    nadj->layout()->addWidget(nadjSpinBox);

    FloatAdjustmentWidget* kadj = new FloatAdjustmentWidget;
    kadj->setName("k");
    kadj->setValue(k_);
    layout->addWidget(kadj);

    FloatAdjustmentWidget* wcadj = new FloatAdjustmentWidget;
    wcadj->setName("wc");
    wcadj->setValue(wc_);
    layout->addWidget(wcadj);

    FloatAdjustmentWidget* epsilonadj = new FloatAdjustmentWidget;
    epsilonadj->setName("epsilon");
    epsilonadj->setValue(epsilon_);
    layout->addWidget(epsilonadj);

    QLabel* infoBox = new QLabel;
    infoBox->setText(
        "An elliptic filter has equalized ripple (equiripple) behavior in both the passband and the stopband. The amount of ripple in each band is independently adjustable, and no other filter of equal order can have a faster transition in gain between the passband and the stopband, for the given values of ripple. Alternatively, one may give up the ability to adjust independently the passband and stopband ripple, and instead design a filter which is maximally insensitive to component variations.\n\n"

        "As the ripple in the stopband approaches zero, the filter becomes a type I Chebyshev filter. As the ripple in the passband approaches zero, the filter becomes a type II Chebyshev filter and finally, as both ripple values approach zero, the filter becomes a Butterworth filter."
    );
    infoBox->setWordWrap(true);
    infoBox->setMaximumWidth(400);
    layout->addWidget(infoBox);

    connect(nadjSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setOrder(int)));
    connect(kadj, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
    connect(wcadj, SIGNAL(valueChanged(double)), this, SLOT(setCutoffFrequency(double)));
    connect(epsilonadj, SIGNAL(valueChanged(double)), this, SLOT(setEpsilon(double)));
}

// ----------------------------------------------------------------------------
void EllipticFilter::updateParameters()
{
    double reciFactor = 0.5;
    double a = -SINH(1.0/n_ * ASINH(1.0/epsilon_));
    double b = COSH(1.0/n_ * ASINH(1.0/epsilon_));
    for (int m = 0; m < n_; ++m)
    {
        system_->denominator().setRoot(m, wc_ * Type<double>::Complex(
            a * std::sin(M_PI / 2.0 * (2.0*m+1)/n_),
            b * std::cos(M_PI / 2.0 * (2.0*m+1)/n_)
        ));
        reciFactor *= 2 / wc_;
    }
    system_->numerator().setFactor(k_ / (reciFactor * epsilon_));

    system_->notifyParametersChanged();
}

// ----------------------------------------------------------------------------
void EllipticFilter::setOrder(int n)
{
    n_ = n;
    system_->denominator().resize(n_);
    system_->notifyStructureChanged();
    updateParameters();
}

// ----------------------------------------------------------------------------
void EllipticFilter::setScale(double k)
{
    k_ = k;
    updateParameters();
}

// ----------------------------------------------------------------------------
void EllipticFilter::setCutoffFrequency(double wc)
{
    wc_ = wc;
    updateParameters();
}

// ----------------------------------------------------------------------------
void EllipticFilter::setEpsilon(double epsilon)
{
    epsilon_ = epsilon;
    updateParameters();
}

// ----------------------------------------------------------------------------
void EllipticFilter::onSetSystem()
{
    if (system_ == NULL)
        return;

    system_->numerator().resize(0);
    setOrder(n_);
    updateParameters();
}
