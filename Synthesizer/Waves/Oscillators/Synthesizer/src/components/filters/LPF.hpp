#include "Filter.hpp"


class LPF : public Filter
{
public:
    Sample Apply(Sample a) override;
};