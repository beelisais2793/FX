#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    struct Timer {
        float time = 0.f;

        void reset() {
            time = 0.f;
        }

        /** Returns the time since last reset or initialization. */
        float process(float deltaTime) {
            time += deltaTime;
            return time;
        }
    };
}