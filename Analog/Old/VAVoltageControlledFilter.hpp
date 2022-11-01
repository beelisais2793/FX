// https://github.com/gratrix/vcv-gratrix/blob/aba7b6768cf4f39c826763c40160aa3bdf1846d3/src/VCF.cpp
// The clipping function of a transistor pair is approximately tanh(x)
// TODO: Put this in a lookup table. 5th order approx doesn't seem to cut it
inline float clip(float x) {
	return tanhf(x);
}

struct LadderFilter {
	float cutoff = 1000.0f;
	float resonance = 1.0f;
	float state[4] = {};

	void calculateDerivatives(float input, float *dstate, const float *state) {
		float cutoff2Pi = 2*M_PI * cutoff;

		float satstate0 = clip(state[0]);
		float satstate1 = clip(state[1]);
		float satstate2 = clip(state[2]);

		dstate[0] = cutoff2Pi * (clip(input - resonance * state[3]) - satstate0);
		dstate[1] = cutoff2Pi * (satstate0 - satstate1);
		dstate[2] = cutoff2Pi * (satstate1 - satstate2);
		dstate[3] = cutoff2Pi * (satstate2 - clip(state[3]));
	}

	void process(float input, float dt) {
		float deriv1[4], deriv2[4], deriv3[4], deriv4[4], tempState[4];

		calculateDerivatives(input, deriv1, state);
		for (int i = 0; i < 4; i++)
			tempState[i] = state[i] + 0.5f * dt * deriv1[i];

		calculateDerivatives(input, deriv2, tempState);
		for (int i = 0; i < 4; i++)
			tempState[i] = state[i] + 0.5f * dt * deriv2[i];

		calculateDerivatives(input, deriv3, tempState);
		for (int i = 0; i < 4; i++)
			tempState[i] = state[i] + dt * deriv3[i];

		calculateDerivatives(input, deriv4, tempState);
		for (int i = 0; i < 4; i++)
			state[i] += (1.0f / 6.0f) * dt * (deriv1[i] + 2.0f * deriv2[i] + 2.0f * deriv3[i] + deriv4[i]);
	}
	void reset() {
		for (int i = 0; i < 4; i++) {
			state[i] = 0.0f;
		}
	}
};

