#pragma once

#include <cmath>

namespace FX::Distortion::DistortionCompressor
{
    class PeakLevelDetector {
    public:
        PeakLevelDetector(DspFloatType sampleRate);
        ~PeakLevelDetector();
        
        DspFloatType tick(DspFloatType inputSample);
        void setDetector(DspFloatType sampleRate);
        
    private:
        DspFloatType fs, inputAbs, peakOutput;
        DspFloatType b0Attack, b0Release, b0, a1;
        DspFloatType releaseTime = 0.100f;     // seconds
    };

    class GainDynamics {
    public:
        GainDynamics(DspFloatType sampleRate, DspFloatType attackTime, DspFloatType releaseTime);
        ~GainDynamics();
        
        DspFloatType tick(DspFloatType inputSample);
        void setDetector(DspFloatType sampleRate);
        void setAttack(DspFloatType attackTime);
        void setRelease(DspFloatType releaseTime);
        
    private:
        DspFloatType fs, outputGain;
        DspFloatType b0Attack, b0Release, b0;
        DspFloatType attackTime, releaseTime;     // in seconds
    };

    #define PI 3.14159265358979323846
    #define TAU 6.28318530717958647692

    class Distortion
    {
    public:
        struct Controls {
            // Distortion mode, 0 = bypass, 1 = soft-clip, 2 = hard-clip
            int mode;
            // Drive, [1., ?), the amount of gain prior to the non-linearity
            DspFloatType drive;
            // Threshold, [0., 1], the amplitude at which to start fold-back distortion
            DspFloatType threshold;
            // Mix, [0., 1.] ratio between a dry and wet signal
            DspFloatType mix;
        } controls;
        
        Distortion();
        ~Distortion();
        DspFloatType processSample(DspFloatType, int);
        
    private:
        // Intermediate values
    // DspFloatType input, output;
        DspFloatType softClipThreshold = 2.f / 3.f;
        
        // Nonlinearities
        DspFloatType softClip(DspFloatType sample);
        DspFloatType arctangent(DspFloatType sample, DspFloatType alpha);
        DspFloatType hardClip(DspFloatType sample);
        DspFloatType squareLaw(DspFloatType sample, DspFloatType alpha);
        DspFloatType cubicWaveShaper(DspFloatType sample);
        
        DspFloatType foldback(DspFloatType sample);
        DspFloatType waveShaper1(DspFloatType sample, DspFloatType alpha);
        DspFloatType waveShaper2(DspFloatType sample, DspFloatType alpha);
        DspFloatType waveShaper3(DspFloatType sample, DspFloatType alpha);
        
        DspFloatType gloubiBoulga(DspFloatType sample);
        DspFloatType gloubiApprox(DspFloatType sample);

    public:
        DspFloatType input;
        DspFloatType output;
    };


    Distortion::Distortion() : input(0.f), output(0.f) {
        controls.mode = 0;
        controls.drive = 1.f;
        controls.mix = 0.f;
    }

    Distortion::~Distortion() {}

    DspFloatType Distortion::processSample(DspFloatType sample, int type)
    {
        input = sample;
        output = input * controls.drive;
        
        switch (type) {
            case 1:
                output = softClip(output);
                break;
            case 2:
                output = arctangent(input, controls.drive);
                break;
            case 3:
                output = hardClip(output);
                break;
            case 4:
                output = squareLaw(input, controls.drive);
                break;
            case 5:
                output = cubicWaveShaper(output);
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
    DspFloatType Distortion::softClip(DspFloatType sample)
    {
        if (sample < -1.f) {
            return -softClipThreshold; //2/3
        }
        else if (sample > 1.f) {
            return softClipThreshold;
        }
        else {
            return sample - ((sample * sample * sample) / 3.f);
        }
    }

    // Arctangent nonlinearity
    DspFloatType Distortion::arctangent(DspFloatType sample, DspFloatType alpha)
    {
        // f(x) = (2 / PI) * arctan(alpha * x[n]), where alpha >> 1 (drive param)
        return (2.f / PI)* atan(alpha * sample);
    }

    // Hard-clipping nonlinearity
    DspFloatType Distortion::hardClip(DspFloatType sample)
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
    DspFloatType Distortion::squareLaw(DspFloatType sample, DspFloatType alpha)
    {
        return sample + alpha * sample * sample;
    }

    /** A cubic nonlinearity, input range: [-1, 1]?
     
        Use 3x oversampling to eliminate aliasing
    */
    DspFloatType Distortion::cubicWaveShaper(DspFloatType sample)
    {
        return 1.5f * sample - 0.5f * sample * sample * sample;
    }

    // Foldback nonlinearity, input range: (-inf, inf)
    DspFloatType Distortion::foldback(DspFloatType sample)
    {
        // Threshold should be > 0.f
        if (sample > controls.threshold || sample < -controls.threshold) {
            sample = fabs(fabs(fmod(sample - controls.threshold,
                                    controls.threshold * 4))
                        - controls.threshold * 2) - controls.threshold;
        }
        return sample;
    }

    // A nonlinearity by Partice Tarrabia and Bram de Jong
    DspFloatType Distortion::waveShaper1(DspFloatType sample, DspFloatType alpha)
    {
        const DspFloatType k = 2.f * alpha / (1.f - alpha);
        return (1.f + k) * sample / (1.f + k * fabs(sample));
    }

    // A nonlinearity by Jon Watte
    DspFloatType Distortion::waveShaper2(DspFloatType sample, DspFloatType alpha)
    {
        const DspFloatType z = PI * alpha;
        const DspFloatType s = 1.f / sin(z);
        const DspFloatType b = 1.f / alpha;
        
        if (sample > b) {
            return 1.f;
        }
        else {
            return sin(z * sample) * s;
        }
    }

    // A nonlinearity by Bram de Jong, input range: [-1, 1]
    DspFloatType Distortion::waveShaper3(DspFloatType sample, DspFloatType alpha)
    {
        // original design requires sample be positive
        // alpha: [0, 1]
        bool isNegative = false;
        DspFloatType output = sample;
        if (sample < 0.f) {
            isNegative = true;
            output = -output;
        }
        
        if (output > alpha) {
            output = alpha + (output - alpha)
                / (1.f + powf(((output - alpha)) / (1.f - alpha), 2.f));
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
    DspFloatType Distortion::gloubiBoulga(DspFloatType sample)
    {
        const DspFloatType x = sample * 0.686306;
        const DspFloatType a = 1 + exp(sqrt(fabs(x)) * -0.75);
        return (exp(x) - exp(-x * a)) / (exp(x) + exp(-x));
    }

    // Approximation based on description in gloubiBoulga
    DspFloatType Distortion::gloubiApprox(DspFloatType sample)
    {
        return sample - (0.15f * sample * sample) - (0.15f * sample * sample * sample);
    }

    // Times are in seconds (e.g. 100ms = 0.1f, 1.2s = 1.2f)
    GainDynamics::GainDynamics(DspFloatType sampleRate, DspFloatType newAttackTime, DspFloatType newReleaseTime) {
        attackTime = newAttackTime;
        releaseTime = newReleaseTime;
        setDetector(sampleRate);
    }

    GainDynamics::~GainDynamics() {}

    DspFloatType GainDynamics::tick(DspFloatType inputGain) {
        if (inputGain < outputGain) {   // Isn't this suppose to be (input > lastOutput)?
            b0 = b0Attack;
        }
        else {
            b0 = b0Release;
        }
        
        // Simplified filter equation (out = b0 * input + a1 * lastOut)
        outputGain += b0 * (inputGain - outputGain);
        
        return outputGain;
    }

    void GainDynamics::setDetector(DspFloatType sampleRate) {
        fs = sampleRate;
        outputGain = 0.f;
        setAttack(attackTime);
        setRelease(releaseTime);
    }

    void GainDynamics::setAttack(DspFloatType newAttackTime) {
        attackTime = newAttackTime;
        b0Attack = 1. - expf(-1. / (attackTime * fs));;
    }

    void GainDynamics::setRelease(DspFloatType newReleaseTime) {
        releaseTime = newReleaseTime;
        b0Release = 1. - expf(-1. / (releaseTime * fs));;
    }

    PeakLevelDetector::PeakLevelDetector(DspFloatType sampleRate) {
        setDetector(sampleRate);
    }

    PeakLevelDetector::~PeakLevelDetector() {}

    DspFloatType PeakLevelDetector::tick(DspFloatType inputSample) {
        inputAbs = fabs(inputSample);
        
        if (inputAbs > peakOutput) {
            b0 = b0Attack;
        }
        else {
            b0 = b0Release;
        }
        
        // Simplified filter equation (out = b0 * input + a1 * lastOut)
        peakOutput += b0 * (inputAbs - peakOutput);
        
        return peakOutput;
    }

    void PeakLevelDetector::setDetector(DspFloatType sampleRate) {
        fs = sampleRate;
        peakOutput = 0.f;
        
        // set coefficients for leaky integrator
        b0Attack = 1.f;
        a1 = expf(-1 / (releaseTime * fs));
        b0Release = 1.f - a1;
    }   
}