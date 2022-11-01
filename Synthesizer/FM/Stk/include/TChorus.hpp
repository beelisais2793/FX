#pragma once

#include "Undenormal.hpp"
#include <vector>

enum waveformIndex {
    waveformSine = 0,
    waveformTriangle,
    waveformSawtooth,
    waveformInverseSawtooth,
};

enum interpolationIndex {
    interpolationNearestNeighbour = 0,
    interpolationLinear,
    interpolationCubic,
};


template<typename T>
struct TChorus {

    std::vector<T> delayBuffer[2];

    int delayBufferSamples;
    int delayBufferChannels;
    int delayWritePosition[2];

    T lfoPhase;
    T inverseSampleRate;
    T twoPi;
    
    T sample_rate;
    T current_delay;
    T current_width;
    T current_depth;
    int   num_voices;
    size_t num_channels;
    T current_frequency;

    waveformIndex waveform;
    interpolationIndex interpolation;
    
    TChorus(T sampleRate=44100, T maxDelayTime=50.0f, int numChannels=2, T currentDelay=10.0, T currentWidth=0.02, T currentDepth=1.0,int numVoices=5,T currentFreq = 0.2 ) {        
        delayBufferSamples = (int)(maxDelayTime/1000.0f * sampleRate) + 1;
        if (delayBufferSamples < 1)delayBufferSamples = 1;        
        delayBufferChannels = numChannels;
        delayWritePosition[0] = 0;
        delayWritePosition[1] = 0;
        delayBuffer[0].resize(delayBufferSamples);
        delayBuffer[1].resize(delayBufferSamples);
        memset(delayBuffer[0].data(),0,delayBuffer[0].size()*sizeof(float));
        memset(delayBuffer[1].data(),0,delayBuffer[1].size()*sizeof(float));
        lfoPhase = 0.0f;
        inverseSampleRate = 1.0f / (T)sampleRate;
        twoPi = 2.0f * M_PI;
        waveform = waveformSine;
        interpolation = interpolationNearestNeighbour;
        sample_rate = sampleRate;        
        num_channels=2;
        current_delay = currentDelay/1000.0f;
        current_width = currentWidth;
        current_depth = currentDepth;
        num_voices = numVoices;
        current_frequency = currentFreq;
    }

    void SetMaxDelay(T maxDelayTime) {
        delayBufferSamples = (int)(maxDelayTime/1000.0f * sample_rate) + 1;
        if (delayBufferSamples < 1)delayBufferSamples = 1;        
        delayBuffer[0].resize(delayBufferSamples);
        delayBuffer[1].resize(delayBufferSamples);
    }
    void SetChannels(int n) { num_channels = n; }
    void SetVoices(int n) { num_voices = n; }
    void SetDelay(T ms) { current_delay = ms/1000.0f; }
    void SetWidth(T v) { current_width = v; }
    void SetFrequency(T f) { current_frequency = f; }

    T lfo (T phase, int waveform) {
        T out = 0.0f;
        switch (waveform) {
            case waveformSine: {
                out = 0.5f + 0.5f * sinf (twoPi * phase);
                break;
            }
            case waveformTriangle: {
                if (phase < 0.25f)
                    out = 0.5f + 2.0f * phase;
                else if (phase < 0.75f)
                    out = 1.0f - 2.0f * (phase - 0.25f);
                else
                    out = 2.0f * (phase - 0.75f);
                break;
            }
            case waveformSawtooth: {
                if (phase < 0.5f)
                    out = 0.5f + phase;
                else
                    out = phase - 0.5f;
                break;
            }
            case waveformInverseSawtooth: {
                if (phase < 0.5f)
                    out = 0.5f - phase;
                else
                    out = 1.5f - phase;
                break;
            }
        }
        return out;
    }

    void ProcessBlock(size_t n, T ** inputs, T ** outputs) 
    {
        Undenormal sphinxter;

        const int numInputChannels  = delayBufferChannels;
        const int numOutputChannels = delayBufferChannels;
        const int numSamples = n;
                      
        bool  stereo = delayBufferChannels == 2;

        int localWritePosition;
        T phase;

        for (int channel = 0; channel < numInputChannels; ++channel) {
            T* channelData = inputs[channel];
            T* channelOut  = outputs[channel];
            T* delayData = delayBuffer[channel].data();
            localWritePosition = delayWritePosition[channel];
            phase = lfoPhase;
            
            for (int sample = 0; sample < numSamples; ++sample) {
                const T in = channelData[sample];
                T out = 0.0f;
                T phaseOffset = 0.0f;
                T weight;
                channelOut[sample] = 0;
                for (int voice = 0; voice < num_voices - 1; ++voice) {
                    if (stereo && num_voices > 2) {
                        weight = (T)voice / (T)(num_voices - 2);
                        if (channel != 0)
                            weight = 1.0f - weight;
                    } else {
                        weight = 1.0f;
                    }

                    if(weight != 0)
                    {
                        T localDelayTime = (current_delay + current_width * lfo (phase + phaseOffset, waveform)) * (T)sample_rate;
                        T readPosition = fmodf ((T)localWritePosition - localDelayTime + (T)delayBufferSamples, delayBufferSamples);
                        int   localReadPosition = floorf (readPosition);

                        switch (interpolation) {
                            case interpolationNearestNeighbour: {
                                T closestSample = delayData[localReadPosition % delayBufferSamples];
                                out = closestSample;
                                break;
                            }
                            case interpolationLinear: {
                                T fraction = readPosition - (T)localReadPosition;
                                T delayed0 = delayData[(localReadPosition + 0)];
                                T delayed1 = delayData[(localReadPosition + 1) % delayBufferSamples];
                                out = delayed0 + fraction * (delayed1 - delayed0);
                                break;
                            }
                            case interpolationCubic: {
                                T fraction = readPosition - (T)localReadPosition;
                                T fractionSqrt = fraction * fraction;
                                T fractionCube = fractionSqrt * fraction;

                                T sample0 = delayData[(localReadPosition - 1 + delayBufferSamples) % delayBufferSamples];
                                T sample1 = delayData[(localReadPosition + 0)];
                                T sample2 = delayData[(localReadPosition + 1) % delayBufferSamples];
                                T sample3 = delayData[(localReadPosition + 2) % delayBufferSamples];

                                T a0 = - 0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
                                T a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
                                T a2 = - 0.5f * sample0 + 0.5f * sample2;
                                T a3 = sample1;
                                out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
                                break;
                            }
                        }

                        if (stereo && num_voices == 2)
                            channelOut[sample] = (channel == 0) ? in : out * current_depth;
                        else
                            channelOut[sample] += out * current_depth * weight;
                    }
                    if (num_voices == 3)
                        phaseOffset += 0.25f;
                    else if (num_voices > 3)
                        phaseOffset += 1.0f / (T)(num_voices - 1);
                }

                delayData[localWritePosition] = in;

                if (++localWritePosition >= delayBufferSamples)
                    localWritePosition -= delayBufferSamples;

                phase += current_frequency * inverseSampleRate;
                if (phase >= 1.0f)
                    phase -= 1.0f;
            }
            delayWritePosition[channel] = localWritePosition;
        }        
        lfoPhase = phase;        
    }

    void InplaceProcess(size_t n, float ** buffer) {
        ProcessBlock(n,buffer,buffer);
    }
};
