#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_pink = fv3::noisegen_pink_frac_;        
    using fv3_white1 = fv3::noisegen_gaussian_white_noise_1_;
    using fv3_white3 = fv3::noisegen_gaussian_white_noise_3_;
    #else
    using fv3_pink = fv3::noisegen_pink_frac_f;
    using fv3_white1 = fv3::noisegen_gaussian_white_noise_1_f;
    using fv3_white3 = fv3::noisegen_gaussian_white_noise_3_f;
    #endif
    struct PinkNoise : public GeneratorProcessorPlugin<fv3_pink>
    {
        
        PinkNoise() : GeneratorProcessorPlugin<fv3_pink>()
        {        
            
        }
        
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {            
            return A*this->process();
        }
    };      

    struct GaussianWhiteNoise1 : public GeneratorProcessorPlugin<fv3_white1>
    {
        
        GaussianWhiteNoise1() : GeneratorProcessorPlugin<fv3_white1>()
        {        
            
        }
        
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {            
            return A*this->process();
        }
    };      
    /*
    struct GaussianWhiteNoise2 : public GeneratorProcessorPlugin<fv3::noisegen_gaussian_white_noise_2_>
    {
        
        GaussianWhiteNoise2() : GeneratorProcessorPlugin<fv3::noisegen_gaussian_white_noise_2_>()
        {        
            
        }
        
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {            
            return A*this->process();
        }
    };      
    */
    struct GaussianWhiteNoise3 : public GeneratorProcessorPlugin<fv3_white3>
    {
        
        GaussianWhiteNoise3() : GeneratorProcessorPlugin<fv3_white3>()
        {        
            
        }
        
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {            
            return A*this->process();
        }
    };      
}