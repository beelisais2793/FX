#include "DPSFG.hpp"
#include "DPSFGView.hpp"
#include "DPSFGScene.hpp"
#include <QHBoxLayout>

// ----------------------------------------------------------------------------
DPSFG::DPSFG(QWidget* parent) :
    Tool(parent),
    dpsfgView_(new DPSFGView(new DPSFGScene))
{
    setLayout(new QHBoxLayout);
    layout()->addWidget(dpsfgView_);
}

// ----------------------------------------------------------------------------
void DPSFG::onSetSystem()
{
}
