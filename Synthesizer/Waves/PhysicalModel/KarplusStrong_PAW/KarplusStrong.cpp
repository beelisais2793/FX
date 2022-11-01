#include <KarplusStrong.h>
#include <math.h>
#include <stdio.h>


KarplusStrong::KarplusStrong() {}

KarplusStrong::KarplusStrong(float fs, float minFrequency, float initialFrequency)
{
	setup(fs, minFrequency, initialFrequency);
}

int KarplusStrong::setup(float fs, float minFrequency, float initialFrequency)
{
	fs_ = fs;
	unsigned int bufferLength = 1 + (unsigned int)(fs_ / minFrequency + 0.5f);
	delayBuffer.resize(bufferLength, 0);
	setFrequency(initialFrequency);

	return 0;
}

float KarplusStrong::process(float input) 
{	
	updateReadPointer();
	
	float prev = interpolatedRead(readPointer - 1.f);
	
	// Difference equation for K-S (including input excitation):
	float in = interpolatedRead(readPointer);
#ifdef KS_CONSTANT_LOWPASS
	// just use an average filter + attenuation on the delay line:
	// y(n) = scaling * x(n) + lossFactor * (y(n-N) + y(n-(N+1)) / 2
	float filterOut = (in + prev) * 0.5f;
#else /* KS_CONSTANT_LOWPASS */
	// add a one-pole lowpass after the average filter + attenuation on the delay line:
	// y(n) = scaling * x(n) + lossFactor * (y(n-N) + y(n-(N+1)))/2 * alpha + y(n - 1) * (1 - alpha)
	float filterOut = (in + prev) * 0.5f * onepoleAlpha  + pastFilterOut * (1.f - onepoleAlpha);
	pastFilterOut = filterOut;
#endif /* KS_CONSTANT_LOWPASS */

	float out = input + lossFactor_ * filterOut;
	delayBuffer[writePointer] = out;

	updateWritePointer();

	return out;
}

void KarplusStrong::process(float* input, float* output, unsigned int length)
{
	for (unsigned int i = 0; i < length; i++)
		output[i] = process(input[i]);
}

void KarplusStrong::setFrequency(float frequency)
{
	delayLength_ = fs_/frequency; // Real value for the period of the first partial
}

#ifdef KS_CONSTANT_LOWPASS
void KarplusStrong::setLossFactor(float lossFactor)
{
	lossFactor_ = lossFactor;
}
#else /* KS_CONSTANT_LOWPASS */
void KarplusStrong::setDamping(float damping)
{
	lossFactor_ = damping * 0.2f + 0.792f; // some hard-coded values to avoid pitch change as damping changes. Better check out the textbook!
	onepoleAlpha = damping;
	if(onepoleAlpha < 0.1)
		onepoleAlpha = 0;
	else if (onepoleAlpha > 1) // keep the feedback to reasonable levels
		onepoleAlpha = 1; // onepole is in bypass
}
#endif /* KS_CONSTANT_LOWPASS */

void KarplusStrong::updateReadPointer()
{
	readPointer = (writePointer - delayLength_ + delayBuffer.size());
	while(readPointer >= delayBuffer.size())
		readPointer -= delayBuffer.size();
}

float KarplusStrong::interpolatedRead(float index)
{
	while(index < 0)
		index += delayBuffer.size();
	int pIndex = (int)index;
	int nIndex = pIndex + 1;
	while(nIndex >= delayBuffer.size())
		nIndex -= delayBuffer.size();
	float frac = index - pIndex;
	float pVal = delayBuffer[pIndex];
	float nVal = delayBuffer[nIndex];

	return linearInterpolation(frac, pVal, nVal);
}

float KarplusStrong::linearInterpolation(float index, float pVal, float nVal)
{
	return pVal + index * (nVal - pVal);
}

void KarplusStrong::updateWritePointer()
{
	if(++writePointer >= delayBuffer.size())
		writePointer = 0;
}

KarplusStrong::~KarplusStrong() 
{
	cleanup();
}	

void KarplusStrong::cleanup()
{
}
