#include "TX81Z_output_filter.h"
#include "TX81Z_common.h"

namespace TX81Z {
constexpr float OutputFilter::gain;

void OutputFilter::clear()
{
    previousIn_ = 0;
    previousOut_ = 0;
}

void OutputFilter::run(const float *audioIn, float *audioOut, unsigned numFrames)
{
    float previousIn = previousIn_;
    float previousOut = previousOut_;

    for (unsigned i = 0; i < numFrames; ++i) {
        float y = audioIn[i] - previousIn + 0.997f * previousOut;
        previousIn = audioIn[i];
        previousOut = y;

        audioOut[i] = gain * y;
    }

    previousIn_ = previousIn;
    previousOut_ = previousOut;
}
}  // namespace TX81Z
