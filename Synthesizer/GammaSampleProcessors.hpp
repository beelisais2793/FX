// sample
// sample queue push_front pop_back
// SoundFile.h

#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::Sample
{
    struct SamplePlayer : public GeneratorProcessorPlugin<gam::SamplePlayer<float>>
    {
        SamplePlayer() : GeneratorProcessorPlugin<gam::SamplePlayer<float>>()
        {
            
        }
    };
}