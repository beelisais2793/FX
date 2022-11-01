#include "StandardLowOrderFilter.hpp"
#include "tfp/plugin/Plugin.hpp"

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    return plugin->registerTool<tfp::StandardLowOrderFilter>(
        tfp::Plugin::GENERATOR,
        tfp::Plugin::LTI_SYSTEM_DISCRETE,
        "Standard Low Order Filters",
        "Alex Murray",
        "Calculate the transfer functions of standard 1st and 2nd order filters",
        "alex.murray@gmx.ch"
    );
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
