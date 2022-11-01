/*
  ==============================================================================
    Phaser.h
    Created: 18 Mar 2019 3:36:31pm
    Author:  Joonas
  ==============================================================================
*/

#pragma once

#pragma once

#include <vector>
#include "SoundObject.hpp"
#include "AudioDSP_FirstOrderAllPass.hpp"
#include "AudioDSP_DCBlocker.hpp"
#include "AudioDSP_Lfo.hpp"

namespace JoonasFX {
	struct StereoVector
	{
		std::vector<DspFloatType> vec[2];

		StereoVector(size_t n=1024)
		{
			vec[0].resize(n);
			vec[1].resize(n);
		}

		void resize(size_t n) {
			vec[0].resize(n);
			vec[1].resize(n);
		}

		std::vector<DspFloatType>& getLeft() { return vec[0]; }
		std::vector<DspFloatType>& getRight() { return vec[1]; }

		void setLeft(std::vector<DspFloatType> L) { vec[0] = L; }
		void setRight(std::vector<DspFloatType> R) { vec[1] = R; }

		void setChannel(size_t ch, size_t n, DspFloatType * data) {        
			memcpy(vec[ch %2].data(),data,n*sizeof(DspFloatType));
		}
		DspFloatType& operator()(size_t channel, size_t index) {
			return vec[channel % 2][index];
		}

	};

	struct Phaser : public StereoFXProcessor
	{

		Phaser() : StereoFXProcessor(), mDryBuffer(1024) { prepare(); }
		~Phaser() = default;

		void prepare(DspFloatType sampleRate = 44100.0f);
		void ProcessBlock(size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs);

		// Private methods
		DspFloatType getCoeff(const int& channel);
		DspFloatType calcCoeff(const DspFloatType& fc);
		
		void power(bool p = true) { on_button = p; }
		
		void randomize()
		{
			Random r;
			lfoFreq = r.frand()*5;
			mFeedback[0] = r.frand();
			mFeedback[1] = r.frand();
			W = r.frand();
			G = 1-W;
			FB = r.frand();
		}
		enum {
			PORT_RANDOM,
			PORT_WAVEFORM,
			PORT_LFOFREQ,
			PORT_UNIPOLAR,
			PORT_FEEDBACK0,
			PORT_FEEDBACK1,
			PORT_WET,
			PORT_FEEDBACK,
			PORT_ONOFF,
			PORT_FCFIRSTLAST,
			PORT_FCMINTRI,
			PORT_FCMAXTRI,
			PORT_FCMINSIN,
			PORT_FCMAXSIN,
		};
		void printPorts()
		{
			printf("PORTS\n");
			printf("PORT_WAVEFORM=0\n");
			printf("PORT_LFOFREQ=1\n");
			printf("PORT_FEEDBACK0=2\n");
			printf("PORT_FEEDBACK1=3\n");
			printf("PORT_WET=4\n");
			printf("PORT_FEEDBACK=5\n");
			printf("PORT_ONOFF=6\n");
			printf("PORT_FCFIRSTLAST=7\n");
			printf("PORT_FCMINTRI=8\n");
			printf("PORT_FCMAXTRI=9\n");
			printf("PORT_FCMINSIN=10\n");
			printf("PORT_FCMAXSIN=11\n");
			printf("PORT_UNIPOLAR=12\n");
		}
		void setPort(int port, DspFloatType v) {
			switch(port)
			{
				case PORT_RANDOM: if(v != 0.0) randomize(); break;
				case PORT_WAVEFORM: lfoWaveform = (LFO::Waveforms)((int)v); mLFO.setWaveform(lfoWaveform); break;
				case PORT_LFOFREQ: lfoFreq = v; mLFO.setFreq(lfoFreq); break;
				case PORT_UNIPOLAR: mLFO.setUnipolar((bool)v); break;
				case PORT_FEEDBACK0: mFeedback[0] = v; break;
				case PORT_FEEDBACK1: mFeedback[1] = v; break;
				case PORT_WET: W = v; G = 1-W; break;
				case PORT_FEEDBACK: FB = v; break;
				case PORT_ONOFF: on_button = (bool)v; break;
				case PORT_FCFIRSTLAST: mFcFirstLast = v; break;
				case PORT_FCMINTRI: mFcMinTri = v; mTriA = mFcMaxTri - mFcMinTri;  break;
				case PORT_FCMAXTRI: mFcMaxTri = v; mTriA = mFcMaxTri - mFcMinTri;  break;
				case PORT_FCMINSIN: mFcMinSin = v; mSinA = mFcMaxSin - mFcMinSin;  break;
				case PORT_FCMAXSIN: mFcMaxSin = v; mSinA = mFcMaxSin - mFcMinSin;  break;
			}
		}
		bool on_button = true;
		DCBlocker mDCBlocker;
		std::array<FirstOrderAllPass, 10> mAPFilters;
		std::array<LFO, 2> mLFOs;
		LFO mLFO;
		DspFloatType lfoFreq = 0.05;
		StereoVector mDryBuffer;
		DspFloatType mSampleRate = 44100.f;
		// Array to store feedback values
		DspFloatType mFeedback[2] = { 0.f };
		// Center frequency of A1, A2, A9, A10
		DspFloatType mFcFirstLast = 816.075f;
		// Center frequency of mod AP filters
		// Triangular
		DspFloatType mFcMinTri = 1217.59f;
		DspFloatType mFcMaxTri = 5805.80f;
		DspFloatType mTriA = mFcMaxTri - mFcMinTri;
		// Rectified sine
		DspFloatType mFcMinSin = 4629.31f;
		DspFloatType mFcMaxSin = 20236.1f;
		DspFloatType mSinA = mFcMaxSin - mFcMinSin;

		int maximumBlockSize = 1024;
		size_t numChannels = 2;
		LFO::Waveforms lfoWaveform = LFO::Waveforms::tri;
		DspFloatType W = 0.9;
		DspFloatType G = 0.1;
		DspFloatType FB = 0.9;
	};

	void Phaser::prepare(DspFloatType sampleRate)
	{
		// Prepare class members
		mSampleRate = sampleRate;
		mDryBuffer.resize(maximumBlockSize);

		// Prepare DCBlocker
		mDCBlocker.prepare(numChannels);

		// Prepare LFOs
		lfoWaveform = LFO::Waveforms::tri;

		for (auto& lfo : mLFOs)
		{		
			lfo.setUnipolar(true);
			lfo.setWaveform(lfoWaveform);
			lfo.setFreq(lfoFreq);
		}

		// Prepare AP filters
		// Coefficient for A1, A2, A9 and A10
		DspFloatType coeffFirstLast = calcCoeff(mFcFirstLast);
		// Coefficient for modulated filters
		DspFloatType fc = mFcMinTri + mTriA * mLFO.getValue();
		DspFloatType coeffMod = calcCoeff(fc);

		for (auto i = 0; i < mAPFilters.size(); ++i)
		{
			if (i == 0 || i == 1 || i == 8 || i == 9)
			{
				mAPFilters[i].prepare(numChannels);
				mAPFilters[i].updateCoefficients(coeffFirstLast);
			}
			else
			{
				mAPFilters[i].prepare(numChannels);
				mAPFilters[i].updateCoefficients(coeffMod);
			}
		}
	}


	void Phaser::ProcessBlock(size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
	{
		Undenormal dn;
		if(on_button == false) {
			for(size_t i = 0; i < 2; i++) memcpy(outputs[i],inputs[i],numSamples*sizeof(DspFloatType));
			return;
		}
		prepare();
		// DC Blocker ============================	
		mDCBlocker.ProcessBlock(numSamples,inputs,outputs);

		// Copy current buffer to dry buffer =====
		for (auto channel = 0; channel < numChannels; ++channel)
		{
			mDryBuffer.setChannel(channel,numSamples, outputs[channel]);
		}

		// AP Filters ============================
		for (auto channel = 0; channel < numChannels; ++channel)
		{
			const DspFloatType* input = outputs[channel];
			DspFloatType* output = outputs[channel];

			for (auto sample = 0; sample < numSamples; ++sample)
			{
				DspFloatType outputSample = 0.f;
				DspFloatType inputSample = input[sample] + mFeedback[channel] * FB;

				// Update mod AP filter coefficients
				DspFloatType coeff = getCoeff(channel);
				for (auto i = 2; i < 8; ++i)
				{
					mAPFilters[i].updateCoefficients(coeff);
				}
				// Process sample trough AP filters
				for (auto i = 0; i < mAPFilters.size(); ++i)
				{
					outputSample = mAPFilters[i].Tick(inputSample, channel);
					inputSample  = outputSample;
				}
				output[sample]     = mDryBuffer.vec[channel][sample] * G + outputSample * W;
				mFeedback[channel] = outputSample;
			}
		}	
	}


	//==============================================================================
	DspFloatType Phaser::getCoeff(const int& channel)
	{
		DspFloatType coeff = 0.f;
		auto& lfo = mLFOs[channel];
		
		// Rectified sine wave
		if (lfoWaveform != LFO::Waveforms::tri)
		{
			lfo.setWaveform(LFO::Waveforms::rectSine);
			DspFloatType fc = mFcMinSin + mSinA * lfo.getValue();
			lfo.advanceSamples(1);
			coeff = calcCoeff(fc);
		}
		// Triangular
		else
		{
			lfo.setWaveform(LFO::Waveforms::tri);
			DspFloatType fc = mFcMinTri + mTriA * lfo.getValue();
			lfo.advanceSamples(1);
			coeff = calcCoeff(fc);
		}
		return coeff;
	}

	//==============================================================================
	DspFloatType Phaser::calcCoeff(const DspFloatType & fc)
	{
		return static_cast<DspFloatType>(-1.f * (1.f - tan(2.f * M_PI * fc * ((1.f / mSampleRate) / 2.f))) / (1.f + tan(2.f * M_PI * fc * ((1.f / mSampleRate) / 2.f))));
	}
}