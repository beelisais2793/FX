#include "BodePlot.hpp"
#include "tfp/plugin/Plugin.hpp"

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    return plugin->registerTool<tfp::BodePlot>(
        tfp::Plugin::VISUALISER,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "Bode Plot",
        "Alex Murray",
        "Visualise the amplitude and phase of a system.",
        "alex.murray@gmx.ch"
    );
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
