/*
The following code was taken from the following post:
https://www.musicdsp.org/en/latest/Filters/24-moog-vcf.html

A C++ class implementation of the filter was posted as a comment by moc.erehwon@tsaot.

That class was revised and added to for this implementation:

 - The processBlock function was added for easy use in JUCE projects.
 - Highpass and Bandpass filtering have been added as options
 - A saturation effect (originaly found in the StilsonMoogFilter class)
   was applied.
*/

#include "MoogFilterI.h"

void MoogFilterI::init(float sampleRate)
{
	this->sampleRate = sampleRate;

	out1 = out2 = out3 = out4 = 0.0f;
	in1 = in2 = in3 = in4 = 0.0f;
	calculateCoefficients();
};

void MoogFilterI::setCutoff(float cutoff)
{
	this->cutoff = cutoff;
	calculateCoefficients();
}

void MoogFilterI::setResonance(float resonance)
{
	if (resonance >= 0.0 && resonance <= 1.0)
	{
		this->resonance = resonance;
		calculateCoefficients();
	}
}

void MoogFilterI::setSaturation(float saturationAmount)
{
	if (saturationAmount >= 0.0 && saturationAmount <= 1.0)
	{
		this->saturationAmount = saturationAmount;
	}
}

void MoogFilterI::processBlock(const AudioSourceChannelInfo& bufferToFill, int passMode)
{
	int startSample = bufferToFill.startSample;
	int numSamples = bufferToFill.numSamples;
	AudioSampleBuffer* buffer = bufferToFill.buffer;

	for (int s = 0; s < numSamples; s++)
	{
		buffer->setSample(0, startSample + s, process(buffer->getSample(0, startSample + s), passMode));
	}
}

void MoogFilterI::calculateCoefficients()
{
	float f = (cutoff + cutoff) / this->sampleRate;
	p = f * (1.8f - 0.8f * f);
	k = p + p - 1.f;

	float t = (1.f - p) * 1.386249f;
	float t2 = 12.f + t * t;
	r = resonance * (t2 + 6.f * t) / (t2 - 6.f * t);
};

float MoogFilterI::process(float input, int passMode)
{
	// process input
	input = saturate(input);
	input -= r * out4;

	//Four cascaded onepole filters (bilinear transform)
	out1 = input * p + in1 * p - k * out1;
	out2 =  out1 * p + in2 * p - k * out2;
	out3 =  out2 * p + in1 * p - k * out3;
	out4 =  out3 * p + in4 * p - k * out4;

	in1 = input;
	in2 = out1;
	in3 = out2;
	in4 = out3;

	//Clipper band limited sigmoid
	out4 -= (out4 * out4 * out4) / 6.f;

	switch (passMode)
	{
	case LOWPASS:
		return out4;
		break;
	case HIGHPASS:
		return input - out4 - out1;
		break;
	case BANDPASS:
		return out4 - out1;
		break;
	default:
		return out4;
	}
}

float MoogFilterI::saturate(float input)
{
	float drySignal = input;
	input *= 1.5f;

	float x1 = fabsf(input + saturationLimit);
	float x2 = fabsf(input - saturationLimit);
	float wetSignal = (0.5f * (x1 - x2));
	
	return (wetSignal * saturationAmount) + (drySignal * (1.0f - saturationAmount));
}