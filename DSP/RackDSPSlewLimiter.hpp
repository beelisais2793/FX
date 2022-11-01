#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    template <typename T = float>
    struct TSlewLimiter {
        T out = 0.f;
        T rise = 0.f;
        T fall = 0.f;

        void reset() {
            out = 0.f;
        }

        void setRiseFall(T rise, T fall) {
            this->rise = rise;
            this->fall = fall;
        }
        T process(T deltaTime, T in) {
            out = std::clamp(in, out - fall * deltaTime, out + rise * deltaTime);
            return out;
        }
        T process(T in) {
            return process(1.f, in);
        }
    };

    typedef TSlewLimiter<> SlewLimiter;
}