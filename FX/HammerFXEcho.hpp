#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Echo
    {
        echo_params cp;

        Echo() {
            echo_create(&cp);
        }
        ~Echo() {
            echo_destroy(&cp);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            echo_filter(&cp,n,in,out);
        }
    };
}

