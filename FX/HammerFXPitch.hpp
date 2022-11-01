#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Pitch
    {
        tubeamp_params cp;

        Pitch() {
            pitch_create(&cp);
        }
        ~Pitch() {
            pitch_destroy(&cp);
        }
        
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            pitch_filter(&cp,n,in,out);
        }
    };
}

