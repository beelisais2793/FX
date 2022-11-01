/*
The following code was taken from the following post:
https://www.musicdsp.org/en/latest/Filters/24-moog-vcf.html

A C++ class implementation of the filter was posted as a comment by moc.erehwon@tsaot.

That class was revised and added to for this implementation:
 - several variable names were changed for better readability
 - The processBlock function was added for easy use in JUCE projects.
 - Highpass and Bandpass filtering have been added as options
 - A saturation effect was applied. 
   (adapted from the saturation function in the StilsonMoogFilter class)
*/


#pragma once


#define LOWPASS 0
#define HIGHPASS 1
#define BANDPASS 2

namespace Analog::Filters::Moog::MoogFilterI
{
	struct MoogFilterI : public FilterProcessor
	{
		MoogFilterI() : FilterProcessor() {
			init(sampleRate);
			setCutoff(1000.0);
			setResonance(0.01);
		}
		
		void init(double sampleRate);
		
		void setCutoff(double cutoff);
		void setResonance(double resonance);
		
		// saturationAmount ranges [0..1] and is a dry/wet ratio
		// 0.0 will effectively turn off the saturation
		void setSaturation(double saturationAmount);

		void ProcessBlock(size_t n, float * input, float * output);
	
		void calculateCoefficients();
		double process(double x);
		double saturate(double input);

		double Tick(double I, double A=1, double X=1, double Y=1) {
			return A*process(I);
		}

		int passMode = LOWPASS;
		double cutoff;
		double resonance;
		double saturationAmount;
		
		double sampleRate;
		double out1, out2, out3, out4;
		double in1, in2, in3, in4;

		// coefficients determined by cutoff and resonance
		double r, p, k;

		const double saturationLimit = 0.95;
	};

	
	void MoogFilterI::init(double sampleRate)
	{
		this->sampleRate = sampleRate;

		out1 = out2 = out3 = out4 = 0.0f;
		in1 = in2 = in3 = in4 = 0.0f;
		calculateCoefficients();
	};

	void MoogFilterI::setCutoff(double cutoff)
	{
		this->cutoff = cutoff;
		calculateCoefficients();
	}

	void MoogFilterI::setResonance(double resonance)
	{
		if (resonance >= 0.0 && resonance <= 1.0)
		{
			this->resonance = resonance;
			calculateCoefficients();
		}
	}

	void MoogFilterI::setSaturation(double saturationAmount)
	{
		if (saturationAmount >= 0.0 && saturationAmount <= 1.0)
		{
			this->saturationAmount = saturationAmount;
		}
	}

	void MoogFilterI::ProcessBlock(size_t n, float * inputs, float * outputs)
	{
		for (int s = 0; s < n; s++)
		{
			outputs[s] =  process(inputs[s]);
		}
	}

	void MoogFilterI::calculateCoefficients()
	{
		double f = (cutoff + cutoff) / this->sampleRate;
		p = f * (1.8f - 0.8f * f);
		k = p + p - 1.f;

		double t = (1.f - p) * 1.386249f;
		double t2 = 12.f + t * t;
		r = resonance * (t2 + 6.f * t) / (t2 - 6.f * t);
	};

	double MoogFilterI::process(double input)
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

	
	double MoogFilterI::saturate(double input)
	{
		double drySignal = input;
		input *= 1.5f;

		double x1 = fabsf(input + saturationLimit);
		double x2 = fabsf(input - saturationLimit);
		double wetSignal = (0.5f * (x1 - x2));
		
		return (wetSignal * saturationAmount) + (drySignal * (1.0f - saturationAmount));
	}
}
#undef LOWPASS
#undef HIGHPASS
#undef BANDPASS