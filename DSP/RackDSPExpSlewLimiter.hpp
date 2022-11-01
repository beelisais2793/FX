#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    template <typename T = float>
    struct TExponentialSlewLimiter {
        T out = 0.f;
        T riseLambda = 0.f;
        T fallLambda = 0.f;

        void reset() {
            out = 0.f;
        }

        void setRiseFall(T riseLambda, T fallLambda) {
            this->riseLambda = riseLambda;
            this->fallLambda = fallLambda;
        }
        T process(T deltaTime, T in) {
            T lambda = simd::ifelse(in > out, riseLambda, fallLambda);
            T y = out + (in - out) * lambda * deltaTime;
            // If the change from the old out to the new out is too small for floats, set `in` directly.
            out = simd::ifelse(out == y, in, y);
            return out;
        }
        T process(T in) {
            return process(1.f, in);
        }
    };

    typedef TExponentialSlewLimiter<> ExponentialSlewLimiter;
}