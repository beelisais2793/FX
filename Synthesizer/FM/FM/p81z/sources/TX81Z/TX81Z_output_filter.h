#pragma once

namespace TX81Z {
//==============================================================================
// Remove DC from the output, apply some gain reduction
struct OutputFilter {
    void setup(float sampleRate) {}
    void clear();
    void run(const float *audioIn, float *audioOut, unsigned numFrames);

private:
    static constexpr float gain = 0.5; // -6 dB
    float previousIn_ = 0, previousOut_ = 0;
};
}  // namespace TX81Z
