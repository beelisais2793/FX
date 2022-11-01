#pragma once

#include "TimeDomainResponsePlot.hpp"

namespace tfp {

class ImpulsePlot : public TimeDomainResponsePlot
{
public:
    explicit ImpulsePlot(QWidget* parent=NULL);

protected:
    virtual TransferFunction<double>::PFEResultData doPartialFractionExpansion() OVERRIDE;
};

}
