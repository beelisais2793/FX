#include "ImpulsePlot.hpp"
#include "tfp/models/System.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
ImpulsePlot::ImpulsePlot(QWidget* parent) :
    TimeDomainResponsePlot(parent)
{
}

// ----------------------------------------------------------------------------
TransferFunction<double>::PFEResultData ImpulsePlot::doPartialFractionExpansion()
{
    return system_->partialFractionExpansion(0);
}

}
