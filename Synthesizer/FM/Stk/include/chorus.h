#pragma once

#include "samples.hpp"
#include <cassert>



namespace SoundWave {

    struct Chorus {

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
        SampleVector<float> delayBuffer[2];

        int delayBufferSamples;
        int delayBufferChannels;
        int delayWritePosition;

        float lfoPhase;
        float inverseSampleRate;
        float twoPi;

        float sample_rate;
        float current_delay;
        float current_width;
        float current_depth;
        int   num_voices;
        float current_frequency;
        float max_delay_time;

        waveformIndex waveform;
        interpolationIndex interpolation;

        Chorus(float sampleRate=44100, float maxDelayTime=50.0f, int numChannels=2, float currentDelay=10.0, float currentWidth=10.0, float currentDepth=1.0,int numVoices=2,float currentFreq = 0.2 ) {
            max_delay_time = maxDelayTime;
            delayBufferSamples = (int)(maxDelayTime/1000.0f * sampleRate) + 1;
            if (delayBufferSamples < 1) delayBufferSamples = 1;
            delayBufferChannels = numChannels;
            delayWritePosition = 0;
            delayBuffer[0].resize(delayBufferSamples, 1);
            delayBuffer[1].resize(delayBufferSamples, 1);
            lfoPhase = 0.0f;
            inverseSampleRate = 1.0f / (float)sampleRate;
            twoPi = 2.0f * M_PI;
            waveform = waveformSine;
            interpolation = interpolationNearestNeighbour;
            sample_rate = sampleRate;
            current_delay = currentDelay/1000.0f;
            current_width = currentWidth/1000.0f;
            current_depth = currentDepth;
            num_voices = numVoices;
            current_frequency = currentFreq;
        }
        void setNumVoices(int n) {
            num_voices = n;
        }
        void setWaveForm(waveformIndex w) {
            waveform = w;
        }
        void setInterpolation(interpolationIndex interp) {
            interpolation = interp;
        }
        void setMaxDelayTime(float delay) {
            max_delay_time = delay;
            delayBufferSamples = (int)(delay/1000.0f * sample_rate) + 1;
            if (delayBufferSamples < 1) delayBufferSamples = 1;
            delayBufferChannels = delayBufferChannels;
            delayWritePosition = 0;
            delayBuffer[0].resize(delayBufferSamples, 1);
            delayBuffer[1].resize(delayBufferSamples, 1);
        }

        void setCurrentDelay(float delay) {
            current_delay = delay/1000.0f;
        }
        void setCurrentWidth(float width) {
            current_width = width/1000.0f;
        }
        void setCurrentDepth(float depth) {
            current_depth = depth;
        }
        void setFrequency(float freq) {
            current_frequency = freq;
        }

        float lfo (float phase, int waveform) {
            float out = 0.0f;
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

        void Process(size_t n, const SampleVector<float> & input, SampleVector<float> & output) {

            const int numInputChannels  = delayBufferChannels;
            const int numOutputChannels = delayBufferChannels;
            const int numSamples        = n;

            //======================================

            bool  stereo = delayBufferChannels == 2;
            int localWritePosition;
            float phase;

            for (int channel = 0; channel < numInputChannels; ++channel) {
                SampleVector<float>  channelData = input.get_channel(channel);
                SampleVector<float>& delayData  = delayBuffer[channel];
                localWritePosition = delayWritePosition;
                phase = lfoPhase;

                for (int sample = 0; sample < numSamples; ++sample) {
                    const float in = channelData[sample];
                    float out = 0.0f;
                    float phaseOffset = 0.0f;
                    float weight;

                    for (int voice = 0; voice < num_voices - 1; ++voice) {
                        if (stereo && num_voices > 2) {
                            weight = (float)voice / (float)(num_voices - 2);
                            if (channel != 0)
                                weight = 1.0f - weight;
                        } else {
                            weight = 1.0f;
                        }

                        float localDelayTime = (current_delay + current_width * lfo (phase + phaseOffset, waveform)) * (float)sample_rate;
                        float readPosition = fmodf ((float)localWritePosition - localDelayTime + (float)delayBufferSamples, delayBufferSamples);
                        int   localReadPosition = floorf (readPosition);

                        switch (interpolation) {
                            case interpolationNearestNeighbour: {
                                float closestSample = delayData[localReadPosition % delayBufferSamples];
                                out = closestSample;
                                break;
                            }
                            case interpolationLinear: {
                                float fraction = readPosition - (float)localReadPosition;
                                float delayed0 = delayData[(localReadPosition + 0)];
                                float delayed1 = delayData[(localReadPosition + 1) % delayBufferSamples];
                                out = delayed0 + fraction * (delayed1 - delayed0);
                                break;
                            }
                            case interpolationCubic: {
                                float fraction = readPosition - (float)localReadPosition;
                                float fractionSqrt = fraction * fraction;
                                float fractionCube = fractionSqrt * fraction;

                                float sample0 = delayData[(localReadPosition - 1 + delayBufferSamples) % delayBufferSamples];
                                float sample1 = delayData[(localReadPosition + 0)];
                                float sample2 = delayData[(localReadPosition + 1) % delayBufferSamples];
                                float sample3 = delayData[(localReadPosition + 2) % delayBufferSamples];

                                float a0 = - 0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
                                float a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
                                float a2 = - 0.5f * sample0 + 0.5f * sample2;
                                float a3 = sample1;
                                out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
                                break;
                            }
                        }


                        if (stereo && num_voices == 2)
                            channelData[sample] = (channel == 0) ? in : out * current_depth;
                        else
                            channelData[sample] += out * current_depth * weight;

                        if (num_voices == 3)
                            phaseOffset += 0.25f;
                        else if (num_voices > 3)
                            phaseOffset += 1.0f / (float)(num_voices - 1);
                    }

                    delayData[localWritePosition] = in;

                    if (++localWritePosition >= delayBufferSamples)
                        localWritePosition -= delayBufferSamples;

                    phase += current_frequency * inverseSampleRate;

                    if (phase >= 1.0f)
                        phase -= 1.0f;
                }
                output.set_channel(channelData,channel);
            }

            delayWritePosition = localWritePosition;
            lfoPhase = phase;

        }

        void Process(size_t n, const float * input, float * output) {

            const int numInputChannels  = delayBufferChannels;
            const int numOutputChannels = delayBufferChannels;
            const int numSamples        = n;

            //======================================

            bool  stereo = delayBufferChannels == 2;
            int localWritePosition;
            float phase;

            for (int channel = 0; channel < numInputChannels; ++channel) {

                SampleVector<float>& delayData   = delayBuffer[channel];
                localWritePosition = delayWritePosition;
                phase = lfoPhase;

                for (int sample = 0; sample < numSamples; ++sample) {
                    float in = get_stride(channel,2,sample,input);
                    float out = 0.0f;
                    float phaseOffset = 0.0f;
                    float weight;
                    float s = in;

                    for (int voice = 0; voice < num_voices - 1; ++voice) {
                        if (stereo && num_voices > 2) {
                            weight = (float)voice / (float)(num_voices - 2);
                            if (channel != 0)
                                weight = 1.0f - weight;
                        } else {
                            weight = 1.0f;
                        }

                        float localDelayTime = (current_delay + current_width * lfo (phase + phaseOffset, waveform)) * (float)sample_rate;
                        float readPosition = fmodf ((float)localWritePosition - localDelayTime + (float)delayBufferSamples, delayBufferSamples);
                        int   localReadPosition = floorf (readPosition);

                        switch (interpolation) {
                            case interpolationNearestNeighbour: {
                                float closestSample = delayData[localReadPosition % delayBufferSamples];
                                out = closestSample;
                                break;
                            }
                            case interpolationLinear: {
                                float fraction = readPosition - (float)localReadPosition;
                                float delayed0 = delayData[(localReadPosition + 0)];
                                float delayed1 = delayData[(localReadPosition + 1) % delayBufferSamples];
                                out = delayed0 + fraction * (delayed1 - delayed0);
                                break;
                            }
                            case interpolationCubic: {
                                float fraction = readPosition - (float)localReadPosition;
                                float fractionSqrt = fraction * fraction;
                                float fractionCube = fractionSqrt * fraction;

                                float sample0 = delayData[(localReadPosition - 1 + delayBufferSamples) % delayBufferSamples];
                                float sample1 = delayData[(localReadPosition + 0)];
                                float sample2 = delayData[(localReadPosition + 1) % delayBufferSamples];
                                float sample3 = delayData[(localReadPosition + 2) % delayBufferSamples];

                                float a0 = - 0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
                                float a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
                                float a2 = - 0.5f * sample0 + 0.5f * sample2;
                                float a3 = sample1;
                                out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
                                break;
                            }
                        }


                        if (stereo && num_voices == 2)
                            s = (channel == 0) ? in : out * current_depth;
                        else
                            s += out * current_depth * weight;

                        if (num_voices == 3)
                            phaseOffset += 0.25f;
                        else if (num_voices > 3)
                            phaseOffset += 1.0f / (float)(num_voices - 1);

                    }

                    delayData[localWritePosition] = in;

                    if (++localWritePosition >= delayBufferSamples)
                        localWritePosition -= delayBufferSamples;

                    phase += current_frequency * inverseSampleRate;

                    if (phase >= 1.0f)
                        phase -= 1.0f;

                    set_stride(channel,2,sample,output,s);
                }

            }

            delayWritePosition = localWritePosition;
            lfoPhase = phase;

        }
    };
}
