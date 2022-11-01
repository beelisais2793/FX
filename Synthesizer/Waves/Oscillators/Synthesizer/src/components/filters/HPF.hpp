#include "Filter.hpp"


class HPF : public Filter
{
public:
    Sample Apply(Sample a) override;
};