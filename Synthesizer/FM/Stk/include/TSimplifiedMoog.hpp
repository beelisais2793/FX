/* -------------------------------------------------------------------------
 * This source code is provided without any warranties as published in 
 * DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
 * http://www.dafx.de. It may be used for educational purposes and not 
 * for commercial applications without further permission.
 * -------------------------------------------------------------------------
 */

#pragma once

#include "TLadderBase.hpp"

namespace SoundAlchemy::MoogFilters
{
template<typename T>
class TSimplifiedMoog : public TLadderFilterBase<T>
{
public:
	
	TSimplifiedMoog(T sampleRate) : TLadderFilterBase<T>(sampleRate)
	{
		// To keep the overall level approximately constant, comp should be set
		// to 0.5 resulting in a 6 dB passband gain decrease at the maximum this->resonance
		// (compared to a 12 dB decrease in the original Moog model
		gainCompensation = 0.5;
		
		memset(stage, 0, sizeof(stage));
		memset(stageZ1, 0, sizeof(stageZ1));
		memset(stageTanh, 0, sizeof(stageTanh));
		
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}
	
	virtual ~TSimplifiedMoog()
	{
		
	}
	
	// This system is nonlinear so we are probably going to create a signal with components that exceed nyquist.
	// To prevent aliasing distortion, we need to oversample this processing chunk. Where do these extra samples
	// come from? Todo! We can use polynomial interpolation to generate the extra samples, but this is expensive.
	// The cheap solution is to zero-stuff the incoming sample buffer.
	// With resampling, numSamples should be 2x the frame size of the existing sample rate.
	// The output of this filter needs to be run through a decimator to return to the original samplerate.
	void Process(size_t n, T * samples, T * _output)
	{
		// Processing still happens at sample rate...
		for (int s = 0; s < n; ++s)
		{
			for (int stageIdx = 0; stageIdx < 4; ++stageIdx)
			{
				if (stageIdx)
				{
					input = stage[stageIdx-1];
					stageTanh[stageIdx-1] = std::tanh(input);
					stage[stageIdx] = (h * stageZ1[stageIdx] + h0 * stageTanh[stageIdx-1]) + (1.0 - g) * (stageIdx != 3 ? stageTanh[stageIdx] : std::tanh(stageZ1[stageIdx]));
				}
				else
				{
					input = samples[s] - ((4.0 * this->resonance) * (output - gainCompensation * samples[s]));
					stage[stageIdx] = (h * std::tanh(input) + h0 * stageZ1[stageIdx]) + (1.0 - g) * stageTanh[stageIdx];
				}
				
				stageZ1[stageIdx] = stage[stageIdx];
			}
			
			output = stage[3];
			SNAP_TO_ZERO(output);
			_output[s] = output;
		}
	}

	void Process(T * samples, uint32_t n)
	{
		// Processing still happens at sample rate...
		for (int s = 0; s < n; ++s)
		{
			for (int stageIdx = 0; stageIdx < 4; ++stageIdx)
			{
				if (stageIdx)
				{
					input = stage[stageIdx-1];
					stageTanh[stageIdx-1] = std::tanh(input);
					stage[stageIdx] = (h * stageZ1[stageIdx] + h0 * stageTanh[stageIdx-1]) + (1.0 - g) * (stageIdx != 3 ? stageTanh[stageIdx] : std::tanh(stageZ1[stageIdx]));
				}
				else
				{
					input = samples[s] - ((4.0 * this->resonance) * (output - gainCompensation * samples[s]));
					stage[stageIdx] = (h * std::tanh(input) + h0 * stageZ1[stageIdx]) + (1.0 - g) * stageTanh[stageIdx];
				}
				
				stageZ1[stageIdx] = stage[stageIdx];
			}
			
			output = stage[3];
			SNAP_TO_ZERO(output);
			samples[s] = output;
		}
	}

	T Tick(T input) {
		T r;
		Process(1,&input,&r);
		return r;
	}
	virtual void SetResonance(T r) override
	{
		this->resonance = r;
	}
	
	virtual void SetCutoff(T c) override
	{
		this->cutoff = c;
		
		// Not being oversampled at the moment... * 2 when functional
		T fs2 = sampleRate;
		
		// Normalized this->cutoff [0, 1] in radians: ((2*pi) * this->cutoff / samplerate)
		g = (2 * MOOG_PI) * this->cutoff / fs2; // feedback coefficient at fs*2 because of doublesampling
		g *= MOOG_PI / 1.3; // correction factor that allows _cutoff to be supplied Hertz
		
		// FIR part with gain g
		h = g / 1.3;
		h0 = g * 0.3 / 1.3;
	}
	
private:
	
	double output;
	double lastStage;
	
	double stage[4];
	double stageZ1[4];
	double stageTanh[3];
	
	double input;
	double h;
	double h0;
	double g;
	
	T gainCompensation;
};
}