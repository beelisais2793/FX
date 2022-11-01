#pragma once

#include <cmath>
#include "SoundAlchemy.hpp"


#define PI 3.14159265358979323846
#define TAU 6.28318530717958647692

namespace SoundAlchemy::Distortion
{

template<typename T>
class Distortion
{
public:
    struct Controls {
        // Distortion mode, 0 = bypass, 1 = soft-clip, 2 = hard-clip
        int mode;
        // Drive, [1., ?), the amount of gain prior to the non-linearity
        T drive;
        // Threshold, [0., 1], the amplitude at which to start fold-back distortion
        T threshold;
        // Mix, [0., 1.] ratio between a dry and wet signal
        T mix;
    } controls;
    
    Distortion();
    ~Distortion();
    T processSample(T sample);
    
private:
    // Intermediate values
    T input, output = 0.f;
    T softClipThreshold = 2.f / 3.f;
    
    // Nonlinearities
    T softClip(T sample);
    T arctangent(T sample, T alpha);
    T hardClip(T sample);
    T squareLaw(T sample, T alpha);
    T cubicWaveShaper(T sample);
    
    T foldback(T sample);
    T waveShaper1(T sample, T alpha);
    T waveShaper2(T sample, T alpha);
    T waveShaper3(T sample, T alpha);
    
    T gloubiBoulga(T sample);
    T gloubiApprox(T sample);
};

template<typename T>
Distortion<T>::Distortion() {
    controls.mode = 2;
    controls.drive = 1.0f;
    controls.mix = 1.0f;
}

template<typename T>
Distortion<T>::~Distortion() {}

template<typename T>
T Distortion<T>::processSample(T sample)
{
    input = sample;
    output = input * controls.drive;
    
    switch (controls.mode) {
        case 1:
            output = softClip(output);
            break;
        case 2:
            output = hardClip(output);
            break;
        case 3:
            output = cubicWaveShaper(output);
            break;
        case 4:
            output = arctangent(input, controls.drive);
            break;
        case 5:
            output = squareLaw(input, controls.drive);
            break;
        case 6:
            output = foldback(output);
            break;
        case 7:
            output = gloubiApprox(output);
            break;
        case 8:
            output = gloubiBoulga(output);
            break;
        default:
            output = input;
            break;
    }
    
    return (1.f - controls.mix) * input + controls.mix * output;
}

/** Cubic soft-clipping nonlinearity
 
    Use 3x oversampling to eliminate aliasing
 */
template<typename T>
T Distortion<T>::softClip(T sample)
{
    if (sample < -1.f) {
        return -softClipThreshold;
    }
    else if (sample > 1.f) {
        return softClipThreshold;
    }
    else {
        return sample - ((sample * sample * sample) / 3.f);
    }
}

// Arctangent nonlinearity
template<typename T>
T Distortion<T>::arctangent(T sample, T alpha)
{
    // f(x) = (2 / PI) * arctan(alpha * x[n]), where alpha >> 1 (drive param)
    return (2.f / PI)* std::atan(alpha * sample);
}

// Hard-clipping nonlinearity
template<typename T>
T Distortion<T>::hardClip(T sample)
{
    if (sample < -1.f) {
        return -1.f;
    }
    else if (sample > 1.f) {
        return 1.f;
    }
    else {
        return sample;
    }
}

// Square law series expansion
template<typename T>
T Distortion<T>::squareLaw(T sample, T alpha)
{
    return sample + alpha * sample * sample;
}

/** A cubic nonlinearity, input range: [-1, 1]?
 
    Use 3x oversampling to eliminate aliasing
 */
template<typename T>
T Distortion<T>::cubicWaveShaper(T sample)
{
    return 1.5f * sample - 0.5f * sample * sample * sample;
}

// Foldback nonlinearity, input range: (-inf, inf)
template<typename T>
T Distortion<T>::foldback(T sample)
{
    // Threshold should be > 0.f
    if (sample > controls.threshold || sample < -controls.threshold) {
        sample = std::fabs(std::fabs(std::fmod(sample - controls.threshold,
                                controls.threshold * 4))
                      - controls.threshold * 2) - controls.threshold;
    }
    return sample;
}

// A nonlinearity by Partice Tarrabia and Bram de Jong
template<typename T>
T Distortion<T>::waveShaper1(T sample, T alpha)
{
    const T k = 2.f * alpha / (1.f - alpha);
    return (1.f + k) * sample / (1.f + k * std::fabs(sample));
}

// A nonlinearity by Jon Watte
template<typename T>
T Distortion<T>::waveShaper2(T sample, T alpha)
{
    const T z = PI * alpha;
    const T s = 1.f / std::sin(z);
    const T b = 1.f / alpha;
    
    if (sample > b) {
        return 1.f;
    }
    else {
        return std::sin(z * sample) * s;
    }
}

// A nonlinearity by Bram de Jong, input range: [-1, 1]
template<typename T>
T Distortion<T>::waveShaper3(T sample, T alpha)
{
    // original design requires sample be positive
    // alpha: [0, 1]
    bool isNegative = false;
    T output = sample;
    if (sample < 0.f) {
        isNegative = true;
        output = -output;
    }
    
    if (output > alpha) {
        output = alpha + (output - alpha) / (1.f + std::pow(((output - alpha) / (1.f - alpha)), 2.f));
    }
    if (output > 1.f) {
        output = (alpha + 1.f) / 2.f;
    }
    
    if (isNegative) {
        output = -output;
    }
    
    return output;
}


/** A nonlinearity by Laurent de Soras (allegedily)
    This is very expensive, and someone recommended using
    f(x) = x - 0.15 * x^2 - 0.15 * x^3 for a fast approximation.
 */
template<typename T>
T Distortion<T>::gloubiBoulga(T sample)
{
    const double x = sample * 0.686306;
    const double a = 1 + std::exp(std::sqrt(std::fabs(x)) * -0.75);
    return (std::exp(x) - std::exp(-x * a)) / (std::exp(x) + std::exp(-x));
}

// Approximation based on description in gloubiBoulga
template<typename T>
T Distortion<T>::gloubiApprox(T sample)
{
    return sample - (0.15f * sample * sample) - (0.15f * sample * sample * sample);
}
}

