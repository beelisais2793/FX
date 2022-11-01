#include "EllipticFilter.hpp"
#include "tfp/plugin/Plugin.hpp"

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    return plugin->registerTool<tfp::EllipticFilter>(
        tfp::Plugin::GENERATOR,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "Elliptic Filter",
        "Alex Murray",
        "Design an elliptic filter and compute its transfer function",
        "alex.murray@gmx.ch"
    );
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
