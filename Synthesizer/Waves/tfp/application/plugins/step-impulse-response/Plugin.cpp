#include "ImpulsePlot.hpp"
#include "StepPlot.hpp"
#include "tfp/plugin/Plugin.hpp"

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    if (plugin->registerTool<tfp::ImpulsePlot>(
        tfp::Plugin::VISUALISER,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "Impulse Response",
        "Alex Murray",
        "Calculate the impulse response of a transfer function.",
        "alex.murray@gmx.ch"

    ) == false) return false;

    if (plugin->registerTool<tfp::StepPlot>(
        tfp::Plugin::VISUALISER,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "Step Response",
        "Alex Murray",
        "Calculate the step response of a transfer function.",
        "alex.murray@gmx.ch"
    ) == false) return false;
    return true;
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
