#include "Filter.h"
#include <cmath>

#ifdef PAL
#include "Gui.h"
#endif

namespace pal
{

Filter::Filter()
{
    makeLowPass(uiF0, uiQ);
}

void Filter::draw()
{
#ifdef PAL
    const char *filterTypes[] = {"Low pass", "High pass", "Band pass"};

    bool typeChanged = ImGui::Combo(
        "Filter type",
        (int *)&uiSelectedFilterType,
        filterTypes,
        IM_ARRAYSIZE(filterTypes));
    
    bool f0Changed = ImGui::SliderFloat("Frequency", &uiF0, 20, 20000, "%.3f", 4);

    bool qChanged = ImGui::SliderFloat("Q", &uiQ, 0.01, 10);

    if (typeChanged || f0Changed || qChanged)
    {
        switch (uiSelectedFilterType)
        {
            case FilterType::LowPass:  makeLowPass(uiF0, uiQ);  break;
            case FilterType::HighPass: makeHighPass(uiF0, uiQ); break;
            case FilterType::BandPass: makeBandPass(uiF0, uiQ); break;
            default: break;
        }

        reset();
    }
#endif
}

void Filter::makeBandPass(float f0, float q)
{
    uiSelectedFilterType = FilterType::BandPass;
    uiF0 = f0;
    uiQ = q;

    const float omega0 = 2 * M_PI * (f0 / sampleRate);
    const float alpha = sinf(omega0) / (2 * q);

    b0 = q * alpha;
    b1 = 0;
    b2 = - q * alpha;

    a0 = 1 + alpha;
    a1 = - 2 * cos(omega0);
    a2 = 1 - alpha;
}

void Filter::makeHighPass(float f0, float q)
{
    uiSelectedFilterType = FilterType::HighPass;
    uiF0 = f0;
    uiQ = q;

    const float omega0 = 2 * M_PI * (f0 / sampleRate);
    const float alpha = sinf(omega0) / (2 * q);

    b0 = 0.5 * (1 + cosf(omega0));
    b1 = -(1 + cosf(omega0));
    b2 = 0.5 * (1 + cosf(omega0));

    a0 = 1 + alpha;
    a1 = - 2 * cos(omega0);
    a2 = 1 - alpha;
}

void Filter::makeLowPass(float f0, float q)
{
    uiSelectedFilterType = FilterType::LowPass;
    uiF0 = f0;
    uiQ = q;

    const float omega0 = 2 * M_PI * (f0 / sampleRate);
    const float alpha = sinf(omega0) / (2 * q);

    b0 = 0.5 * (1 - cosf(omega0));
    b1 = 1 - cosf(omega0);
    b2 = 0.5 * (1 - cosf(omega0));

    a0 = 1 + alpha;
    a1 = - 2 * cos(omega0);
    a2 = 1 - alpha;
}

float Filter::process(float x)
{
    float y = (b0 / a0) * x + (b1 / a0) * x1 + (b2 / a0) * x2
                            - (a1 / a0) * y1 - (a2 / a0) * y2;

    y2 = y1;
    y1 = y;

    x2 = x1;
    x1 = x;

    return y;
}

void Filter::reset()
{
    y1 = 0;
    y2 = 0;
    x1 = 0;
    x2 = 0;
}

}