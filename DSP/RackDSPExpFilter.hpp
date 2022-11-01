#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    /** Applies exponential smoothing to a signal with the ODE
    \f$ \frac{dy}{dt} = x \lambda \f$.
    */
    template <typename T = float>
    struct TExponentialFilter {
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
            // If no change was made between the old and new output, assume T granularity is too small and snap output to input
            out = simd::ifelse(out == y, in, y);
            return out;
        }

        T process(T in) {
            return process(1.f, in);
        }
    };

    typedef TExponentialFilter<> ExponentialFilter;
}