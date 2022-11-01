#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    /** Like ExponentialFilter but jumps immediately to higher values.
    */
    template <typename T = float>
    struct TPeakFilter {
        T out = 0.f;
        T lambda = 0.f;

        void reset() {
            out = 0.f;
        }

        void setLambda(T lambda) {
            this->lambda = lambda;
        }

        void setTau(T tau) {
            this->lambda = 1 / tau;
        }

        T process(T deltaTime, T in) {
            T y = out + (in - out) * lambda * deltaTime;
            out = std::fmax(y, in);
            return out;
        }
        /** Use the return value of process() instead. */
        T peak() {
            return out;
        }
        /** Use setLambda() instead. */
        void setRate(T r) {
            lambda = 1.f - r;
        }
        T process(T x) {
            return process(1.f, x);
        }
    };

    typedef TPeakFilter<> PeakFilter;
}