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
		
		void init(DspFloatType sampleRate);
		
		void setCutoff(DspFloatType cutoff);
		void setResonance(DspFloatType resonance);
		
		// saturationAmount ranges [0..1] and is a dry/wet ratio
		// 0.0 will effectively turn off the saturation
		void setSaturation(DspFloatType saturationAmount);

		void ProcessBlock(size_t n, DspFloatType * input, DspFloatType * output);
	
		void calculateCoefficients();
		DspFloatType process(DspFloatType x);
		DspFloatType saturate(DspFloatType input);

		DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
			return A*process(I);
		}

		enum
        {
            PORT_CUTOFF,
            PORT_RESONANCE,			
			PORT_SAUTRATION,
        };
        void setPort(int port, DspFloatType v)
        {
            switch (port)
            {
            case PORT_CUTOFF:
                setCutoff(v);
                break;
            case PORT_RESONANCE:
                setResonance(v);
                break;		
			case PORT_SATURATION:
				setSaturation(v);
				break;
            }
        }
		int passMode = LOWPASS;
		DspFloatType cutoff;
		DspFloatType resonance;
		DspFloatType saturationAmount;
		
		DspFloatType sampleRate;
		DspFloatType out1, out2, out3, out4;
		DspFloatType in1, in2, in3, in4;

		// coefficients determined by cutoff and resonance
		DspFloatType r, p, k;

		const DspFloatType saturationLimit = 0.95;
	};

	
	void MoogFilterI::init(DspFloatType sampleRate)
	{
		this->sampleRate = sampleRate;

		out1 = out2 = out3 = out4 = 0.0f;
		in1 = in2 = in3 = in4 = 0.0f;
		calculateCoefficients();
	};

	void MoogFilterI::setCutoff(DspFloatType cutoff)
	{
		this->cutoff = cutoff;
		calculateCoefficients();
	}

	void MoogFilterI::setResonance(DspFloatType resonance)
	{
		if (resonance >= 0.0 && resonance <= 1.0)
		{
			this->resonance = resonance;
			calculateCoefficients();
		}
	}

	void MoogFilterI::setSaturation(DspFloatType saturationAmount)
	{
		if (saturationAmount >= 0.0 && saturationAmount <= 1.0)
		{
			this->saturationAmount = saturationAmount;
		}
	}

	void MoogFilterI::ProcessBlock(size_t n, DspFloatType * inputs, DspFloatType * outputs)
	{
		for (int s = 0; s < n; s++)
		{
			outputs[s] =  process(inputs[s]);
		}
	}

	void MoogFilterI::calculateCoefficients()
	{
		DspFloatType f = (cutoff + cutoff) / this->sampleRate;
		p = f * (1.8f - 0.8f * f);
		k = p + p - 1.f;

		DspFloatType t = (1.f - p) * 1.386249f;
		DspFloatType t2 = 12.f + t * t;
		r = resonance * (t2 + 6.f * t) / (t2 - 6.f * t);
	};

	DspFloatType MoogFilterI::process(DspFloatType input)
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

	
	DspFloatType MoogFilterI::saturate(DspFloatType input)
	{
		DspFloatType drySignal = input;
		input *= 1.5f;

		DspFloatType x1 = fabsf(input + saturationLimit);
		DspFloatType x2 = fabsf(input - saturationLimit);
		DspFloatType wetSignal = (0.5f * (x1 - x2));
		
		return (wetSignal * saturationAmount) + (drySignal * (1.0f - saturationAmount));
	}
}
#undef LOWPASS
#undef HIGHPASS
#undef BANDPASS