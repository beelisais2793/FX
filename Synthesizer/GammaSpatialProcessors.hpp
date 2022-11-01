#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::Spatial
{

    struct LoopGain : public FunctionProcessorPlugin<gam::LoopGain<double>>
    {

    };

    struct Loop1P : public FunctionProcessorPlugin<gam::Loop1P<double>>
    {

    };

    struct Loop1P1Z : public FunctionProcessorPlugin<gam::Loop1P1Z<double>>
    {

    };

    struct Echo : public FunctionProcessorPlugin<gam::Echo<double>>
    {

    };
    struct EchoCSine : public FunctionProcessorPlugin<gam::EchoCSine<double>>
    {

    };    
    struct ReverbMS : public FunctionProcessorPlugin<gam::ReverbMS<double>>
    {

    };
    struct Dist : public FunctionProcessorPlugin<gam::Dist<2,double>>
    {

    };
    
}
