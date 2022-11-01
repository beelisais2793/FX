// https://github.com/Oliver614/Eko
#pragma once

#include <vector>

class DelayLine
{
public:
	DelayLine() 
	{
		mCircularBufferSizeInSamples = 0;
		mCircularBufferPosition = 0;
		mSampleRate = 0.f;
	}
	~DelayLine() {}

	inline void init(const float& sampleRate, const float& bufferSizeInSeconds);
	inline void init(const uint32_t& bufferSizeInSamples, const float& sampleRate);
	inline float read(const float& readTime);
	inline float interpolate(const float& x, const float& x1, const float& fraction);
	inline void write(const float& sample);
	inline void nextPos();
	inline uint32_t getSize();

private:

	std::vector<float> mCircularBuffer;
	int32_t mCircularBufferSizeInSamples;
	int32_t mCircularBufferPosition;
	float mSampleRate;
};

inline void DelayLine::init(const float& sampleRate, const float& bufferSizeInSeconds)
{
	mSampleRate = sampleRate;
	mCircularBufferSizeInSamples = sampleRate * bufferSizeInSeconds;
	mCircularBuffer = std::vector<float>(mCircularBufferSizeInSamples);
	std::fill(mCircularBuffer.begin(), mCircularBuffer.end(), 0.f);
}

inline void DelayLine::init(const uint32_t& bufferSizeInSamples, const float& sampleRate)
{
	mSampleRate = sampleRate;
	mCircularBufferSizeInSamples = bufferSizeInSamples;
	mCircularBuffer = std::vector<float>(bufferSizeInSamples);
	std::fill(mCircularBuffer.begin(), mCircularBuffer.end(), 0.f);
}

inline void DelayLine::write(const float& sample)
{
	mCircularBuffer[mCircularBufferPosition] = sample;
	nextPos();
}

inline float DelayLine::read(const float& readTime)  // No Interpolation
{
	float readTimeInSamples = readTime * mSampleRate;
	int delayReadHead = mCircularBufferPosition - readTimeInSamples;
	if (delayReadHead < 0) delayReadHead += mCircularBufferSizeInSamples;

	return mCircularBuffer[delayReadHead];
}

inline float DelayLine::interpolate(const float& x, const float& x1, const float& fraction)
{
	return (1.f - fraction) * x + fraction * x1;
}

inline void DelayLine::nextPos()
{
	mCircularBufferPosition++;
	mCircularBufferPosition %= mCircularBufferSizeInSamples;
}

inline uint32_t DelayLine::getSize()
{
	return mCircularBufferSizeInSamples;
}

class EkoFilter
{
public:

	EkoFilter() {}
	~EkoFilter() {}

	void init(float sampleRate, float samplesPerBlock);
	void reset();
	void setTypeLP();
	void setTypeHP();
	void setTypeBP();
	void setTypeVariable(float cutoffValue);
	void setCutoff(float cutoff);
	void setResonance(float resonance);
	float process(float sample);

private:

	static constexpr int mNumChannels = 1;
	juce::dsp::StateVariableTPTFilter<float> filter;
	juce::dsp::ProcessSpec spec;

};

void EkoFilter::init(float sampleRate, float samplesPerBlock)
{
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = mNumChannels;

	filter.prepare(spec);
}

void EkoFilter::reset()
{
	filter.reset();
}

void EkoFilter::setTypeLP()
{
	filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
}

void EkoFilter::setTypeHP()
{
	filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
}

void EkoFilter::setTypeBP()
{
	filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
}

void EkoFilter::setTypeVariable(float cutoffValue)
{

	cutoffValue <= 0.f ? filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass) : filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);


}

void EkoFilter::setCutoff(float cutoff)
{
	filter.setCutoffFrequency(cutoff);
}

void EkoFilter::setResonance(float resonance)
{
	float res = resonance / 1.4142135623731;
	filter.setResonance(resonance);
}

float EkoFilter::process(float sample)
{
	return filter.processSample(0, sample);
}

class EkoPreDelay
{
public:

	EkoPreDelay();
	~EkoPreDelay();

	void init(const float& sampleRate, const float& maxDelayTime);
	void process(const float& sampleLeft, const float& sampleRight, const float& delayTime, const float& feedback, float & returnLeft, float& returnRight);

private:

	float mFeedbackLeft, mFeedbackRight, mSampleRate;
	DelayLine mDelayLineRight, mDelayLineLeft;

};

EkoPreDelay::EkoPreDelay()
{
	mFeedbackLeft = 0.f;
	mFeedbackRight = 0.f;
	mSampleRate = 0.f;
}

EkoPreDelay::~EkoPreDelay()
{

}

void EkoPreDelay::init(const float& sampleRate, const float& maxDelayTime)
{
	uint32_t bufferSizeInSamples = sampleRate * maxDelayTime;
	mDelayLineLeft.init(bufferSizeInSamples, sampleRate);
	mDelayLineRight.init(bufferSizeInSamples, sampleRate);
	mSampleRate = sampleRate;
}

void EkoPreDelay::process(const float& sampleLeft, const float& sampleRight, const float& delayTime, const float& feedback, float& returnLeft, float& returnRight)
{
	float feedbackAmount = delayTime > 0.f ? feedback * 0.95f : 0.f;
	float lessOneSample = 1.f / (mSampleRate);
	mDelayLineLeft.write(sampleLeft + mFeedbackLeft);
	mDelayLineRight.write(sampleRight + mFeedbackRight);

	float left = mDelayLineLeft.read(delayTime + lessOneSample);
	float right = mDelayLineRight.read(delayTime + lessOneSample);

	mFeedbackLeft = left * feedbackAmount;
	mFeedbackRight = right * feedbackAmount;

	returnLeft = left;
	returnRight = right;
}

class EkoTime
{
public:

	EkoTime() {}
	~EkoTime() {}

private:

	float exp_log(const float& in, const float& shp);
	float crossFade(const float& in1, const float& in2, const float& x);
	float shaperStage(const float& in, const float& xB, const float& xA, const float& shp, const float& levA, const float& levB, float& greaterThan);
	float shaper(const float& time, const float& shape, const float& spread);

public:

	float process(const float& spr, const float& center, const float& time);

};

/*
* Finds the exponent of log, similar to f (x) = x ^ 1 / n but
* symmetrical to the axis f (x) = 1-x
*/
float EkoTime::exp_log(const float& in, const float& shp)
{

	float strongness = shp > 0.f ? (1 / (1 - shp)) + -1 : shp;
	float negStrongness = strongness * -1;

	float xIn = in > 1e-9f ? in : 0.f;

	float a = (1 / strongness) + 1;

	float b = xIn + a;
	float c = b * negStrongness;
	float d = c + strongness;
	float e = a / d;
	float f = e + a;
	float h = in > 1e-9f ? f : 0.f;

	return h;
}

/*
* Crossfades the input x between the two input levels
*/
float EkoTime::crossFade(const float& in1, const float& in2, const float& x)
{
	float a = (in2 - in1);
	float b = a * x;
	float c = in1 + b;
	return c;
}

/*
* Shapes the time as long as the input (in) is greater than xB
* else it passes to the next shaper
*/
float EkoTime::shaperStage(const float& in, const float& xB, const float& xA, const float& shp, const float& levA, const float& levB, float& greaterThan)
{
	if (in >= xB)
	{
		greaterThan = in;
		return 0.f;
	}
	greaterThan = 0.f;

	float a = in >= xB ? 0.f : in;
	float b = a - xA;
	float c = 1.f / (xB - xA);
	float d = b * c;
	float e = exp_log(d, shp);
	return crossFade(levA, levB, e);
}

/*
* if the first shaper fails or passes the input to the greater than output it then moves onto the second stage of shaping.
*/
float EkoTime::shaper(const float& time, const float& X, const float& spread)
{
	float greaterThanStageA = 0.f, greaterThatStageB = 0.f;
	float a = shaperStage(time, X, 0.f, spread, 0.f, X, greaterThanStageA);
	if (greaterThanStageA == 0.f) return a;
	float b = shaperStage(greaterThanStageA, 1.f, X, spread * -1, X, 1.f, greaterThatStageB);
	return b;
}

/*
* Translates spr to a value between 0.3 and 0.999 then executes the shaper function.
*/
float EkoTime::process(const float& shape /*Spread*/, const float& spr /*Center*/, const float& time)
{
	float sprSquared = (spr * spr);
	float spread = ((0.3f - 0.999f) * sprSquared) + 0.999f;

	return shaper(time, shape, spread);
}


struct Diffuser
{
	Diffuser()
	{
		mMaxDiffusionTime = 0.f;
		mSampleRate = 0.f;
		mToWrite = 0.f;
		mZ = 0.f;
		mOut = 0.f;

	}

	~Diffuser() {}
	void init(const float& sampleRate, const float& maxDiffusuionTime, float samplesPerBlock);

	void diffuse(const float& input, const float& feedback, float& toWrite, const float& z, float& out);
	float twoPassDiffuse(const float& input, const float& time, const float& feedback, float& outputArray);

	DelayLine mDelayLine;

	float mMaxDiffusionTime;
	float mSampleRate;
	float mToWrite, mZ, mOut;

};

//==============================================================================================================================

struct MultiTapDiffusion
{
	MultiTapDiffusion() {}
	~MultiTapDiffusion() {}

	void init(const float& sampleRate, const float& samplesPerBlock);
	void processMultiTapDiffusion(const float& in, float& feedback, float* t, float* df, float* out);

	static constexpr uint32_t mNumberOfDiffusionLines = 8;
	const float mMaxDiffuserTime = 2.f;
	float mSampleRate = 0.f;

	Diffuser mDiffuserArray[mNumberOfDiffusionLines];
};

//==============================================================================================================================

class Eko
{
public:
	Eko();
	~Eko() {}

	void init(const float& sampleRate, const float& samplesPerBlock);

public:

	void spreadParams(const float& spread);
	void diffusionCalcs(const float& diff);
	void phase();
	float sclClip(const float& ctl, const float& min, const float& max);
	void scl(const float& tx, const float& tPlus, float& tx2, float& lx);
	float lfoStage(const float& i, const float& phase, const float& n, const float& t, const float& tx, const float& lx);
	void lfo(const float& txIn, const float& lfoAmt, float* t1, float* t2);

	void eq(const float& hpCutoff, const float& lpCutoff, float& outL, float& outR);
	void fb(const float& LIn, const float& RIn, const float& fb, float& returnLeft, float& feedbackLeft, float& returnRight, float& feedbackRight);
	void read(float& returnLeft, float& returnRight, const float& diffusion);

	void colour(const float& sampleLeft, const float& sampleRight, const float& cutoff, const float& emphasis, float& returnLeft, float& returnRight);
	void mix(const float& l0, const float& r0, const float& l1, const float& r1, const float& fb, const float& mix, float& returnLeft, float& returnRight);

public:

	void process(const float& lIn, const float& rIn, float* lArray, float* rArray, const float& diffusion, const float& size, const float& LPAmmount, const float& HPAmmount, 
		const float& fbck, const float& mix, const float& preDelayTime, const float& preDelayFeedback, const float& colourCutoff, const float& colourEmphasis, float& returnLeft, float& returnRight);

private:

	EkoPreDelay mPreDelay;
	MultiTapDiffusion mMultiTapDelayL, mMultiTapDelayR;
	EkoFilter mEqFilterHighpassLeft, mEqFilterHighpassRight, mEqFilterLowpassLeft, mEqFilterLowpassRight;
	EkoFilter mColourFilterLeft, mColourFilterRight;

	static constexpr uint32_t mMaxDiffusionSteps = 8;
	float mPhase;

	float mPassThroughLeft, mPassThroughRight;
	float mSampleLeft, mSampleRight;

	bool mSwitchLfoDirectionUp;

	float mHpCutoff, mLpCutoff, mLfoAmount, mTimeVariation, mDiffusionAmount, mSampleRate, mDiffusionFeedbackLeft, mDiffusionFeedbackRight;
	float mTimingsLeft[mMaxDiffusionSteps], mTimingsRight[mMaxDiffusionSteps], df[mMaxDiffusionSteps];
	float mLtoScanX8[mMaxDiffusionSteps], mRtoScanX8[mMaxDiffusionSteps];
	float mDiffusionArray[mMaxDiffusionSteps];
};


void Diffuser::init(const float& sampleRate, const float& maxDiffusuionTime, float samplesPerBlock)
{
	mMaxDiffusionTime = maxDiffusuionTime;
	mSampleRate = sampleRate;
	mDelayLine.init(sampleRate, maxDiffusuionTime);

}

void Diffuser::diffuse(const float& input, const float& feedback, float& toWrite, const float& z, float& out)
{
	float feedbackInverse = feedback * -1.f;
	float toOut = ((input * feedbackInverse) + z);
	toWrite = ((toOut * feedback) + input);
	out = toOut;
}

float Diffuser::twoPassDiffuse(const float& input, const float& time, const float& feedback, float& outputArray)
{

	diffuse(input, feedback, mToWrite, mZ, mOut);
	mDelayLine.write(mToWrite);
	mZ = mDelayLine.read(time);
	outputArray = mOut;
	return mOut;
}

//==============================================================================================================================

inline void MultiTapDiffusion::init(const float& sampleRate, const float& samplesPerBlock)
{
	mSampleRate = sampleRate;
	for (auto& diffuser : mDiffuserArray)
	{
		diffuser.init(sampleRate, mMaxDiffuserTime, samplesPerBlock);
	}
}

void MultiTapDiffusion::processMultiTapDiffusion(const float& in, float& feedback, float* t, float* df, float* out)
{
	float a = in;
	float b, c, d, e, f, g, h, i;

	b = mDiffuserArray[0].twoPassDiffuse(a, t[0] / mSampleRate, df[0], out[0]);
	c = mDiffuserArray[1].twoPassDiffuse(b, t[1] / mSampleRate, df[1], out[1]);
	d = mDiffuserArray[2].twoPassDiffuse(c, t[2] / mSampleRate, df[2], out[2]);
	e = mDiffuserArray[3].twoPassDiffuse(d, t[3] / mSampleRate, df[3], out[3]);
	f = mDiffuserArray[4].twoPassDiffuse(e, t[4] / mSampleRate, df[4], out[4]);
	g = mDiffuserArray[5].twoPassDiffuse(f, t[5] / mSampleRate, df[5], out[5]);
	h = mDiffuserArray[6].twoPassDiffuse(g, t[6] / mSampleRate, df[6], out[6]);
	i = mDiffuserArray[7].twoPassDiffuse(h, t[7] / mSampleRate, df[7], out[7]);
	feedback = i;
}

//==============================================================================================================================

Eko::Eko()
{
	mHpCutoff = 0.f; mLpCutoff = 0.f; mLfoAmount = 0.f; mTimeVariation = 0.f;
	mDiffusionAmount = 0.f; mSampleRate = 0.f; mSampleLeft = 0.f; mSampleRight = 0.f;
	mPhase = 0.f; mPassThroughLeft = 0.f; mPassThroughRight = 0.f; mDiffusionFeedbackLeft = 0.f; mDiffusionFeedbackRight = 0.f;
	mSwitchLfoDirectionUp = true;

	for (int i = 0; i < mMaxDiffusionSteps; i++)
	{
		mTimingsLeft[i] = 0.f;
		mTimingsRight[i] = 0.f;
		df[i] = 0.f;
		mLtoScanX8[i] = 0.f;
		mRtoScanX8[i] = 0.f;
		mDiffusionArray[i] = 0;
	}
}

void Eko::init(const float& sampleRate, const float& samplesPerBlock)
{
	mSampleRate = sampleRate;
	mMultiTapDelayL.init(sampleRate, samplesPerBlock);
	mMultiTapDelayR.init(sampleRate, samplesPerBlock);

	mEqFilterHighpassLeft.init(sampleRate, samplesPerBlock);
	mEqFilterHighpassRight.init(sampleRate, samplesPerBlock);
	mEqFilterLowpassLeft.init(sampleRate, samplesPerBlock);
	mEqFilterLowpassRight.init(sampleRate, samplesPerBlock);

	mEqFilterHighpassLeft.setTypeHP();
	mEqFilterHighpassRight.setTypeHP();
	mEqFilterLowpassLeft.setTypeLP();
	mEqFilterLowpassRight.setTypeLP();

	mColourFilterLeft.setTypeVariable(0.f);
	mColourFilterRight.setTypeVariable(0.f);

	mPreDelay.init(sampleRate, 3.f);

}

//==============================================================================================================================
// Scales the spread parameters to the appropriate values  
//==============================================================================================================================

void Eko::spreadParams(const float& spread)
{
	mLfoAmount = (spread * 0.6f) + 0.15f;
	float a = (spread * 30.f) + 10.f;
	mTimeVariation = pow(10.0, a / 20.0);
}



void Eko::diffusionCalcs(const float& diff)
{
	mDiffusionArray[0] = 0.66f * diff;
	mDiffusionArray[1] = 0.0286714f + ((0.66f - 0.0286714f) * diff);
	mDiffusionArray[2] = 0.0571429f + ((0.66f - 0.0571429f) * diff);
	mDiffusionArray[3] = 0.0857143f + ((0.66f - 0.0857143f) * diff);
	mDiffusionArray[4] = 0.0857143f + ((0.66f - 0.0857143f) * diff);
	mDiffusionArray[5] = 0.0571429f + ((0.66f - 0.0571429f) * diff);
	mDiffusionArray[6] = 0.0286714f + ((0.66f - 0.0286714f) * diff);
	mDiffusionArray[7] = 0.66f * diff;
}

//==============================================================================================================================
// Oscillates between +/-0.5 every second. 
//==============================================================================================================================

void Eko::phase()
{
	mSwitchLfoDirectionUp ? mPhase += .1f / mSampleRate : mPhase -= .1f / mSampleRate;
	if (mPhase >= 1.f || mPhase <= -0.f)
	{
		mSwitchLfoDirectionUp = !mSwitchLfoDirectionUp;
	}
}

//==============================================================================================================================
// Clips the signal between the max and min. 
//==============================================================================================================================

float Eko::sclClip(const float& ctl, const float& min, const float& max)
{
	float range = max - min;
	float a = ctl > min ? ctl : min;
	float b = a - min;
	float c = b == 0.f ? 0.f : b / range;
	return c;
}


void Eko::scl(const float& tx, const float& tPlus, float& tx2, float& lx)
{
	float a = sclClip(tPlus, 0.25f, 1.f);
	tx2 = tx * mSampleRate * 0.001f;
	lx = a * mSampleRate * 0.001f;
}

float Eko::lfoStage(const float& i, const float& phase, const float& n, const float& t, const float& tx, const float& lx)
{
	float a = i * n + phase;
	float b = abs(a + a);
	float c = t * tx;
	float d = a * lx + c;
	return d;
}

//==============================================================================================================================
// Varies the timings from EkoTime to produce a more prominant diffusion effect dependent on the Spread param. 
// The greater the spread the further apart the timimgs become creating more 'space' 
//==============================================================================================================================

void Eko::lfo(const float& txIn, const float& lfoAmt, float* t1, float* t2)
{
	float n = 0.125f;//1.f / mMaxDiffusionSteps;

	float tx, lx;

	scl(txIn, lfoAmt, tx, lx);
	phase();

	for (int i = 0; i < mMaxDiffusionSteps; i++)
	{
		mTimingsLeft[i] = lfoStage(i, mPhase, n, t1[i], tx, lx);
		mTimingsRight[i] = lfoStage(i, mPhase, n, t2[i], tx, lx);
	}

}

//==============================================================================================================================
// Applies Highpass and lowpass filtering to the incoming signal
//==============================================================================================================================

void Eko::eq(const float& hpCutoff, const float& lpCutoff, float& outL, float& outR)
{
	mHpCutoff = hpCutoff * 2000.f;
	mLpCutoff = lpCutoff * 18800.f + 200.f;

	mEqFilterHighpassLeft.setCutoff(mHpCutoff);
	mEqFilterHighpassRight.setCutoff(mHpCutoff);
	mEqFilterLowpassLeft.setCutoff(mLpCutoff);
	mEqFilterLowpassRight.setCutoff(mLpCutoff);

	float lHp = 0.f; float rHp = 0.f; float lLp = 0.f; float rLp = 0.f;

	lHp = mEqFilterHighpassLeft.process(mSampleLeft);
	rHp = mEqFilterHighpassRight.process(mSampleRight);
	lLp = mEqFilterLowpassLeft.process(lHp);
	rLp = mEqFilterLowpassRight.process(rHp);
	outL = lLp;
	outR = rLp;
}

void Eko::fb(const float& LIn, const float& RIn, const float& fb, float& returnLeft, float& feedbackLeft, float& returnRight, float& feedbackRight)
{
	returnLeft = feedbackLeft * fb + LIn;
	returnRight = feedbackRight * fb + RIn;
}

//==============================================================================================================================
// Reads from 2 taps in the diffuser array depending of the diffuser position,
// the lower the diffuser position the less diffusers the sound travels through before being read.
//==============================================================================================================================

void Eko::read(float& returnLeft, float& returnRight, const float& diffusion)
{
	float pickup = 6.5f * diffusion + 3.f;
	float clipedPickup = pickup > 7.f ? 7.f : pickup;

	int32_t pos = floor(clipedPickup - 0.5f);
	float fract = clipedPickup - pos;
	int32_t posP1 = pos + 1;

	float L0 = mLtoScanX8[pos];
	float L1 = mLtoScanX8[posP1];
	float R0 = mRtoScanX8[pos];
	float R1 = mRtoScanX8[posP1];

	float invfract = (1.f - fract);
	float invShape = (2.f - invfract) * invfract;
	float shape = (2.f - fract) * fract;

	returnLeft = (L0 * invShape) + (L1 * shape);
	returnRight = (R0 * invShape) + (R1 * shape);
}

//==============================================================================================================================
// Performs high or lowpass filtering on reverb output and applies resonance. 
//==============================================================================================================================

void Eko::colour(const float& sampleLeft, const float& sampleRight, const float& cutoff, const float& emphasis, float& returnLeft, float& returnRight)
{
	float filterCutoff = 0.f;
	
	if (cutoff <= 0.f)
	{
		filterCutoff = (1 + cutoff) * 13000.f + 200.f;
	}
	else if (cutoff > 0.f)
	{
		filterCutoff = (cutoff) * 3000.f;
	}

	float filterRes = 3.f * emphasis + 1.f;

	mColourFilterLeft.setTypeVariable(cutoff);
	mColourFilterRight.setTypeVariable(cutoff);
	mColourFilterLeft.setCutoff(filterCutoff);
	mColourFilterRight.setCutoff(filterCutoff);
	mColourFilterLeft.setResonance(filterRes);
	mColourFilterRight.setResonance(filterRes);

	returnLeft = mColourFilterLeft.process(sampleLeft);
	returnRight = mColourFilterRight.process(sampleRight);
}

//==============================================================================================================================
// Mixes the wet and dry signal
// Also takes into account of the feedback parameter to stop oversaturation at high feedback levels.
//==============================================================================================================================

void Eko::mix(const float& l0, const float& r0, const float& l1, const float& r1, const float& fb, const float& mix, float& returnLeft, float& returnRight)
{

	float dryGain = (2 - (1 - mix)) * (1 - mix);

	float fbCalc = fb * fb * -1.f;

	float oneMinus = 1.f - fbCalc;

	float oneOver = 1 / oneMinus;

	float wetShape = (2 - mix) * mix;

	float wetGain = oneOver * wetShape;

	float l0a = dryGain * l0;
	float r0a = dryGain * r0;

	float l1a = wetGain * l1;
	float r1a = wetGain * r1;

	returnLeft = l1a + l0a;
	returnRight = r1a + r0a;
}

//==============================================================================================================================
// Main reverb processing block.
//==============================================================================================================================

void Eko::process(const float& lIn, const float& rIn, float* lArray, float* rArray, const float& diffusion, const float& size, const float& LPAmmount, const float& HPAmmount, 
	const float& fbck, const float& mixAmount, const float& preDelayTime, const float& preDelayFeedback, const float& colourCutoff, const float& colourEmphasis, float& returnLeft, float& returnRight)
{
	mPassThroughLeft = lIn; mPassThroughRight = rIn;
	mSampleLeft = lIn; mSampleRight = rIn;

	spreadParams(size);
	lfo(mTimeVariation, mLfoAmount, lArray, rArray);
	diffusionCalcs(diffusion);

	float leftToPreDelay = 0.f, rightToPreDelay = 0.f;
	eq(HPAmmount, LPAmmount, leftToPreDelay, rightToPreDelay);

	float leftToFeedback = 0.f, rightToFeedback = 0.f;
	mPreDelay.process(leftToPreDelay, rightToPreDelay, preDelayTime, preDelayFeedback, leftToFeedback, rightToFeedback);

	float leftToDiffuser = 0.f, rightToDiffuser = 0.f;
	fb(leftToFeedback, rightToFeedback, fbck, leftToDiffuser, mDiffusionFeedbackLeft, rightToDiffuser, mDiffusionFeedbackRight);

	mMultiTapDelayL.processMultiTapDiffusion(leftToDiffuser, mDiffusionFeedbackLeft, mTimingsLeft, mDiffusionArray, mLtoScanX8);
	mMultiTapDelayR.processMultiTapDiffusion(rightToDiffuser, mDiffusionFeedbackRight, mTimingsLeft, mDiffusionArray, mRtoScanX8);

	float leftToColour = 0.f, rightToColour = 0.f;
	read(leftToColour, rightToColour, diffusion);

	float leftToMix = 0.f, rightToMix = 0.f;
	colour(leftToColour, rightToColour, colourCutoff, colourEmphasis, leftToMix, rightToMix);

	mix(mPassThroughLeft, mPassThroughRight, leftToMix, rightToMix, fbck, mixAmount, returnLeft, returnRight);
}