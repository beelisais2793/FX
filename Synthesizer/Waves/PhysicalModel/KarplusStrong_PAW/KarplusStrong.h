#include <vector>

#define KS_CONSTANT_LOWPASS // undef to use some more experimental and nicer damping instead
class KarplusStrong
{

	private:
		std::vector<float> delayBuffer;

		float readPointer;
		int writePointer = 0;

		// Sample rate
		float fs_;

		// Karplus-strong coefficients
		float delayLength_;
		float lossFactor_ = 0.989;
#ifndef KS_CONSTANT_LOWPASS
		float onepoleAlpha;
		float pastFilterOut = 0;
#endif /* not KS_CONSTANT_LOWPASS */

		void updateReadPointer();

		void updateWritePointer();

		float interpolatedRead(float index);

	public:
		KarplusStrong();
		KarplusStrong(float fs, float minFrequency, float initialFrequency);
		~KarplusStrong();

		int setup(float fs, float minFrequency, float initialFrequency);
		void cleanup();
		
		float process(float input);

		void process(float* input, float* output, unsigned int length);
		
		void setFrequency(float frequency);

#ifdef KS_CONSTANT_LOWPASS
		void setLossFactor(float lossFactor);
#else /* KS_CONSTANT_LOWPASS */
		void setDamping(float damping);
#endif /* not KS_CONSTANT_LOWPASS */

		static float linearInterpolation(float index, float pVal, float nVal);
};
