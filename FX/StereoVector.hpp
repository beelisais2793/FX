#pragma once

#include <vector>
#include <cmath>

struct StereoVector
{
    std::vector<DspFloatType> buffer[2];

    StereoVector() = default;

    void resize(size_t n) {
        buffer[0].resize(n);
        buffer[1].resize(n);
    }
    size_t size() const { return buffer[0].size(); }
    void clear()
    {
        memset(delayBuffer[0].data(),0,delayBufferSamples*sizeof(DspFloatType));
        memset(delayBuffer[1].data(),0,delayBufferSamples*sizeof(DspFloatType));
    }

    DspFloatType get(size_t channel, size_t i) {
        return buffer[channel][i];
    }
    void set(size_t channel, size_t i, DspFloatType v) {
        buffer[channel][i] = v;
    }
    std::vector<DspFloatType>& get_channel(size_t ch) { return buffer[ch]; }
    void set_channel(size_t ch, std::vector<DspFloatType> & v) { buffer[ch] = v; }
};
