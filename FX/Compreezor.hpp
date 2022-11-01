#pragma once

#include "CEnvelopeDetector.hpp"
#include "Undenormal.hpp"
#include <algorithm>
#include <cmath>

extern DspFloatType sampleRate;

namespace FX::Dynamics
{
	struct Compreezor : public StereoFXProcessor
	{

		Compreezor(DspFloatType sampleRate, int samplesPerBlock);
			
		DspFloatType DetGain=1;  //Input Gain in dB    
		DspFloatType Threshold=0; //Compressor Threshold
		DspFloatType AttackTime=10; //Attack Time in Milliseconds
		DspFloatType ReleaseTime=200; //Release Time in Milliseconds
		DspFloatType Ratio=4; //Compression Ratio
		DspFloatType OutputGain=1; //Output Gain in dB
		DspFloatType KneeWidth=0; //Compressor Knee Width
		bool  DigitalAnalogue = false;; //Digital/Analogue style compression


		int DETECT_MODE_PEAK = 0;
		int DETECT_MODE_MS = 1;
		int DETECT_MODE_RMS = 2;

		CEnvelopeDetector  m_LeftDetector;
		CEnvelopeDetector m_RightDetector;

		DspFloatType calcCompressorGain(DspFloatType fDetectorValue, DspFloatType fThreshold,DspFloatType fRatio, DspFloatType fKneeWidth, bool bLimit);
		)
		void ProcessBlock(size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs);
	};

	void Compreezor::ProcessBlock(size_t numSamples,DspFloatType ** inputs, DspFloatType ** outputs)
	{
		Undenormal noDenormals;
		const int totalNumInputChannels = 2;
		const int totalNumOutputChannels = 2;
		DspFloatType fInputGain = std::pow(10.0, DetGain / 20.0);
		DspFloatType fOutputGain = std::pow(10.0, OutputGain / 20.0);

		
		DspFloatType* channelDataL = inputs[0];
		DspFloatType* channelDataR = inputs[1];
		DspFloatType* channelOutL  = outputs[0];
		DspFloatType* channelOutR  = outputs[1];

		for (int sample = 0; sample < numSamples; ++sample)
		{       // ..do something to the data...			    

			channelOutL[sample] = channelDataL[sample] * DetGain;
			channelOutR[sample] = channelDataR[sample] * DetGain;
			
			// detect left channel
			DspFloatType fLeftDetector = m_LeftDetector.detect(channelOutL[sample]);
			// gain calc
			DspFloatType fGn;
			// branch

			//if (m_uProcessorType == COMP) //always true for this project
			fGn = calcCompressorGain(fLeftDetector, Threshold, Ratio, KneeWidth, false);
			// form output and apply make up gain
			channelOutL[sample] = fGn*channelOutL[sample] * OutputGain;

			DspFloatType fRightDetector = m_RightDetector.detect(channelOutR[sample]);

			fGn = calcCompressorGain(fRightDetector, Threshold, Ratio, KneeWidth, false);
			// form output and apply make up gain
			channelOutR[sample] = fGn*channelOutR[sample] * OutputGain;
		}

	}

	DspFloatType Compreezor::calcCompressorGain(DspFloatType fDetectorValue, DspFloatType fThreshold,
		DspFloatType fRatio, DspFloatType fKneeWidth, bool bLimit)
	{
		// slope variable
		DspFloatType CS = 1.0 - 1.0 / fRatio; // [ Eq. 13.1 ]
									// limiting is infinite ratio thus CS->1.0
									//if (bLimit)
									//CS = 1;

									// soft-knee with detection value in range?
		if (fKneeWidth > 0 && fDetectorValue > (fThreshold - fKneeWidth / 2.0) &&
			fDetectorValue < fThreshold + fKneeWidth / 2.0)
		{
			// setup for Lagrange
			DspFloatType x[2];
			DspFloatType y[2];
			x[0] = fThreshold - fKneeWidth / 2.0;
			x[1] = fThreshold + fKneeWidth / 2.0;
			x[1] = std::min(0.0, x[1]); // top limit is 0dBFS
			y[0] = 0; // CS = 0 for 1:1 ratio
			y[1] = CS; // current CS

					// interpolate & overwrite CS
			CS = lagrpol(&x[0], &y[0], 2, fDetectorValue);
		}
		// compute gain; threshold and detection values are in dB
		DspFloatType yG = CS * (fThreshold - fDetectorValue); // [ Eq. 13.1 ]
													// clamp; this allows ratios of 1:1 to still operate
		yG = std::min(0.0f, yG);

		// convert back to linear
		return std::pow(10.0, yG / 20.0);
	}

	//==============================================================================
	Compreezor::Compreezor(DspFloatType sampleRate, int samplesPerBlock) : StereoFXProcessor()
	{
		// Use this method as the place to do any pre-playback
		// initialisation that you need..
		// init the envelope detectors
		// set all params at once with this function; see function definition

		

		if (DigitalAnalogue == true) //Digit
		{
			m_LeftDetector.init((DspFloatType)sampleRate, AttackTime = 0.0, ReleaseTime,
				false, DETECT_MODE_RMS, true);
			m_RightDetector.init((DspFloatType)sampleRate, AttackTime,
				ReleaseTime, false, DETECT_MODE_RMS, true);
		}
		else
		{
			m_LeftDetector.init((DspFloatType)sampleRate, AttackTime = 0.0, ReleaseTime,
				true, DETECT_MODE_RMS, true);
			m_RightDetector.init((DspFloatType)sampleRate, AttackTime,
				ReleaseTime, true, DETECT_MODE_RMS, true);
		}

	}
}