#include "ComplexPlane3D.hpp"
#include "tfp/plugin/Plugin.hpp"

using namespace tfp;

extern "C" {

bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree)
{
    return plugin->registerTool<tfp::ComplexPlane3D>(
        tfp::Plugin::VISUALISER,
        tfp::Plugin::LTI_SYSTEM_CONTINUOUS,
        "Complex Plane 3D",
        "Alex Murray",
        "Plot the poles and zeros of a transfer function in 3D",
        "alex.murray@gmx.ch"
    );
}

void stop_plugin(tfp::Plugin* plugin)
{
}

}
