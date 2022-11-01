#pragma once
// https://github.com/gratrix/vcv-gratrix/blob/aba7b6768cf4f39c826763c40160aa3bdf1846d3/src/VCO.cpp

namespace Analog::Oscillators::VCO
{
	template <int OVERSAMPLE, int QUALITY>
	struct VoltageControlledOscillator {
		bool analog = false;
		bool soft = false;
		float lastSyncValue = 0.0f;
		float phase = 0.0f;
		float freq;
		float pw = 0.5f;
		float pitch;
		bool syncEnabled = false;
		bool syncDirection = false;

		Decimator<OVERSAMPLE, QUALITY> sinDecimator;
		Decimator<OVERSAMPLE, QUALITY> triDecimator;
		Decimator<OVERSAMPLE, QUALITY> sawDecimator;
		Decimator<OVERSAMPLE, QUALITY> sqrDecimator;
		RCFilter sqrFilter;

		// For analog detuning effect
		float pitchSlew = 0.0f;
		int pitchSlewIndex = 0;

		float sinBuffer[OVERSAMPLE] = {};
		float triBuffer[OVERSAMPLE] = {};
		float sawBuffer[OVERSAMPLE] = {};
		float sqrBuffer[OVERSAMPLE] = {};

		void setPitch(float pitchKnob, float pitchCv) {
			// Compute frequency
			pitch = pitchKnob;
			if (analog) {
				// Apply pitch slew
				const float pitchSlewAmount = 3.0f;
				pitch += pitchSlew * pitchSlewAmount;
			}
			else {
				// Quantize coarse knob if digital mode
				pitch = roundf(pitch);
			}
			pitch += pitchCv;
			// Note C4
			freq = 261.626f * powf(2.0f, pitch / 12.0f);
		}
		void setPulseWidth(float pulseWidth) {
			const float pwMin = 0.01f;
			pw = clamp(pulseWidth, pwMin, 1.0f - pwMin);
		}
		float interpolateLinear(const float* p, float x) {
			int xi = x;
			float xf = x - xi;
			return crossfade(p[xi], p[xi + 1], xf);
		}
		}
		inline float randomNormal() {
			return random::normal();
		}
		void process(float deltaTime, float syncValue) {
			if (analog) {
				// Adjust pitch slew
				if (++pitchSlewIndex > 32) {
					const float pitchSlewTau = 100.0f; // Time constant for leaky integrator in seconds
					pitchSlew += (randomNormal() - pitchSlew / pitchSlewTau) * engineGetSampleTime();
					pitchSlewIndex = 0;
				}
			}

			// Advance phase
			float deltaPhase = clamp(freq * deltaTime, 1e-6, 0.5f);

			// Detect sync
			int syncIndex = -1; // Index in the oversample loop where sync occurs [0, OVERSAMPLE)
			float syncCrossing = 0.0f; // Offset that sync occurs [0.0f, 1.0f)
			if (syncEnabled) {
				syncValue -= 0.01f;
				if (syncValue > 0.0f && lastSyncValue <= 0.0f) {
					float deltaSync = syncValue - lastSyncValue;
					syncCrossing = 1.0f - syncValue / deltaSync;
					syncCrossing *= OVERSAMPLE;
					syncIndex = (int)syncCrossing;
					syncCrossing -= syncIndex;
				}
				lastSyncValue = syncValue;
			}

			if (syncDirection)
				deltaPhase *= -1.0f;

			sqrFilter.setCutoff(40.0f * deltaTime);

			for (int i = 0; i < OVERSAMPLE; i++) {
				if (syncIndex == i) {
					if (soft) {
						syncDirection = !syncDirection;
						deltaPhase *= -1.0f;
					}
					else {
						// phase = syncCrossing * deltaPhase / OVERSAMPLE;
						phase = 0.0f;
					}
				}

				if (analog) {
					// Quadratic approximation of sine, slightly richer harmonics
					if (phase < 0.5f)
						sinBuffer[i] = 1.f - 16.f * powf(phase - 0.25f, 2);
					else
						sinBuffer[i] = -1.f + 16.f * powf(phase - 0.75f, 2);
					sinBuffer[i] *= 1.08f;
				}
				else {
					sinBuffer[i] = sinf(2.f*M_PI * phase);
				}
				if (analog) {
					triBuffer[i] = 1.25f * interpolateLinear(triTable, phase * 2047.f);
				}
				else {
					if (phase < 0.25f)
						triBuffer[i] = 4.f * phase;
					else if (phase < 0.75f)
						triBuffer[i] = 2.f - 4.f * phase;
					else
						triBuffer[i] = -4.f + 4.f * phase;
				}
				if (analog) {
					sawBuffer[i] = 1.66f * interpolateLinear(sawTable, phase * 2047.f);
				}
				else {
					if (phase < 0.5f)
						sawBuffer[i] = 2.f * phase;
					else
						sawBuffer[i] = -2.f + 2.f * phase;
				}
				sqrBuffer[i] = (phase < pw) ? 1.f : -1.f;
				if (analog) {
					// Simply filter here
					sqrFilter.process(sqrBuffer[i]);
					sqrBuffer[i] = 0.71f * sqrFilter.highpass();
				}

				// Advance phase
				phase += deltaPhase / OVERSAMPLE;
				phase = eucmod(phase, 1.0f);
			}
		}

		float sin() {
			return sinDecimator.process(sinBuffer);
		}
		float tri() {
			return triDecimator.process(triBuffer);
		}
		float saw() {
			return sawDecimator.process(sawBuffer);
		}
		float sqr() {
			return sqrDecimator.process(sqrBuffer);
		}
	};
}