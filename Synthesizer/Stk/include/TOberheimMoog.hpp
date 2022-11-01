// See: http://www.willpirkle.com/forum/licensing-and-book-code/licensing-and-using-book-code/
// The license is "You may also use the code from the FX and Synth books without licensing or fees. 
// The code is for you to develop your own plugins for your own use or for commercial use."

#pragma once


#include "TLadderBase.hpp"
#include "Util.h"

namespace SoundAlchemy::MoogFilters
{
template<typename T>
class TVAOnePole
{
public:

	TVAOnePole(T sr) : sampleRate(sr)
	{
		Reset();
	}
	
	void Reset()
	{
		alpha = 1.0;
		beta = 0.0;
		gamma = 1.0;
		delta = 0.0;
		epsilon = 0.0;
		a0 = 1.0;
		feedback = 0.0;
		z1 = 0.0;
	}
	
	double Tick(double s)
	{
		s = s * gamma + feedback + epsilon * GetFeedbackOutput();
		double vn = (a0 * s - z1) * alpha;
		double out = vn + z1;
		z1 = vn + out;
		return out;
	}
	
	void SetFeedback(double fb) { feedback = fb; }
	double GetFeedbackOutput(){ return beta * (z1 + feedback * delta); }
	void SetAlpha(double a) { alpha = a; };
	void SetBeta(double b) { beta = b; };
	
private:

	T sampleRate;
	double alpha;
	double beta;
	double gamma;
	double delta;
	double epsilon;
	double a0;
	double feedback;
	double z1;
};

template<typename T>
class TOberheimVariationMoog : public TLadderFilterBase<T>
{
	
public:
	
	TOberheimVariationMoog(T sampleRate) : TLadderFilterBase<T>(sampleRate)
	{
		LPF1 = new VAOnePole(sampleRate);
		LPF2 = new VAOnePole(sampleRate);
		LPF3 = new VAOnePole(sampleRate);
		LPF4 = new VAOnePole(sampleRate);
		
		saturation = 1.0;
		Q = 3.0;
		
		SetCutoff(1000.f);
		SetResonance(0.1f);
	}
	
	virtual ~TOberheimVariationMoog()
	{
		delete LPF1;
		delete LPF2;
		delete LPF3;
		delete LPF4;
	}
	
	void Process(size_t n, T * samples, T * output)
	{
		for (int s = 0; s < n; ++s)
		{
			T input = samples[s];
			
			double sigma =
				LPF1->GetFeedbackOutput() +
				LPF2->GetFeedbackOutput() +
				LPF3->GetFeedbackOutput() +
				LPF4->GetFeedbackOutput();
			
			input *= 1.0 + K;
			
			// calculate input to first filter
			double u = (input - K * sigma) * alpha0;
			
			u = std::tanh(saturation * u);
			
			double stage1 = LPF1->Tick(u);
			double stage2 = LPF2->Tick(stage1);
			double stage3 = LPF3->Tick(stage2);
			double stage4 = LPF4->Tick(stage3);
			
			// Oberheim variations
			output[s] =
				oberheimCoefs[0] * u +
				oberheimCoefs[1] * stage1 +
				oberheimCoefs[2] * stage2 +
				oberheimCoefs[3] * stage3 +
				oberheimCoefs[4] * stage4;
		}
	}
	void Process(T * samples, uint32_t n)
	{
		for (int s = 0; s < n; ++s)
		{
			T input = samples[s];
			
			double sigma =
				LPF1->GetFeedbackOutput() +
				LPF2->GetFeedbackOutput() +
				LPF3->GetFeedbackOutput() +
				LPF4->GetFeedbackOutput();
			
			input *= 1.0 + K;
			
			// calculate input to first filter
			double u = (input - K * sigma) * alpha0;
			
			u = std::tanh(saturation * u);
			
			double stage1 = LPF1->Tick(u);
			double stage2 = LPF2->Tick(stage1);
			double stage3 = LPF3->Tick(stage2);
			double stage4 = LPF4->Tick(stage3);
			
			// Oberheim variations
			samples[s] =
				oberheimCoefs[0] * u +
				oberheimCoefs[1] * stage1 +
				oberheimCoefs[2] * stage2 +
				oberheimCoefs[3] * stage3 +
				oberheimCoefs[4] * stage4;
		}
	}
	T Tick(T input) {
		T r;
		Process(1,&input,&r);
		return r;
	}
	virtual void SetResonance(T r) override
        {
             // this maps resonance = 1->10 to K = 0 -> 4
             K = (4.0) * (r - 1.0)/(10.0 - 1.0);
        }

	virtual void SetCutoff(T c) override
	{
		this->cutoff = c;
		
		// prewarp for BZT
		double wd = 2.0 * MOOG_PI * this->cutoff;
		double Fs = 1.0 / sampleRate;
		double wa = (2.0 / Fs) * std::tan(wd * Fs / 2.0);
		double g = wa * Fs / 2.0;
		
		// Feedforward coeff
		double G = g / (1.0 + g);
		
		LPF1->SetAlpha(G);
		LPF2->SetAlpha(G);
		LPF3->SetAlpha(G);
		LPF4->SetAlpha(G);

		LPF1->SetBeta(G*G*G / (1.0 + g));
		LPF2->SetBeta(G*G / (1.0 + g));
		LPF3->SetBeta(G / (1.0 + g));
		LPF4->SetBeta(1.0 / (1.0 + g));
		
		gamma = G*G*G*G;
		alpha0 = 1.0 / (1.0 + K * gamma);
		
		// Oberheim variations / LPF4
		oberheimCoefs[0] = 0.0;
		oberheimCoefs[1] = 0.0;
		oberheimCoefs[2] = 0.0;
		oberheimCoefs[3] = 0.0;
		oberheimCoefs[4] = 1.0;
	}
	
private:
	
	VAOnePole * LPF1;
	VAOnePole * LPF2;
	VAOnePole * LPF3;
	VAOnePole * LPF4;
	
	double K;
	double gamma;
	double alpha0;
	double Q;
	double saturation;
	
	double oberheimCoefs[5];
};
}