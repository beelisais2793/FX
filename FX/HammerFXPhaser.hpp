#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Phasor
    {
        echo_params cp;

        Phasor() {
            phasor_create(&cp);
        }
        
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            phasor_filter(&cp,n,in,out);
        }
    };
}

