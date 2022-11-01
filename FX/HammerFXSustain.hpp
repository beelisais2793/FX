#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Sustain
    {
        sustain_params cp;

        Sustain() {
            sustain_create(&cp);
        }
        
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            sustain_filter(&cp,n,in,out);
        }
    };
}

