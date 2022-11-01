#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::TransferFunction
{
    // returns frequency response
    // callback 
    struct TrasnferFunc : public FilterProcessorPlugin<gam::TransferFunc>
    {

    };

    //todo
    // Time domain difference equation 
    // Impulse Response
    // Analog H(S) Bilinear/z
    // Digital H(Z) 
};
