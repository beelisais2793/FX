#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Tremolo
    {
        tremolo_params cp;

        Tremolo() {
            tremolo_create(&cp);
        }
        
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            tremolo_filter(&cp,n,in,out);
        }
    };
}

