#include "tfp/plugin/Tool.hpp"
#include "tfp/models/System.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
Tool::Tool(QWidget* parent) :
    QWidget(parent)
{
}

// ----------------------------------------------------------------------------
Tool::~Tool()
{
    if (system_ != NULL)
        system_->dispatcher.removeListener(this);
}

// ----------------------------------------------------------------------------
void Tool::setSystem(System* system)
{
    if (system_ != NULL)
    {
        system_->dispatcher.removeListener(this);
    }

    system_ = system;
    onSetSystem();

    if (system_ != NULL)
    {
        system_->dispatcher.addListener(this);
        onSystemStructureChanged();
        onSystemParametersChanged();
    }
}

}
