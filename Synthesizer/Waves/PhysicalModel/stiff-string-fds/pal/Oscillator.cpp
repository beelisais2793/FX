#include "Oscillator.h"
#include <cmath>
#include <stdlib.h>

#ifdef PAL
#include "Gui.h"
#endif

namespace pal
{

void Oscillator::draw()
{
#ifdef PAL
    const char *waveShapes[] = {"Sine", "Triangle", "Square", "Sawtooth", "Ramp", "Noise"};

    ImGui::Combo(
        "Wave shape",
        (int *)&type,
        waveShapes,
        IM_ARRAYSIZE(waveShapes));

    ImGui::SliderFloat("Freq.", &freq, 0.01, 20000, "%.3f", 4);

    if (type == Oscillator::Type::Square)
    {
        ImGui::SliderFloat("Pulse width", &pulseWidth, 0, 1);
    }
#endif
}

float Oscillator::getNext()
{
    if (type == Oscillator::Type::Sine)
    {
        phase += 2 * M_PI * freq / sampleRate;

        if (phase >= 2 * M_PI)
        {
            phase -= 2 * M_PI;
        }

        float y = sinf(phase);

        return y;
    }

    if (type == Oscillator::Type::Triangle)
    {
        phase += 2 * M_PI * freq / sampleRate;

        if (phase >= 2 * M_PI)
        {
            phase -= 2 * M_PI;
        }

        float y = 1 - 2 * (fabs(phase - M_PI) / M_PI);

        return y;
    }

    if (type == Oscillator::Type::Square)
    {
        phase += 2 * M_PI * freq / sampleRate;

        if (phase >= 2 * M_PI)
        {
            phase -= 2 * M_PI;
        }

        float y = ((phase / (2 * M_PI)) < pulseWidth ? 1 : -1);

        return y;
    }

    if (type == Oscillator::Type::Sawtooth)
    {
        phase -= 2 * M_PI * freq / sampleRate;

        if (phase <= 0)
        {
            phase += 2 * M_PI;
        }

        float y = 1 - 2 * (phase / (2 * M_PI));

        return y;
    }

    if (type == Oscillator::Type::Ramp)
    {
        phase += 2 * M_PI * freq / sampleRate;

        if (phase >= 2 * M_PI)
        {
            phase -= 2 * M_PI;
        }

        float y = 1 - 2 * (phase / (2 * M_PI));

        return y;
    }

    if (type == Oscillator::Type::Noise)
    {
        float n = 1 - 2 * (rand() / (float)RAND_MAX);
        return n;
    }

    return 0;
}

}