#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    /** When triggered, holds a high value for a specified time before going low again */
    struct PulseGenerator {
        float remaining = 0.f;

        /** Immediately disables the pulse */
        void reset() {
            remaining = 0.f;
        }

        /** Advances the state by `deltaTime`. Returns whether the pulse is in the HIGH state. */
        bool process(float deltaTime) {
            if (remaining > 0.f) {
                remaining -= deltaTime;
                return true;
            }
            return false;
        }

        /** Begins a trigger with the given `duration`. */
        void trigger(float duration = 1e-3f) {
            // Keep the previous pulse if the existing pulse will be held longer than the currently requested one.
            if (duration > remaining) {
                remaining = duration;
            }
        }
    };
}