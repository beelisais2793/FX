/*
  ==============================================================================

    FIR lowpass filter.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class FirLpf
{
public:
	FirLpf(int fs, int numChannels, int samplesPerFame, int order);
	~FirLpf();

    void process(AudioBuffer<float>& buff);
    
    void setCutoff(const float& fc);

private:

	int fs;
    int numChannels;
    int samplesPerFrame;
    
	AudioParameterFloat* cutoff;
	int filterOrder;

	//dsp::ProcessSpec spec;
	//dsp::AudioBlock<float> block;
	dsp::WindowingFunction<float>::WindowingMethod filterWindow;
	dsp::ProcessorDuplicator<dsp::FIR::Filter<float>, dsp::FIR::Coefficients<float>> filter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FirLpf);
};
