#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    struct ClockDivider {
        uint32_t clock = 0;
        uint32_t division = 1;

        void reset() {
            clock = 0;
        }

        void setDivision(uint32_t division) {
            this->division = division;
        }

        uint32_t getDivision() {
            return division;
        }

        uint32_t getClock() {
            return clock;
        }

        /** Returns true when the clock reaches `division` and resets. */
        bool process() {
            clock++;
            if (clock >= division) {
                clock = 0;
                return true;
            }
            return false;
        }
    };
}