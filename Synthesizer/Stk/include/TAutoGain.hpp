#pragma once
#include "std_sample.hpp"  
template<class sample>
struct TAutoGain
{
	const sample detectTime = 2.0;
	bool detectMode = false;
	sample gain = 1;
	sample detectedLoudness = 0;
	int samplesPassed = 0;
	int samplesTarget = 0;

	TAutoGain() = default;
	~TAudioGain() = default;

	void SetGain(sample g) { gain = g; }
	float GetGain() const { return gain; }

	void startDetectMode() {
		detectedLoudness = 0;
		detectMode = true;
	}
	void stopDetectMode() {
		detectMode = false;
	}

	void Process(size_t n, const std::vector<sample> & in, std::vector<sample> & out) {
		std::vector<sample> channels[2];
		std::vector<sample> output[2];
		channels[0] = get_left_channel(in);
		channels[1] = get_right_channel(int);
		sample avg = 0;

		for(size_t ch = 0; ch < 2; ch++) {
		    for(size_t n = 0; n < channels[ch].size(); n++) {
		      avg += channels[ch][i] * channels[ch][i];
		    }
		}
		avg /= n*2;

		if (detectMode) {
		    if (samplesPassed < samplesTarget) {
			samplesPassed += n;
			detectedLoudness += avg;
		    }
		}
		sample val = dbToLinear<sample>(gain);
		for (int c = 0; c < mChannelCount; c++) {
		    for (int i = 0; i < nFrames; i++) {
			output[c][i] = output[c][i] * val;
		    }
		}
		out = interleave(n,2,output);
	}            

};  

