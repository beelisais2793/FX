#pragma once

#include "SoundObject.hpp"
#include <array>
#include "AudioDSP_Lfo.hpp"
#include "AudioDSP_DelayLine.hpp"
#include "AudioDSP_ModDelay.hpp"

namespace JoonasFX
{
    struct Chorus : public StereoFXProcessor
    {
        enum {
            WET0_PORT,
            FEEDBACK0_PORT,
            FREQ0_PORT,
            PHASE0_PORT,
            DEPTH0_PORT,
            WET1_PORT,
            FEEDBACK1_PORT,
            FREQ1_PORT,
            PHASE1_PORT,
            DEPTH1_PORT,
            WET2_PORT,
            FEEDBACK2_PORT,
            FREQ2_PORT,
            PHASE2_PORT,
            DEPTH2_PORT,
            RANDOMIZE_PORT,
            POWER_PORT,
            VOLUME_PORT,
            MINDELAY_PORT,
            MAXDELAY_PORT,
        };
        
        Chorus();
        ~Chorus() = default;
        
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case WET0_PORT: wets[0] = v; break;
                case WET1_PORT: wets[1] = v; break;
                case WET2_PORT: wets[2] = v; break;
                case FEEDBACK0_PORT: feedbacks[0] = v; break;
                case FEEDBACK1_PORT: feedbacks[1] = v; break;
                case FEEDBACK2_PORT: feedbacks[2] = v; break;
                case FREQ0_PORT: freqs[0] = v; break;
                case FREQ1_PORT: freqs[1] = v; break;
                case FREQ2_PORT: freqs[2] = v; break;
                case DEPTH0_PORT: depths[0] = v; break;
                case DEPTH1_PORT: depths[1] = v; break;
                case DEPTH2_PORT: depths[2] = v; break;
                case PHASE0_PORT: phases[0] = v; break;
                case PHASE1_PORT: phases[1] = v; break;
                case PHASE2_PORT: phases[2] = v; break;
                case RANDOMIZE_PORT: if(v != 0.0) randomize(); break;
                case POWER_PORT: power((bool)v); break;
                case VOLUME_PORT: db = pow(10.0,v/20.0); break;
                case MINDELAY_PORT: minDelay = v; break;
                case MAXDELAY_PORT: maxDelayDepth = v; break;
            }
        }
        void ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs);
        void updateParameters();
    
        DspFloatType getVolume() { return pow(10,db/20.0);}

        void power(bool on = true) {
            on_button = on;
        }
       
        void randomize() {
            for(size_t i = 0; i < 3; i++)
            {
                wets[i] = noise.rand();
                phases[i] = noise.rand()*360.0f;
                freqs[i] = noise.rand()*0.05;
                feedbacks[i] = noise.rand();
                depths[i] = noise.rand();                
            }                        
        }
    
        std::vector<DspFloatType> mCenterBuffer;
        
        ModDelay mLeftDelay;
        ModDelay mCenterDelay;
        ModDelay mRightDelay;
        
        bool on_button = true;

        // Prepare delay lines
        DspFloatType minDelay = 7.0f;
        DspFloatType maxDelayDepth = 20.0f;
        
        DspFloatType wets[3]   = { 0.7,0.7,0.7};
        DspFloatType phases[3] = { 90,0,270};
        DspFloatType freqs[3]  = { 0.002,0.001,0.002 };
        DspFloatType feedbacks[3] = { 0.5,0.5,0.5};
        DspFloatType depths[3] = { 0.95, 0.95, 0.95};

        DspFloatType db = 3;
        DspFloatType mSampleRate = 44100.f;	
        int mNumChannels = 2;
        int mDelayBufferLen = 0;

    };

    Chorus::Chorus() : StereoFXProcessor()
    {        
        int delayBufferLen = static_cast<int>(((minDelay + maxDelayDepth) / 1000.f) * sampleRate);        
        ModDelay* moddelays[3] = { &mLeftDelay, &mCenterDelay, &mRightDelay };                
        for (auto i = 0; i < 3; ++i)
        {                                           
            moddelays[i]->W      		  = wets[i];
            moddelays[i]->G      		  = 1-wets[i];
            moddelays[i]->FB     		  = feedbacks[i];
            moddelays[i]->mLFO.setFreq( freqs[i]);
            moddelays[i]->mLFO.setPhase(phases[i]);
            moddelays[i]->mLFO.setDepth(depths[i]);            
        }                        
        // Set values to member variables ===============
        mSampleRate  = sampleRate;	
        mNumChannels = 2;        
    }

    void Chorus::updateParameters()
    {            
        mLeftDelay.update_params();
        mCenterDelay.update_params();
        mRightDelay.update_params();
    }

    
    void Chorus::ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
    {
        Undenormal dn;
        if(on_button == false) {
            memcpy(outputs[0],inputs[0],n*sizeof(DspFloatType));
            memcpy(outputs[1],inputs[1],n*sizeof(DspFloatType));
            return;
        }    
        updateParameters();

        // Left and right input
        DspFloatType* leftInput  = inputs[0];
        DspFloatType* rightInput = inputs[1];
        
        // Middle
        mCenterBuffer.resize(n);    
        for(size_t i = 0; i < n; i++) {
            mCenterBuffer[i] = 0.5*(leftInput[i] + rightInput[i]);
        }
        
        DspFloatType* centerInput = mCenterBuffer.data();

        DspFloatType* leftOutput  = outputs[0];
        DspFloatType* rightOutput = outputs[1];
        DspFloatType* centerOutput = mCenterBuffer.data();

        mLeftDelay.ProcessBlock(n,leftInput  , leftOutput);
        mCenterDelay.ProcessBlock(n,centerInput, centerOutput);
        mRightDelay.ProcessBlock(n,rightInput , rightOutput);

        DspFloatType volume = getVolume();
        for(size_t i = 0; i < n; i++)
        {
            outputs[0][i] += centerOutput[n];
            outputs[0][i] *= volume;
            outputs[1][i] += centerOutput[n];
            outputs[1][i] *= volume;
        }
    }    
};