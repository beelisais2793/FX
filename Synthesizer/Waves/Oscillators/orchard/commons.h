#pragma once

#include "Utility/delayline.h"
#include "Utility/dsp.h"

#define MAX_DELAY static_cast<size_t>(48000 * 1.f)

namespace orchard
{
    using namespace daisysp;

    enum class Range
    {
        FULL,
        HIGH,
        LOW,
    };

    float RandomFloat(float min, float max)
    {
        return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }

    enum class Scale
    {
        IONIAN,
        DORIAN,
        PHRYGIAN,
        LYDIAN,
        MIXOLYDIAN,
        AEOLIAN,
        LOCRIAN,
        LAST_SCALE,
    };
    constexpr int scaleIntervals{15};
    // Ionian w-w-h-w-w-w-h
    // Dorian   w-h-w-w-w-h-w
    // Phrygian   h-w-w-w-h-w-w
    // Lydian       w-w-w-h-w-w-h
    // Mixolydian     w-w-h-w-w-h-w
    // Aeolian          w-h-w-w-h-w-w
    // Locrian            h-w-w-h-w-w-w
    int scales[static_cast<unsigned int>(Scale::LAST_SCALE)][scaleIntervals]{
        {-12, -10, -8, -7, -5, -3, -1, 0, 2, 4, 5, 7, 9, 11, 12},
        {-12, -10, -9, -7, -5, -3, -2, 0, 2, 3, 5, 7, 9, 10, 12},
        {-12, -11, -9, -7, -5, -4, -2, 0, 1, 3, 5, 7, 8, 10, 12},
        {-12, -10, -8, -6, -5, -3, -1, 0, 2, 4, 6, 7, 9, 11, 12},
        {-12, -10, -8, -7, -5, -3, -2, 0, 2, 4, 5, 7, 9, 10, 12},
        {-12, -10, -9, -7, -5, -4, -2, 0, 2, 3, 5, 7, 8, 10, 12},
        {-12, -11, -9, -7, -6, -4, -2, 0, 1, 3, 5, 6, 8, 10, 12},
    };

    Scale currentScale{Scale::PHRYGIAN};

    int RandomInterval(Range range)
    {
        int rnd;

        if (Range::HIGH == range)
        {
            int half{static_cast<int>(std::ceil(scaleIntervals / 2))};
            rnd = (half + (std::rand() % half)) - 1;
        }
        else if (Range::LOW == range)
        {
            int half{static_cast<int>(std::ceil(scaleIntervals / 2))};
            rnd = std::rand() % half - 1;
        }
        else
        {
            rnd = std::rand() % scaleIntervals;
        }

        return rnd;
    }

    int RandomPitch(Range range)
    {
        int rnd;

        if (Range::HIGH == range)
        {
            // (midi 66-72)
            rnd = RandomFloat(42, 72);
        }
        else if (Range::LOW == range)
        {
            // (midi 36-65)
            rnd = RandomFloat(12, 41);
        }
        else
        {
            // (midi 36-96)
            rnd = RandomFloat(12, 72);
        }

        return rnd;
    }
}