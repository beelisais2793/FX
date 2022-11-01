#include "PoleZeroPlot.hpp"
#include "tfp/plugin/Plugin.hpp"

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    return plugin->registerTool<tfp::PoleZeroPlot>(
        tfp::Plugin::VISUALISER,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "Pole Zero Plot",
        "Alex Murray",
        "Plot the poles and zeros of a transfer function",
        "alex.murray@gmx.ch"
    );
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
