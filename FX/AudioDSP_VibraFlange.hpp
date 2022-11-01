// https://github.com/joonastuo/VibraFlanger
// haven't finished porting it

#pragma once

#include "SoundObject.hpp"
#include <array>
#include <cmath>
#include "AudioDSP_DelayLine.hpp"
#include "AudioDSP_Lfo.hpp"

namespace JoonasFX
{
	struct VibraFlange : public StereoFXProcessor
	{
	
		VibraFlange();
		~VibraFlange() = default;	

		void reset();
		void ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs);

		void setWet(DspFloatType w) {
			W = w;
			G = 1 - W;
		}
		void setFeedback(DspFloatType fb) {
			FB = fb;
		}
		void setFrequency(DspFloatType f) {
			lfoFreq = f;
			mLFOs[0].setFreq(f);
			mLFOs[1].setFreq(f);
		}
		void setLfoWaveform(LFO::Waveforms w) {			
			mLFOs[0].setWaveform(w);
			mLFOs[1].setWaveform(w);
		}
		void power(bool on = true) {
			on_button = on;
		}

		enum {
			PORT_WET,
			PORT_FEEDBACK,
			PORT_FREQUENCY,
			PORT_LFOWAVEFORM,
			PORT_POWER
		};

		void printPorts() {
			printf("PORT_WET=0\n");
			printf("PORT_FEEDBACK=1\n");
			printf("PORT_FREQUENCY=2\n");
			printf("PORT_LFOWAVEFORM=3\n");
			printf("PORT_POWER=4\n");
		}
		void setPort(int port, DspFloatType v) {
			switch(port)
			{
				case PORT_WET: setWet(v); break;
				case PORT_FEEDBACK: setFeedback(v); break;
				case PORT_LFOWAVEFORM: setLfoWaveform((LFO::Waveforms)v); break;
				case PORT_POWER: power((bool)v); break;
			}
		}

		// Methods
		void modDelay(size_t,const DspFloatType* input, DspFloatType* output, const int& channel);
		void update();
		DspFloatType linearInterp(const DspFloatType& y0, const DspFloatType& yp1, const DspFloatType& frac);
		
		DspFloatType getFeedback() { return FB; }
		DspFloatType getWetness() { return W; }	
		DspFloatType getLfoFreq() { return lfoFreq; }
		
		
		// Initialise values for two delay lines
		DelayLine<DspFloatType> delayLines[2];
		LFO mLFOs[2];
		bool on_button = true;
		
		DspFloatType   lfoFreq = 0.0025;
		
		DspFloatType FB,W,G;
		DspFloatType mSampleRate = 44100.f;	
		int mDelayBufferLen = 0;
		int mNumChannels = 2;
		DspFloatType mDelayTimeDepth = 5;
		DspFloatType sweepDepth = 0.95;

		void randomize()
		{
			Random r;
			lfoFreq = r.frand()*5;
			FB = r.frand();
			W = r.frand();
			G = 1-W;
			mDelayTimeDepth = r.frand()*20;
			sweepDepth = r.frand();
		}
	};

	inline VibraFlange::VibraFlange()
	{
		// Set specs to class variables
		//mSampleRate      = sampleRate;	
		//mNumChannels     = 2;

		// Prepare delay lines
		mDelayBufferLen  = static_cast<int>(2 * mSampleRate);

		for (auto& dline : delayLines)
		{
			dline.resize(mDelayBufferLen);
			dline.clear();
		}
		
		// Set phase shift to left channel
		mLFOs[0].setPhase(0.25);

		for (auto& lfo : mLFOs)
		{		
			lfo.setWaveform(LFO::Waveforms::sine);
			lfo.setUnipolar(true);
			lfo.setFreq(lfoFreq);
		}
		
		
		FB = 0.5;
		W  = 0.9;
		G  = 1 - W;
	}

	inline void VibraFlange::ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
	{
		Undenormal dn;
		if(on_button == false) {
			for(size_t i = 0; i < 2; i++) memcpy(outputs[i],inputs[i],n*sizeof(DspFloatType));
			return;
		}
		// Update parameters
		//update();
		
		const DspFloatType* leftInput  = inputs[0];
		const DspFloatType* rightInput = inputs[1];
		
		DspFloatType* leftOutput  = outputs[0];
		DspFloatType* rightOutput = outputs[1];

		modDelay(n,leftInput,  leftOutput,  0);
		modDelay(n,rightInput, rightOutput, 1);
	}

	inline void VibraFlange::modDelay(size_t n, const DspFloatType * input, DspFloatType * output, const int& dline)
	{
		// Get channel LFO
		auto& lfo = mLFOs[dline];


		// Iterate trough samples in a channel
		for (auto sample = 0; sample < n; ++sample)
		{
			// Get delay amount
			DspFloatType delayTime = mDelayTimeDepth * sweepDepth * lfo.getValue();
			lfo.advanceSamples(1);
			DspFloatType delayInSamplesFrac = static_cast<DspFloatType>(mSampleRate * (delayTime / 1000.f));
			int delayInSamplesInt = static_cast<int> (delayInSamplesFrac);
			DspFloatType delayFrac = delayInSamplesFrac - delayInSamplesInt;

			// Get delayed sample
			DspFloatType delayedSample = 0.f;
			// Fractal delay with liner interpolation if neede
			if (delayFrac != 0)
			{
				DspFloatType y0  = delayLines[dline].get(delayInSamplesInt);
				DspFloatType ym1 = delayLines[dline].get(delayInSamplesInt + 1);
				delayedSample = y0 + delayFrac*(ym1-y0); 
			}
			// If no frac
			else
			{
				delayedSample = delayLines[dline].get(delayInSamplesInt);
			}

			auto inputSample = input[sample];

			// Add input and feedback to delayLine
			delayLines[dline].push(inputSample + delayedSample * FB);

			// Mix dry and wet input
			output[sample] = inputSample * G + delayedSample * W;
		}
	}
}