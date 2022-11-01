#pragma once

#include "tfp/plugin/Tool.hpp"

class DPSFGView;

class DPSFG : public tfp::Tool
{
public:
    explicit DPSFG(QWidget* parent=NULL);

protected:
    virtual void onSetSystem() OVERRIDE;
    virtual void onSystemStructureChanged() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE {}
    virtual void replot() OVERRIDE {}
    virtual void autoScale() OVERRIDE {}

private:
    DPSFGView* dpsfgView_;
};
