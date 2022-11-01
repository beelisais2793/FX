#pragma once

#include "tfp/config.hpp"

namespace tfp {

class TFP_PUBLIC_API SystemListener
{
public:
    virtual void onSystemStructureChanged() = 0;
    virtual void onSystemParametersChanged() = 0;
};

}
