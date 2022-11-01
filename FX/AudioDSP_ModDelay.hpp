/// https://github.com/joonastuo/VibraFlanger
/// not finished porting it

#pragma once

#include "SoundObject.hpp"
#include "AudioDSP_DelayLine.hpp"


namespace JoonasFX
{
    //==============================================================================
    struct ModDelay : public MonoFXProcessor
    {

        ModDelay();
        ~ModDelay() = default;
        
        
        void update_params()
        {
            mLFO.setFreq(freq);
            mLFO.setDepth(depth);    
            mLFO.setPhase(phase);
            G = 1 - W;            
        }
        void Update();
        void ProcessBlock(size_t n, DspFloatType* input, DspFloatType* output);
        DspFloatType linearInterp(const DspFloatType& y0, const DspFloatType& yp1, const DspFloatType& frac);
        
        enum {
            PORT_FREQ,
            PORT_FEEDBACK,
            PORT_DEPTH,
            PORT_WET,
            PORT_MINDELAY,
            PORT_MAXDELAY,            
        };
        void printPorts()
        {
            printf("PORTS\n");
            printf("PORT_FREQ=0\n");
            printf("PORT_FEEDBACK=1\n");
            printf("PORT_DEPTH=2\n");
            printf("PORT_WET=3\n");
            printf("PORT_MINDELAY=4\n");
            printf("PORT_MAXDELAY=5\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_FREQ: mLFO.setFreq(v); break;
                case PORT_FEEDBACK: FB = v; break;
                case PORT_DEPTH: mLFO.setDepth(v); break;
                case PORT_WET: W = v; G = 1 - W; break;
                case PORT_MINDELAY: mMinDelayTime = v; break;
                case PORT_MAXDELAY: mMaxDelayDepth= v; break;                    
            }
        }
        void randomize() {
            FB = noise.rand();
            W = noise.rand();
            G = 1 - W;
            phase = noise.rand()*360.0f;
            mLFO.setFreq(noise.rand()*10.0f);
            mLFO.setDepth(noise.rand());
        }
        
        DspFloatType FB,W,G,freq,depth,phase;
        DspFloatType mSampleRate = 44100.f;        
        DspFloatType mMinDelayTime = 7.f;
        DspFloatType mMaxDelayDepth = 20.f;
        DelayLine<DspFloatType> mDelayLine;
        LFO mLFO;
        
    };

    ModDelay::ModDelay() : MonoFXProcessor()
    {
        mSampleRate = sampleRate;
        
        // Delay line
        size_t length = (mMinDelayTime + mMaxDelayDepth) * sampleRate;
        mDelayLine.resize(length);
        mDelayLine.clear();
        
        // LFO	
        mLFO.setWaveform(LFO::Waveforms::sine);
        mLFO.setUnipolar(true);
        mLFO.setPhase(0);
        mLFO.setFreq(0.2);
        mLFO.setDepth(1.0);
                    
        W = 0.5;
        G = 1.0 - W;
        FB= 0.5;
    }    
    void ModDelay::ProcessBlock(size_t nSamples, DspFloatType * input, DspFloatType * output)
    {
        update_params();
        // Iterate trough samples in a channel
        for (auto sample = 0; sample < nSamples; ++sample)
        {
           
            // Get delay amount
            DspFloatType delayTime = mMinDelayTime + mMaxDelayDepth * mLFO.getValueAndAdvance();

            // Separate delay to int + frac
            DspFloatType delayInSamplesFrac = mSampleRate * (delayTime / 1000.f);
            int   delayInSamplesInt  = static_cast<int> (delayInSamplesFrac);
            DspFloatType frac = delayInSamplesFrac - delayInSamplesInt;

            // Get delayed value
            DspFloatType delayedSample = 0.f;
            // Fractal delay with liner interpolation if neede
            if (frac != 0)
            {
                DspFloatType y0 = mDelayLine.get(delayInSamplesInt);
                DspFloatType ym1 = mDelayLine.get(delayInSamplesInt + 1);
                delayedSample = linearInterp(y0, ym1, frac);
            }
            // No fractal part
            else
            {
                delayedSample = mDelayLine.get(delayInSamplesInt);
            }

            auto inputSample = input[sample];

            // Add input and feedback to delayLine
            mDelayLine.push(inputSample + delayedSample * FB);

            // Mix dry and wet input
            output[sample] = inputSample * G + delayedSample * W;
        }
    }

    //==============================================================================
    DspFloatType ModDelay::linearInterp(const DspFloatType & y0, const DspFloatType & yp1, const DspFloatType & frac)
    {
        return yp1 * frac + y0 * (1 - frac);
    }
}