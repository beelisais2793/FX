#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::Oscillators
{

    struct Sweep : public GeneratorProcessorPlugin<gam::Sweep>
    {

    };

    struct WaveTable : public GeneratorProcessorPlugin<gam::Osc<double>>
    {

    };

    struct CSine : public GeneratorProcessorPlugin<gam::CSine<double>>
    {

    };

    struct Sine : public GeneratorProcessorPlugin<gam::Sine<double>>
    {

    };

    struct SineR : public GeneratorProcessorPlugin<gam::SineR<double>>
    {

    };

    struct SineRs : public GeneratorProcessorPlugin<gam::SineRs<double>>
    {

    };

    struct SineD : public GeneratorProcessorPlugin<gam::SineD<double>>
    {

    };

    struct SineDs : public GeneratorProcessorPlugin<gam::SineDs<double>>
    {

    };

    struct Chirplet : public GeneratorProcessorPlugin<gam::Chriplet<double>>
    {

    };

    struct LFO : public GeneratorProcessorPlugin<gam::LFO>
    {

    };

    struct DWO : public GeneratorProcessorPlugin<gam::DWO>
    {

    };

    struct Buzz : public GeneratorProcessorPlugin<gam::Buzz<double>>
    {

    };

    struct Impulse : public GeneratorProcessorPlugin<gam::Impulse<double>>
    {

    };

    struct Saw : public GeneratorProcessorPlugin<gam::Saw<double>>
    {

    };

    struct Square : public GeneratorProcessorPlugin<gam::Square<double>>
    {

    };

    struct DSF : public GeneratorProcessorPlugin<gam::DSF<double>>
    {

    };

    struct Upsample : public GeneratorProcessorPlugin<gam::Upsample>
    {

    };

    struct Ramped : public GeneratorProcessorPlugin<gam::Ramped<double>>
    {

    };
}
