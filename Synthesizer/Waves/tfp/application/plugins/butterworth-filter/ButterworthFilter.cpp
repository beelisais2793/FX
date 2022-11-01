#include "ButterworthFilter.hpp"
#include "tfp/views/FloatAdjustmentWidget.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>

using namespace tfp;

// ----------------------------------------------------------------------------
ButterworthFilter::ButterworthFilter(QWidget* parent) :
    Tool(parent),
    n_(5), k_(1.0), wc_(1.0)
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

    QLabel* infoBox = new QLabel;
    infoBox->setText(
        "The Butterworth filter is designed to have as flat a frequency response as possible in the passband."
    );
    infoBox->setWordWrap(true);
    layout->addWidget(infoBox);

    connect(nadjSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setOrder(int)));
    connect(kadj, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
    connect(wcadj, SIGNAL(valueChanged(double)), this, SLOT(setCutoffFrequency(double)));
}

// ----------------------------------------------------------------------------
void ButterworthFilter::updateParameters()
{
    double factor = k_;
    for (int k = 0; k < n_; ++k)
    {
        system_->denominator().setRoot(k, wc_ * std::exp(Type<double>::Complex(0, ((2*k+n_+1)*M_PI) / (2*n_))));
        factor *= wc_;
    }
    system_->numerator().setFactor(factor);

    system_->notifyParametersChanged();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::setOrder(int n)
{
    n_ = n;
    system_->denominator().resize(n_);
    system_->notifyStructureChanged();
    updateParameters();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::setScale(double k)
{
    k_ = k;
    updateParameters();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::setCutoffFrequency(double wc)
{
    wc_ = wc;
    updateParameters();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::onSetSystem()
{
    if (system_ == NULL)
        return;

    system_->numerator().resize(0);
    setOrder(n_);
    updateParameters();
}
