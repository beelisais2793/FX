#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_earlyref = fv3::earlyref_;
    #else
    using fv3_earlyref = fv3::earlyref_f;
    #endif
    struct EarlyRef : public StereoFXProcessorPlugin<fv3_earlyref>
    {
        enum Presets
        {
            DEFAULT=0,
            PRESET1,
            PRESET2,
            PRESET_11=11,
            PRESET_12,
            PRESET_13,
            PRESET_14,
            PRESET_15,
            PRESET_16,
            PRESET_17,
            PRESET_18,
            PRESET_19,
            PRESET_20,
            PRESET_21,
            PRESET_22
        };

        EarlyRef(DspFloatType LRDelay, int preset = DEFAULT) 
        : StereoFXProcessorPlugin<fv3_earlyref>()
        {
            this->setLRDelay(LRDelay);            
            this->loadPresetReflection(preset);
        }

        void ProcessBlock(size_t n, DspFloatType ** input, DspFloatType ** output)
        {
            this->processreplace(input[0],input[1],output[0],output[1],n);
        }
    };
}