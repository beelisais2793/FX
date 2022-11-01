#pragma once

namespace pal
{
/// Implements a canonical biquad filter based on the equations found in the
/// audio EQ cookbook.
///
/// See https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
class Filter
{
    public:
    /// Initialize a new filter with default parameters
    /// (low pass, f0=1000, q=2).
    Filter();

    /// Draw a UI to control the filter.
    ///
    void draw();

    /// Set the filter coefficients to make a band pass filter.
    ///
    /// @param  f0      The desired cutoff frequency.
    /// @param  q       The desired q-factor.
    void makeBandPass(float f0, float q);

    /// Set the filter coefficients to make a high pass filter.
    ///
    /// @param  f0      The desired cutoff frequency.
    /// @param  q       The desired q-factor.
    void makeHighPass(float f0, float q);

    /// Set the filter coefficients to make a low pass filter.
    ///
    /// @param  f0      The desired cutoff frequency.
    /// @param  q       The desired q-factor.
    void makeLowPass(float f0, float q);

    /// Apply the filter to an input sample.
    ///
    /// @param  x   The sample to process.
    /// @returns    The processed sample.
    float process(float x);

    /// Reset the internal state of the filter.
    ///
    void reset();

    /// Set the sample rate. Should be the same as the rest of your app.
    /// Defaults to 44100Hz.
    ///
    /// @param  value   The desired sample rate.
    void setSampleRate(float value) { sampleRate = value; };

    private:

    enum FilterType
    {
        LowPass,
        HighPass,
        BandPass,
        NumFilterTypes
    };

    float sampleRate = 44100;

    float y1 = 0;
    float y2 = 0;
    float x1 = 0;
    float x2 = 0;

    float a0 = 0;
    float a1 = 0;
    float a2 = 0;

    float b0 = 0;
    float b1 = 0;
    float b2 = 0;

    FilterType uiSelectedFilterType = FilterType::LowPass;
    float uiF0 = 1000;
    float uiQ = 2;
};
}