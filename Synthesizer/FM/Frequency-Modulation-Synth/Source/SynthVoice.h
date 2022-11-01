/*
  ==============================================================================

    SynthVoice.h
    Created: 18 Apr 2019 3:22:30am
    Author:  quasa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "Maximilian.h"

/**
	Represents a voice that a Synthesiser can use to play a SynthesiserSound.

	A voice plays a single sound at a time, and a synthesiser holds an array of
	voices so that it can play polyphonically.

	@see Synthesiser, SynthesiserSound

	@tags{Audio}
*/

class SynthVoice : public SynthesiserVoice {
public:
	SynthVoice() :
		opFrequency(1000),
		opFreqFixed(1000),
		opBFrequency(1000),
		opBFreqFixed(1000),
		opCFrequency(1000),
		opCFreqFixed(1000),
		opDFrequency(1000),
		opDFreqFixed(1000)
	{
		opFreqRatio = 0;
		opBFreqRatio = 0;
		opCFreqRatio = 0;
		opDFreqRatio = 0;

		opModIndex = 0;
		opBModIndex = 0;
		opCModIndex = 0;
		opDModIndex = 0;

		opToggleFreq = 0;
		opBToggleFreq = 0;
		opCToggleFreq = 0;
		opDToggleFreq = 0;
	}

	bool canPlaySound(SynthesiserSound *sound) override
	{
		// cast sound to synthsound, if it can't, don't play
		return dynamic_cast<SynthSound*> (sound) != nullptr;
	}

	double setCarrier(int &waveType, maxiOsc &osc, double &freq, double modulator , double &modIndex)
	{
		switch(waveType)
		{
		case 0:
			return osc.sinewave(freq + modulator * modIndex);
		case 1:
			return osc.sawn(freq + modulator * modIndex);
		case 2:
			return osc.square(freq + modulator * modIndex);
		case 3:
			return osc.triangle(freq + modulator * modIndex);
		default:
			return osc.sinewave(freq + modulator * modIndex);
		}
	}

	double setCarrier(int &waveType, maxiOsc &osc, double &freq, double modulator1, double modulator2, double &modIndex1, double &modIndex2)
	{
		switch (waveType)
		{
		case 0:	
			return osc.sinewave(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		case 1:
			return osc.sawn(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		case 2:
			return osc.square(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		case 3:
			return osc.triangle(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		default:
			return osc.sinewave(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		}
	}

	//double setCarrier()
	//{
	//	switch (opOscType)
	//	{
	//	case 0:
	//		return oscA.sinewave(opFreq + setModulator(opBOscType, oscB, opBFreq) * opBModIndex);
	//	case 1:
	//		return oscA.saw(opFreq + setModulator(opBOscType, oscB, opBFreq) * opBModIndex);
	//	case 2:
	//		return oscA.square(opFreq + setModulator(opBOscType, oscB, opBFreq) * opBModIndex);
	//	case 3:
	//		return oscA.triangle(opFreq + setModulator(opBOscType, oscB, opBFreq) * opBModIndex);
	//	default:
	//		return oscA.sinewave(opFreq + setModulator(opBOscType, oscB, opBFreq) * opBModIndex);
	//
	//	}
	//}

	double setModulator(int &waveType, maxiOsc &osc, double &freq)
	{

		switch (waveType)
		{
		case 0:
			return osc.sinewave(freq);
		case 1:
			return osc.sawn(freq);
		case 2:
			return osc.pulse(freq, .5);
		case 3:
			return osc.triangle(freq);
		default:
			return osc.sinewave(freq);
		}
	}

	double setModulator(int &waveType, maxiOsc &osc, double &freq, float &adsr)
	{
		
		switch (waveType)
		{
		case 0:
			return adsr * osc.sinewave(freq);
		case 1:
			return adsr * osc.sawn(freq);
		case 2:
			return adsr * osc.pulse(freq, .5);
		case 3:
			return adsr * osc.triangle(freq);
		default:
			return adsr * osc.sinewave(freq);
		}
	}

	double setModulator(int &waveType, maxiOsc &osc, double &freq, double modulator, double &modIndex, float &adsr)
	{
		switch (waveType)
		{
		case 0:
			return adsr * osc.sinewave(freq + modulator * modIndex);
		case 1:
			return adsr * osc.sawn(freq + modulator * modIndex);
		case 2:
			return adsr * osc.pulse(freq + modulator * modIndex, .5);
		case 3:
			return adsr * osc.triangle(freq + modulator * modIndex);
		default:
			return adsr * osc.sinewave(freq + modulator * modIndex);
		}
	}

	double setModulator(int &waveType, maxiOsc &osc, double &freq, double modulator1, double modulator2, double &modIndex1, double &modIndex2, float &adsr)
	{
		switch (waveType)
		{
		case 0:
			return adsr * osc.sinewave(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		case 1:
			return adsr * osc.sawn(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		case 2:
			return adsr * osc.pulse(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2), .5);
		case 3:
			return adsr * osc.triangle(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		default:
			return adsr * osc.sinewave(freq + (modulator1 * modIndex1) + (modulator2 * modIndex2));
		}
	}

	void setFrequency(double &baseFrequency, double &freqRatio, double &targetFrequency, int &fixedFreq)
	{
		if (fixedFreq != 1)
		{
			targetFrequency = pow(10, freqRatio / 3986.45) * baseFrequency;
		}
	}

	double setAlgorithm()
	{
		switch (algorithm)
		{
		case 0: 
			// single carrier (A)
			// D -> C -> B -> A
			setFrequency(opFrequency, opBFreqRatio, opBFrequency, opBToggleFreq);
			setFrequency(opBFrequency, opCFreqRatio, opCFrequency, opCToggleFreq);
			setFrequency(opCFrequency, opDFreqRatio, opDFrequency, opDToggleFreq);
			//return adsrValue * setCarrier(opOscType, oscA, opFrequency, setModulator(opBOscType, oscB, opBFrequency, adsrBValue), opBModIndex);
			//return (setCarrier(opOscType, oscA, opFrequency,
			//	setModulator(opBOscType, oscB, opBFrequency, setModulator(opCOscType, oscC, opCFrequency), opCModIndex), opBModIndex));
			return adsrValue *(setCarrier(opOscType, oscA, opFrequency,
				setModulator(opBOscType, oscB, opBFrequency, 
					setModulator(opCOscType, oscC, opCFrequency, 
						setModulator(opDOscType, oscD, opDFrequency, adsrDValue), opDModIndex, adsrCValue), opCModIndex, adsrBValue), opBModIndex));

		case 1:
			// single carrier (a)
			// C -
			//	   \
			//		->	A
			//	   /
			// B -
			setFrequency(opFrequency, opBFreqRatio, opBFrequency, opBToggleFreq);
			setFrequency(opFrequency, opCFreqRatio, opCFrequency, opCToggleFreq);
			return adsrValue * setCarrier(opOscType, oscA, opFrequency, setModulator(opBOscType, oscB, opBFrequency, adsrBValue), setModulator(opCOscType, oscC, opCFrequency, adsrCValue), opBModIndex, opCModIndex);
		case 2:
			// double carrier (a + b)
			//	   -> A
			//    /
			// C -
			//    \
			//	   -> B
			return (adsrValue * setCarrier(opOscType, oscA, opFrequency, setModulator(opCOscType, oscC, opCFrequency, adsrCValue), opCModIndex) + adsrBValue * setCarrier(opBOscType, oscB, opBFrequency, setModulator(opCOscType, oscC, opCFrequency, adsrCValue), opCModIndex));
		case 3:
			//
			// D -> B
			// C -> A
			//
			setFrequency(opFrequency, opCFreqRatio, opCFrequency, opCToggleFreq);
			setFrequency(opBFrequency, opDFreqRatio, opDFrequency, opDToggleFreq);

			return (adsrValue * setCarrier(opOscType, oscA, opFrequency, setModulator(opCOscType, oscC, opCFrequency, adsrCValue), opCModIndex) +
					adsrBValue * setCarrier(opBOscType, oscB, opBFrequency, setModulator(opDOscType, oscD, opDFrequency, adsrDValue), opDModIndex));
		case 4:
			//return setCarrier(opOscType, oscA, opFreq, setModulator(opBOscType, oscB, opBFreq), opBModIndex);
		case 5:
			//return setCarrier(opOscType, oscA, opFreq, setModulator(opBOscType, oscB, opBFreq), opBModIndex);
		default:
			return adsrValue * (setCarrier(opOscType, oscA, opFrequency,
				setModulator(opBOscType, oscB, opBFrequency,
					setModulator(opCOscType, oscC, opCFrequency,
						setModulator(opDOscType, oscD, opDFrequency, adsrDValue), opDModIndex, adsrCValue), opCModIndex, adsrBValue), opBModIndex));
		}
	}

	void getOperatorAParams(float* oscType, float* index, float* freq, float* fixedFreq, float* freqRatio)
	{
		opOscType = *oscType;
		opModIndex = *index;
		opFreqFixed = *freq;
		opToggleFreq = *fixedFreq;
		opFreqRatio = *freqRatio;

		//switch ((int)*freqRatio)
		//{
		//case 0:
		//	opFreqRatio = 0;
		//	break;
		//case 1:
		//	opFreqRatio = 1200;
		//	break;
		//case 2:
		//	opFreqRatio = 400;
		//	break;
		//case 3:
		//	opFreqRatio = -1200;
		//	break;
		//case 4:
		//	opFreqRatio = -700;
		//	break;
		//case 5:
		//	break;
		//default:
		//	opFreqRatio = 0;
		//	break;
		//}
	}

	void getOperatorBParams(float* oscType, float* index, float* freq, float* fixedFreq, float* freqRatio)
	{
		opBOscType = *oscType;
		opBModIndex = *index;
		opBFreqFixed = *freq;
		opBToggleFreq = *fixedFreq;
		opBFreqRatio = *freqRatio;
	}

	void getOperatorCParams(float* oscType, float* index, float* freq, float* fixedFreq, float* freqRatio)
	{
		opCOscType = *oscType;
		opCModIndex = *index;
		opCFreqFixed = *freq;
		opCToggleFreq = *fixedFreq;
		opCFreqRatio = *freqRatio;
	}

	void getOperatorDParams(float* oscType, float* index, float* freq, float* fixedFreq, float* freqRatio)
	{
		opDOscType = *oscType;
		opDModIndex = *index;
		opDFreqFixed = *freq;
		opDToggleFreq = *fixedFreq;
		opDFreqRatio = *freqRatio;
	}

	void getAlgorithm(float* algSelection)
	{
		algorithm = *algSelection;
	}

	//==============================================================================

	void setADSRSampleRate(double sampleRate)
	{
		adsrB.setSampleRate(sampleRate);
		adsr.setSampleRate(sampleRate);
	}

	// set envelope params for ADSR envelope
	void getEnvelopeParams(float* attack, float* decay, float* sustain, float* release)
	{
		adsrParams.attack = *attack * 2;
		adsrParams.decay = *decay * 2;
		adsrParams.sustain = *sustain;// *abs(setLfo());
		adsrParams.release = *release * 2;
	}

	void getEnvelopeBParams(float* attack, float* decay, float* sustain, float* release)
	{
		adsrBParams.attack = *attack * 2;
		adsrBParams.decay = *decay * 2;
		adsrBParams.sustain = *sustain;// *abs(setLfo());
		adsrBParams.release = *release * 2;
	}

	void getEnvelopeCParams(float* attack, float* decay, float* sustain, float* release)
	{
		adsrCParams.attack = *attack * 2;
		adsrCParams.decay = *decay * 2;
		adsrCParams.sustain = *sustain;// *abs(setLfo());
		adsrCParams.release = *release * 2;
	}

	void getEnvelopeDParams(float* attack, float* decay, float* sustain, float* release)
	{
		adsrDParams.attack = *attack * 2;
		adsrDParams.decay = *decay * 2;
		adsrDParams.sustain = *sustain;// *abs(setLfo());
		adsrDParams.release = *release * 2;
	}

	//double setEnvelope()
	//{
	//	return setOscType() * level;
	//}

	void getLfoParams(float* lfoOsc, float* lfoRateValue, float* lfoAmountValue)
	{
		lfoOscType = *lfoOsc;
		lfoRate = *lfoRateValue;
		lfoAmount = *lfoAmountValue;
	}

	//double setLfo()
	//{
	//	double lfo;
	//	switch (lfoOscType)
	//	{
	//	case 0:
	//		lfo = oscLFO.sinewave(lfoRate) * lfoAmount;
	//		break;
	//	case 1:
	//		lfo = oscLFO.saw(lfoRate) * lfoAmount * .2;
	//		break;
	//	case 2:
	//		lfo = oscLFO.square(lfoRate) * lfoAmount; 
	//		break;
	//	case 3:
	//		lfo = oscLFO.triangle(lfoRate) * lfoAmount; 
	//		break;
	//	default:
	//		lfo = oscLFO.sinewave(lfoRate) * lfoAmount; 
	//		break;
	//	}
	//	return lfo;
	//}

	// set filter parameters
	//void getFilterParams(float* filterType, float* filterCutOff, float* filterResonance)
	//{
	//	filterSelection = *filterType;
	//	filterCutOffVal = *filterCutOff + (setLfo() * 100);
	//	filterResonanceVal = *filterResonance;
	//}

	void getGain(float* gain)
	{
		masterGain = *gain;
	}

	double setGain()
	{
		return masterGain * level * 0.5;
	}

	void startNote(int midiNoteNumber, 
				   float velocity, 
				   SynthesiserSound* sound, 
				   int currentPitchWheelPosition) override
	{
		// formula to get new frequency based on how much the pitch is changed in cents:
		// F2 = 10 ^ (c / 3986.45) * F1

		(opToggleFreq != 1) ? opFrequency =  MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opFrequency = opFreqFixed;
		(opBToggleFreq != 1) ? opBFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opBFrequency = opBFreqFixed;
		(opCToggleFreq != 1) ? opCFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opCFrequency = opCFreqFixed;
		(opDToggleFreq != 1) ? opDFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opDFrequency = opDFreqFixed;


		//(opToggleFreq != 1) ? opFrequency = pow(10, opFreqRatio / 3986.45) * MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opFrequency = opFreqFixed;
		//(opBToggleFreq != 1) ? opBFrequency = pow(10, opBFreqRatio / 3986.45) * MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opBFrequency = opBFreqFixed;
		//(opCToggleFreq != 1) ? opCFrequency = pow(10, opCFreqRatio / 3986.45) * MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opCFrequency = opCFreqFixed;
		//(opDToggleFreq != 1) ? opDFrequency = pow(10, opDFreqRatio / 3986.45) * MidiMessage::getMidiNoteInHertz(midiNoteNumber) : opDFrequency = opDFreqFixed;

		level = velocity; 
		
		adsr.noteOn();
		adsrB.noteOn();
		adsrC.noteOn();
		adsrD.noteOn();
	}


	void stopNote(float velocity, bool allowTailOff) override
	{
		if (velocity == 0)
			clearCurrentNote();
		allowTailOff = true;
		
		adsr.noteOff();
		adsrB.noteOff();
		adsrC.noteOff();
		adsrD.noteOff();
	}

	//bool isVoiceActive() const override
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	void pitchWheelMoved(int newPitchWheelValue) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	void controllerMoved(int controllerNumber, int newControllerValue) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	//void aftertouchChanged(int newAftertouchValue)
	//{

	//}

	//void channelPressureChanged(int newChannelPressureValue)
	//{

	//}

	// this gets called in ProcessBlock of AudioProcessor object
	void renderNextBlock(AudioBuffer<float>& outputBuffer, 
		int startSample, 
		int numSamples) override
	{
		adsr.setParameters(adsrParams);
		adsrB.setParameters(adsrBParams);
		adsrC.setParameters(adsrCParams);
		adsrD.setParameters(adsrDParams);

		// loop through all of the samples
		while (--numSamples >= 0)
		{
			adsrValue = adsr.getNextSample();
			adsrBValue = adsrB.getNextSample();
			adsrCValue = adsrC.getNextSample();
			adsrDValue = adsrD.getNextSample();

			double sound = setAlgorithm() * setGain();

			// fm synthesis

			// multiply wave by level (which is velocity)
			// in other words, make level equal to key velocity

			// loop through channels
			 for (int i = outputBuffer.getNumChannels(); --i >= 0;)
				 outputBuffer.addSample(i, startSample, sound);

			++startSample;
		}
	}	
	
	//void setCurrentPlaybackSampleRate(double newRate)
	//{

	//}

	//bool isPlayingChannel(int midiChannel) const
	//{
	//	return true;
	//}


private:

	//double tailOff = 0.0;
	//double angleDelta = 0.0;
	//double currentAngle = 0.0;

	maxiOsc oscA, oscB, oscC, oscD;
	maxiOsc oscLFO;
	
	double level;

	// JUCE ADSR envelope
	ADSR adsr, adsrB, adsrC, adsrD;
	ADSR::Parameters adsrParams, adsrBParams, adsrCParams, adsrDParams;
	float adsrValue, adsrBValue, adsrCValue, adsrDValue;

	// filter variables
	int filterSelection;
	float filterCutOffVal;
	float filterResonanceVal;

	// LFO variables
	int lfoOscType;
	float lfoRate;
	float lfoAmount;

	// Operator variables
	int opOscType, opToggleFreq;
	double opFreqFixed,opFreqRatio, opFrequency, opModIndex;

	int opBOscType, opBToggleFreq;
	double opBFreqFixed, opBFreqRatio, opBFrequency, opBModIndex;

	int opCOscType, opCToggleFreq;
	double opCFreqFixed, opCFreqRatio, opCFrequency, opCModIndex;

	int opDOscType, opDToggleFreq;
	double opDFreqFixed, opDFreqRatio, opDFrequency, opDModIndex;

	int algorithm = 0;

	float masterGain;

};
