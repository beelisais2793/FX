#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    /** Detects when a boolean changes from false to true */
    struct BooleanTrigger {
        bool state = true;

        void reset() {
            state = true;
        }

        bool process(bool state) {
            bool triggered = (state && !this->state);
            this->state = state;
            return triggered;
        }
    };


    /** Turns HIGH when value reaches 1.f, turns LOW when value reaches 0.f. */
    template <typename T = float>
    struct TSchmittTrigger {
        T state;
        TSchmittTrigger() {
            reset();
        }
        void reset() {
            state = T::mask();
        }
        T process(T in) {
            T on = (in >= 1.f);
            T off = (in <= 0.f);
            T triggered = ~state & on;
            state = on | (state & ~off);
            return triggered;
        }
    };


    template <>
    struct TSchmittTrigger<float> {
        bool state = true;

        void reset() {
            state = true;
        }

        /** Updates the state of the Schmitt Trigger given a value.
        Returns true if triggered, i.e. the value increases from 0 to 1.
        If different trigger thresholds are needed, use
            process(rescale(in, low, high, 0.f, 1.f))
        for example.
        */
        bool process(float in) {
            if (state) {
                // HIGH to LOW
                if (in <= 0.f) {
                    state = false;
                }
            }
            else {
                // LOW to HIGH
                if (in >= 1.f) {
                    state = true;
                    return true;
                }
            }
            return false;
        }

        bool isHigh() {
            return state;
        }
    };

    typedef TSchmittTrigger<> SchmittTrigger;
}