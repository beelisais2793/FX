#include "StandardLowOrderFilter.hpp"
#include "tfp/views/FloatAdjustmentWidget.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <QComboBox>

using namespace tfp;

// ----------------------------------------------------------------------------
StandardLowOrderFilter::StandardLowOrderFilter(QWidget* parent) :
    Tool(parent),
    qpadj_(new FloatAdjustmentWidget),
    filterType_(LOWPASS_1),
    k_(1.0), wp_(1.0), qp_(1.0)
{
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QComboBox* filters = new QComboBox;
    filters->addItem("Lowpass 1st Order");
    filters->addItem("Highpass 1st Order");
    filters->addItem("Lowpass 2nd Order");
    filters->addItem("Bandpass");
    filters->addItem("Highpass 2nd Order");
    layout->addWidget(filters);

    FloatAdjustmentWidget* kadj = new FloatAdjustmentWidget;
    kadj->setName("k");
    kadj->setValue(1);
    layout->addWidget(kadj);

    FloatAdjustmentWidget* wpadj = new FloatAdjustmentWidget;
    wpadj->setName("wp");
    wpadj->setValue(1);
    layout->addWidget(wpadj);

    qpadj_->setName("qp");
    qpadj_->setValue(1);
    layout->addWidget(qpadj_);

    connect(kadj, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
    connect(wpadj, SIGNAL(valueChanged(double)), this, SLOT(setPoleFrequency(double)));
    connect(qpadj_, SIGNAL(valueChanged(double)), this, SLOT(setQualityFactor(double)));
    connect(filters, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterType(int)));
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::updateParameters()
{
    // Handle poles first
    switch (filterType_)
    {
        /*
         *          ...
         * T(s) = --------
         *         s + wp
         */
        case LOWPASS_1:
        case HIGHPASS_1:
            system_->denominator().setRoot(0, -wp_);
            break;

        /*
         *                ...
         * T(s) = --------------------
         *        s^2 + wp/qp*s + wp^2
         */
        case LOWPASS_2:
        case BANDPASS:
        case HIGHPASS_2:
        {

            // Real part
            Type<double>::Complex r1(-wp_ / (2.0 * qp_), 0);
            Type<double>::Complex r2(-wp_ / (2.0 * qp_), 0);

            // Roots are complex conjugates if qp > 0.5
            if (qp_ > 0.5)
            {
                r1.imag(+wp_ * std::sqrt(1.0 - 1.0/(4.0*qp_*qp_)));
                r2.imag(-wp_ * std::sqrt(1.0 - 1.0/(4.0*qp_*qp_)));
            }
            else
            {
                r1 += wp_ * std::sqrt(1.0/(4.0*qp_*qp_) - 1.0);
                r2 -= wp_ * std::sqrt(1.0/(4.0*qp_*qp_) - 1.0);
            }

            system_->denominator().setRoot(0, r1);
            system_->denominator().setRoot(1, r2);

            break;
        }

        case FILTER_COUNT:
            break;
    }

    // Handle zeros
    switch (filterType_)
    {
        case HIGHPASS_2:                         // k*s^2
            system_->numerator().setRoot(1, 0);
        case HIGHPASS_1:                         // k*s
            system_->numerator().setRoot(0, 0);
            system_->numerator().setFactor(k_);
            break;

        case LOWPASS_2:                          // k*wp^2
            system_->numerator().setFactor(k_ * wp_ * wp_);
            break;

        case BANDPASS:                           // k*s*wp
            system_->numerator().setRoot(0, 0);
        case LOWPASS_1:                          // k*wp
            system_->numerator().setFactor(k_ * wp_);
            break;

        case FILTER_COUNT:
            break;
    }

    system_->notifyParametersChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setScale(double k)
{
    k_ = k;
    updateParameters();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setPoleFrequency(double wp)
{
    wp_ = wp;
    updateParameters();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setQualityFactor(double qp)
{
    qp_ = qp;
    updateParameters();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setFilterType(FilterType filterType)
{
    filterType_ = filterType;

    switch (filterType_)
    {
        case LOWPASS_1:
            qpadj_->setEnabled(false);
            system_->numerator().resize(0);
            system_->denominator().resize(1);
            break;

        case HIGHPASS_1:
            qpadj_->setEnabled(false);
            system_->numerator().resize(1);
            system_->denominator().resize(1);
            break;

        case LOWPASS_2:
            qpadj_->setEnabled(true);
            system_->numerator().resize(0);
            system_->denominator().resize(2);
            break;

        case BANDPASS:
            qpadj_->setEnabled(true);
            system_->numerator().resize(1);
            system_->denominator().resize(2);
            break;

        case HIGHPASS_2:
            qpadj_->setEnabled(true);
            system_->numerator().resize(2);
            system_->denominator().resize(2);
            break;

        default:
            std::cerr << "Invalid filter type " << filterType_ << std::endl;
            break;
    }

    system_->notifyStructureChanged();
    updateParameters();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setFilterType(int index)
{
    setFilterType(FilterType(index));
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::onSetSystem()
{
    if (system_ == NULL)
        return;

    setFilterType(filterType_);
}
