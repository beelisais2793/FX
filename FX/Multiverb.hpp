#pragma once

#include <cstdlib>
#include <cmath>

namespace Reverb::Multiverb
{
	class DelayLine {
	public:
		DelayLine();
		~DelayLine();
		void create(int);
		void push(DspFloatType);
		DspFloatType get(void);
		void clear(void);
	private:
		DspFloatType *line;
		int ptr;
		int del;
	};

	DelayLine::DelayLine() {
		ptr = 0;
		line = NULL;
	}

	DelayLine::~DelayLine() {
		delete [] line;
	}

	void DelayLine::create(int n) {
		line = new DspFloatType[n];
		for (int i = 0; i < n; i++)
			line[i] = 0.0;
		del = n;
	}

	void DelayLine::push(DspFloatType x) {
		line[ptr++] = x;
		if (ptr >= del)
			ptr = 0;
	}

	DspFloatType DelayLine::get(void) {
		return(line[ptr]);
	}

	void DelayLine::clear(void) {
		for (int i = 0; i < del; i++)
			line[i] = 0.0;
		ptr = 0;
	}

	#define ORDER 8 // number of waveguide pairs

	class Multiverb
	{
	public:
	Multiverb();
	~Multiverb();

	//Parameters
	void SetSampleRate(DspFloatType sRate);
	void SetLFReverbTime(DspFloatType t60lf);
	DspFloatType GetLFReverbTime(void){return m_t60lf;};
	void SetHFReverbTime(DspFloatType t60hf);
	DspFloatType GetHFReverbTime(void){return m_t60hf;};
	void SetDryGain(DspFloatType dryGain);
	DspFloatType GetDryGain(void){return m_dryGain;};
	void SetWetGain(DspFloatType wetGain);
	DspFloatType GetWetGain(void){return m_wetGain;};
	void SetStereoSep(DspFloatType stereoSep);
	DspFloatType GetStereoSep(void){return m_stereoSep;};

	//Use
	void Update(void);
	void Reset(void);
	void ClockProcess(DspFloatType* LeftSample, DspFloatType* RightSample); 

	private:
	DspFloatType sampRate; // obtained from host
	// user set parameters
	DspFloatType m_t60lf;
	DspFloatType m_t60hf;
	DspFloatType m_dryGain;
	DspFloatType m_wetGain;
	DspFloatType m_stereoSep;
	// derived parameters
	DspFloatType dGain;
	DspFloatType wGain;
	DspFloatType right;
	DspFloatType left;
	DspFloatType atten[ORDER];
	DspFloatType damp[ORDER];
	// delay line pairs
	DelayLine east[ORDER];
	DelayLine west[ORDER];
	// state vectors
	DspFloatType xPlus[ORDER];
	DspFloatType yPlus[ORDER];
	};


	static int delay[] = {// pre-determined sample delays
		439,
		997,
		1601,
		2251,
		2897,
		3583,
		4271,
		4999
	};

	Multiverb::Multiverb() {
		sampRate = 44100.0; // just in case!
		for (int i = 0; i < ORDER; i++) {// initialize delay lines and state vectors
			east[i].create(delay[i]);
			west[i].create(delay[i]);
			xPlus[i] = 0.0;
			yPlus[i] = 0.0;
		}
	}
	Multiverb::~Multiverb() {
	}

	void Multiverb::SetSampleRate(DspFloatType sRate) {
		sampRate = sRate;
	}

	void Multiverb::SetLFReverbTime(DspFloatType t60lf) {
		m_t60lf = t60lf;
	}

	void Multiverb::SetHFReverbTime(DspFloatType t60hf) {
		m_t60hf = t60hf;
	}

	void Multiverb::SetDryGain(DspFloatType dryGain) {
		m_dryGain = dryGain;
	}

	void Multiverb::SetWetGain(DspFloatType wetGain) {
		m_wetGain = wetGain;
	}

	void Multiverb::SetStereoSep(DspFloatType stereoSep) {
		m_stereoSep = stereoSep;
	}

	void Multiverb::Update()
	{// update derived parameters
		DspFloatType alf, ahf;

		dGain = DspFloatType(pow(10.0, m_dryGain / 20.0));
		wGain = DspFloatType(pow(10.0, m_wetGain / 20.0));
		right = wGain * (1.0 - m_stereoSep / 100.0) / 2.0;
		left = wGain * (1.0 + m_stereoSep / 100.0) / 2.0;
		for (int i = 0; i < ORDER; i++) { // update attenuation and damping
			alf = DspFloatType(pow(10.0, -3.0 * DspFloatType(delay[i]) / (sampRate * m_t60lf)));
			ahf = DspFloatType(pow(10.0, -3.0 * DspFloatType(delay[i]) / (sampRate * m_t60hf)));
			damp[i] = (alf - ahf) / (alf + ahf);
			atten[i] = alf * (1.0 - damp[i]);
		}
	}

	void Multiverb::Reset(void)
	{
		for (int i = 0; i < ORDER; i++) {// reset buffers
			east[i].clear();
			west[i].clear();
			xPlus[i] = 0.0;
			yPlus[i] = 0.0;
		}
	}

	void Multiverb::ClockProcess(DspFloatType* LeftSample, DspFloatType* RightSample)
	{// process one sampling period
		DspFloatType xPlusTotal = 0.0;
		DspFloatType yPlusTotal = 0.0;
		for (int i = 0; i < ORDER; i++) {
			xPlus[i] = damp[i] * xPlus[i] + atten[i] * west[i].get();
			yPlus[i] = damp[i] * yPlus[i] + atten[i] * east[i].get();
			xPlusTotal += xPlus[i];
			yPlusTotal += yPlus[i];
		}
		xPlusTotal /= DspFloatType(ORDER);
		yPlusTotal /= DspFloatType(ORDER);
		for (int i = 0; i < ORDER; i++) {
			east[i].push(xPlusTotal - xPlus[i] + *LeftSample);
			west[i].push(yPlusTotal - yPlus[i] + *RightSample);
		}
		*LeftSample = right * xPlusTotal + left * yPlusTotal + dGain * *LeftSample;
		*RightSample = left * xPlusTotal + right * yPlusTotal + dGain * *RightSample;
	}
}