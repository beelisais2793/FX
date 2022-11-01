#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct TubeAmp
    {
        tubeamp_params cp;

        TubeAmp() {
            tubeamp_create(&cp);
        }
        ~TubeAmp() {
            tubeamp_destroy(&cp);
        }
        
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            tubeamp_filter(&cp,n,in,out);
        }
    };
}

