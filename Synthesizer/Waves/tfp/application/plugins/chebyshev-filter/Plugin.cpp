#include "ChebyshevFilter.hpp"
#include "tfp/plugin/Plugin.hpp"

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    return plugin->registerTool<tfp::ChebyshevFilter>(
        tfp::Plugin::GENERATOR,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "Chebyshev Filter",
        "Alex Murray",
        "Generate the transfer function of a Chebyshev filter.",
        "alex.murray@gmx.ch"
    );
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
