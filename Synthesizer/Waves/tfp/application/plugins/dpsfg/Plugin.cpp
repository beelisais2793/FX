#include "view/DPSFG.hpp"
#include "tfp/plugin/Plugin.hpp"

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    return plugin->registerTool<DPSFG>(
        tfp::Plugin::GENERATOR,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "DPSFG",
        "Alex Murray",
        "Draw driving-point signal flow graphs and compute their transfer functions",
        "alex.murray@gmx.ch"
    );
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
