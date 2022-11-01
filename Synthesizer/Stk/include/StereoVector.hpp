#pragma once

#include <vector>
#include <cmath>

struct StereoVector
{
    std::vector<float> buffer[2];

    StereoVector() = default;

    void resize(size_t n) {
        buffer[0].resize(n);
        buffer[1].resize(n);
    }
    size_t size() const { return buffer[0].size(); }
    void clear()
    {
        memset(delayBuffer[0].data(),0,delayBufferSamples*sizeof(float));
        memset(delayBuffer[1].data(),0,delayBufferSamples*sizeof(float));
    }

    float get(size_t channel, size_t i) {
        return buffer[channel][i];
    }
    void set(size_t channel, size_t i, float v) {
        buffer[channel][i] = v;
    }
    std::vector<float>& get_channel(size_t ch) { return buffer[ch]; }
    void set_channel(size_t ch, std::vector<float> & v) { buffer[ch] = v; }
};
