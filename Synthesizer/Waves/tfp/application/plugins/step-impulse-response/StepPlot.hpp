#pragma once

#include "TimeDomainResponsePlot.hpp"

namespace tfp {

class StepPlot : public TimeDomainResponsePlot
{
public:
    explicit StepPlot(QWidget* parent=NULL);

protected:
    virtual TransferFunction<double>::PFEResultData doPartialFractionExpansion() OVERRIDE;
};

}
