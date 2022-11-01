#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    /** The simplest possible analog filter using an Euler solver.
    https://en.wikipedia.org/wiki/RC_circuit
    Use two RC filters in series for a bandpass filter.
    */
    template <typename T = float>
    struct TRCFilter {
        T c = 0.f;
        T xstate[1];
        T ystate[1];

        TRCFilter() {
            reset();
        }

        void reset() {
            xstate[0] = 0.f;
            ystate[0] = 0.f;
        }

        /** Sets the cutoff angular frequency in radians.
        */
        void setCutoff(T r) {
            c = 2.f / r;
        }
        /** Sets the cutoff frequency.
        `f` is the ratio between the cutoff frequency and sample rate, i.e. f = f_c / f_s
        */
        void setCutoffFreq(T f) {
            setCutoff(2.f * M_PI * f);
        }
        void process(T x) {
            T y = (x + xstate[0] - ystate[0] * (1 - c)) / (1 + c);
            xstate[0] = x;
            ystate[0] = y;
        }
        T lowpass() {
            return ystate[0];
        }
        T highpass() {
            return xstate[0] - ystate[0];
        }
    };

    typedef TRCFilter<> RCFilter;
}