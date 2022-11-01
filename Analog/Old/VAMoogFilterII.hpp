/*
This class is an adaption of the Moog VCF variation 2, posted here:
https://www.musicdsp.org/en/latest/Filters/26-moog-vcf-variation-2.html

Comes from a Stilson/Smith CCRMA paper

added highpass and bandpass options
*/

#pragma once

#define LOWPASS 0
#define HIGHPASS 1
#define BANDPASS 2


namespace Analog::Filters::Moog::MoogFilterII
{
	struct MoogFilterII : public FilterProcessor
	{	
		MoogFilterII() : FilterProcessor() {
			init(sampleRate);
			setCutoff(1000.0);
			setResonance(0.01);
		}

		void init(double sampleRate);
		void set(double cutoff, double resonance);
		double processSample(double in);
		void  ProcessBlock(size_t n, float * inputs, float * outputs);

		double Tick(double I, double A=1, double X=1, double Y=1) {
			return A*processSample(I);
		}

		int passMode = LOWPASS;
		double sampleRate;
		double cutoff, resonance;
		double out1, out2, out3, out4;
		double in1, in2, in3, in4;
	};


	void MoogFilterII::init(double sampleRate)
	{
		this->sampleRate = sampleRate;
		out1 = 0.0f;
		out2 = 0.0f;
		out3 = 0.0f;
		out4 = 0.0f;
		in1 = 0.0f;
		in2 = 0.0f;
		in3 = 0.0f;
		in4 = 0.0f;
	}

	// Set coefficients given frequency & resonance [0.0...1.0]
	void MoogFilterII::set(double cutoff, double resonance)
	{
		cutoff = cutoff / (sampleRate/2.0f);
		resonance = resonance * 4.0f;
		this->cutoff = cutoff;
		this->resonance = resonance;
	}


	// Filter (in [-1.0...+1.0])
	double MoogFilterII::processSample(double input)
	{
		double in = input;
		double f = cutoff * 1.16;
		double fb = resonance * (1.0 - 0.15 * f * f);
		in -= out4 * fb;
		in *= 0.35013 * (f * f) * (f * f);
		out1 = in + 0.3 * in1 + (1 - f) * out1; // Pole 1
		in1 = in;
		out2 = out1 + 0.3 * in2 + (1 - f) * out2;  // Pole 2
		in2 = out1;
		out3 = out2 + 0.3 * in3 + (1 - f) * out3;  // Pole 3
		in3 = out2;
		out4 = out3 + 0.3 * in4 + (1 - f) * out4;  // Pole 4
		in4 = out3;

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

		// Lowpass = out4
		// Highpass = input - out4 - out1
		// Bandpass = out4 - out1
	}

	void MoogFilterII::ProcessBlock(size_t numSamples, float * inputs, float * outputs)
	{
		for (int s = 0; s < numSamples; s++)
		{
			outputs[s] =  processSample(inputs[s]);
		}
	}
}
#undef LOWPASS
#undef HIGHPASS
#undef BANDPASS