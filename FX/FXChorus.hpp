#pragma once

#include <vector>
#include <cmath>

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


struct Chorus {

    std::vector<DspFloatType> delayBuffer[2];

    int delayBufferSamples;
    int delayBufferChannels;
    int delayWritePosition[2];

    DspFloatType lfoPhase;
    DspFloatType inverseSampleRate;
    DspFloatType twoPi;
    
    size_t inputChannels;
    size_t outputChannels;

    DspFloatType sample_rate;
    DspFloatType current_delay;
    DspFloatType current_width;
    DspFloatType current_depth;
    int   num_voices;
    DspFloatType current_frequency;

    waveformIndex waveform;
    interpolationIndex interpolation;
    
    Chorus( DspFloatType sampleRate=44100, 
            DspFloatType maxDelayTime=50.0f,             
            int numChannels=2, 
            DspFloatType currentDelay=10.0, 
            DspFloatType currentWidth=10.0, 
            DspFloatType currentDepth=1.0,
            int numVoices=5,
            DspFloatType currentFreq = 0.2 ) 
    {                    
        delayBufferChannels = numChannels;
        inputChannels = numChannels;
        outputChannels= numChannels;

        delayWritePosition[0] = 0;        
        delayWritePosition[1] = 0;        
        lfoPhase = 0.0f;
        sample_rate   = sampleRate;        
        inverseSampleRate = 1.0f / (DspFloatType)sampleRate;
        twoPi         = 2.0f * M_PI;
        waveform      = waveformSine;
        interpolation = interpolationNearestNeighbour;        
        current_delay = currentDelay/1000.0f;
        current_width = currentWidth/1000.0f;
        current_depth = currentDepth;
        num_voices    = numVoices;
        current_frequency = currentFreq;

        setMaxDelayTime(maxDelayTime);
    }
    void setMaxDelayTime(DspFloatType t) {
         DspFloatType maxDelayTime = t/1000.0f;
         delayBufferSamples = (int)(maxDelayTime * sample_rate) + 1;
         if (delayBufferSamples < 1) delayBufferSamples = 1;                  
         delayBuffer[0].resize(delayBufferSamples);
         delayBuffer[1].resize(delayBufferSamples);
    }
    void setCurrentDelay(DspFloatType ms) {
        current_delay = ms/1000.0f;
    }
    void setWidth(DspFloatType ms) {
        current_width = ms/1000.0f;
    }
    void setDepth(DspFloatType d) {
        current_depth = d;
    }
    void setNumVoices(unsigned n) {
        num_voices = n;
    }
    void setLfoFrequency(DspFloatType f) {
        current_frequency = f;
    }
    DspFloatType lfo (DspFloatType phase, int waveform) {
        DspFloatType out = 0.0f;
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

    // monophonic stereo mix
    DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
    {
        DspFloatType in[2];
        DspFloatType out[2];
        in[0] = in[1] = I;
        out[0] = out[1] = 0.0f;
        Process(1,in,out);
        return 0.5*(out[0] + out[1]);
    }

    void Process(size_t n, const DspFloatType * input, DspFloatType * output) {
    
        const int numInputChannels  = delayBufferChannels;
        const int numOutputChannels = delayBufferChannels;
        const int numSamples = n;
        
        //======================================
        
        bool  stereo = (delayBufferChannels == 2);

        int localWritePosition[2];
        DspFloatType phase;

        for (int channel = 0; channel < numInputChannels; ++channel) {
            const DspFloatType * channelData = input;
	        DspFloatType       * delayData   = delayBuffer[channel].data();
            
            localWritePosition[channel] = delayWritePosition[channel];            
            phase = lfoPhase;
            
            for (int sample = 0; sample < numSamples; ++sample) 
            {                
                size_t input_index  = numInputChannels*sample + channel;
                size_t output_index = numOutputChannels*sample + channel;                
                const DspFloatType in      = channelData[input_index];
                DspFloatType out = 0.0f;
                DspFloatType phaseOffset = 0.0f;
                DspFloatType weight;
		                    
                output[output_index] = in;
                
                for (int voice = 0; voice < num_voices - 1; ++voice) 
                {
                    if (stereo && num_voices > 2) {
                        weight = (DspFloatType)voice / (DspFloatType)(num_voices - 2);
                        if (channel != 0)
                            weight = 1.0f - weight;
                    } else {
                        weight = 1.0f;
                    }

                    DspFloatType localDelayTime = (current_delay + current_width * lfo (phase + phaseOffset, waveform)) * (DspFloatType)sample_rate;
                    DspFloatType readPosition = fmodf ((DspFloatType)localWritePosition[channel] - localDelayTime + (DspFloatType)delayBufferSamples, delayBufferSamples);                    
                    int   localReadPosition = floorf (readPosition);
                    
                    switch (interpolation) {
                        case interpolationNearestNeighbour: {
                            DspFloatType closestSample = delayData[localReadPosition % delayBufferSamples];
                            out = closestSample;
                            break;
                        }
                        case interpolationLinear: {
                            DspFloatType fraction = readPosition - (DspFloatType)localReadPosition;
                            DspFloatType delayed0 = delayData[(localReadPosition + 0)];
                            DspFloatType delayed1 = delayData[(localReadPosition + 1) % delayBufferSamples];
                            out = delayed0 + fraction * (delayed1 - delayed0);
                            break;
                        }
                        case interpolationCubic: {
                            DspFloatType fraction = readPosition - (DspFloatType)localReadPosition;
                            DspFloatType fractionSqrt = fraction * fraction;
                            DspFloatType fractionCube = fractionSqrt * fraction;

                            DspFloatType sample0 = delayData[(localReadPosition - 1 + delayBufferSamples) % delayBufferSamples];
                            DspFloatType sample1 = delayData[(localReadPosition + 0)];
                            DspFloatType sample2 = delayData[(localReadPosition + 1) % delayBufferSamples];
                            DspFloatType sample3 = delayData[(localReadPosition + 2) % delayBufferSamples];

                            DspFloatType a0 = - 0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
                            DspFloatType a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
                            DspFloatType a2 = - 0.5f * sample0 + 0.5f * sample2;
                            DspFloatType a3 = sample1;
                            out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
                            break;
                        }
                    }

                    if (stereo && num_voices == 2)
                        output[output_index] = (channel == 0) ? in : out * current_depth;
                    else
                        output[output_index] += out * current_depth * weight;

                    if (num_voices == 3)
                        phaseOffset += 0.25f;
                    else if (num_voices > 3)
                        phaseOffset += 1.0f / (DspFloatType)(num_voices - 1);
                }                                
                
                delayData[localWritePosition[channel]] = in;                                
                if (++localWritePosition[channel] >= delayBufferSamples)
                    localWritePosition[channel] -= delayBufferSamples;

                phase += current_frequency * inverseSampleRate;
                if (phase >= 1.0f)
                    phase -= 1.0f;
            }                                    
        }        
        delayWritePosition[0] = localWritePosition[0];
        delayWritePosition[1] = localWritePosition[1];
        lfoPhase = phase;
    }

};
