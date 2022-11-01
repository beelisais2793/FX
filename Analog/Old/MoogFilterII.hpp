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
	class MoogFilterII
	{
	public:
		void init(float sampleRate);
		void set(float cutoff, float resonance);
		float processSample(float in, int passMode);
		void  ProcessBlock(size_t n, float * inputs, float * outputs, int pass = LOWPASS)

	private:
		float sampleRate;
		float cutoff, resonance;
		float out1, out2, out3, out4;
		float in1, in2, in3, in4;
	};


	void MoogFilterII::init(float sampleRate)
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
	void MoogFilterII::set(float cutoff, float resonance)
	{
		cutoff = cutoff / (sampleRate/2.0f);
		resonance = resonance * 4.0f;
		this->cutoff = cutoff;
		this->resonance = resonance;
	}


	// Filter (in [-1.0...+1.0])
	float MoogFilterII::processSample(float input, int passMode)
	{
		float in = input;
		float f = cutoff * 1.16;
		float fb = resonance * (1.0 - 0.15 * f * f);
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

	void MoogFilterII::ProcessBlock(size_t n, float * inputs, float * outputs, int passMode)
	{
		for (int s = 0; s < numSamples; s++)
		{
			output[s] =  processSample(inputs[s], passMode));
		}
	}
]
#undef LOWPASS
#undef HIGHPASS
#undef BANDPASS