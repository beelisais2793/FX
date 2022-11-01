#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::Noise
{

    struct BrownNoise : public GeneratorProcessorPlugin<gam::NoiseBrown>
    {
        
    };

    struct PinkNoise : public GeneratorProcessorPlugin<gam::PinkBrown>
    {
        
    };

    struct WhiteNoise : public GeneratorProcessorPlugin<gam::NoiseWhite>
    {
        
    };

    struct VioletNoise : public GeneratorProcessorPlugin<gam::NoiseViolet>
    {
        
    };

    struct BinaryNoise : public GeneratorProcessorPlugin<gam::NoiseBinary>
    {
        
    };
}