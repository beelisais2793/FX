#pragma once

#include "tfp/config.hpp"

namespace tfp {

class TFP_PUBLIC_API PluginListener
{
public:
    virtual void onPluginUnloading() = 0;
};

}
