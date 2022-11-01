#pragma once

namespace Analog::Filters::RKLadderFilter
{
	static DspFloatType clip(DspFloatType x) {
		// return std::tanh(x);
		// Pade approximant of tanh
		x = clamp(x, -3.f, 3.f);
		return x * (27 + x * x) / (27 + 9 * x * x);
	}


	inline DspFloatType crossfade(DspFloatType a, DspFloatType b, DspFloatType p) {
		return a + (b - a) * p;
	}

	/** Solves an ODE system using the 4th order Runge-Kutta method */

	void stepRK4(DspFloatType t, DspFloatType dt, DspFloatType x[], int len, std::function<void (DspFloatType,const DspFloatType[],DspFloatType[])> f) {
		DspFloatType k1[len];
		DspFloatType k2[len];
		DspFloatType k3[len];
		DspFloatType k4[len];
		DspFloatType yi[len];

		f(t, x, k1);

		for (int i = 0; i < len; i++) {
			yi[i] = x[i] + k1[i] * dt / DspFloatType(2);
		}
		f(t + dt / DspFloatType(2), yi, k2);

		for (int i = 0; i < len; i++) {
			yi[i] = x[i] + k2[i] * dt / DspFloatType(2);
		}
		f(t + dt / DspFloatType(2), yi, k3);

		for (int i = 0; i < len; i++) {
			yi[i] = x[i] + k3[i] * dt;
		}
		f(t + dt, yi, k4);

		for (int i = 0; i < len; i++) {
			x[i] += dt * (k1[i] + DspFloatType(2) * k2[i] + DspFloatType(2) * k3[i] + k4[i]) / DspFloatType(6);
		}
	}



	struct LadderFilter : public FilterProcessor {
		DspFloatType omega0;
		DspFloatType resonance = 1;
		DspFloatType state[4];
		DspFloatType input;

		enum {
			LP,
			HP,
		};
		int type = LP;

		LadderFilter() : FilterProcessor() {
			reset();
			setCutoff(0);
		}

		void reset() {
			for (int i = 0; i < 4; i++) {
				state[i] = 0;
			}
		}

		void setCutoff(DspFloatType cutoff) {
			omega0 = 2 * DspFloatType(M_PI) * cutoff;
		}
		void setResonance(DspFloatType r) {
			resonance = r;
		}
		enum {
			PORT_CUTOFF,
			PORT_RESONANCE,
			PORT_LP,
			PORT_HP
		};
		void setPort(int port, DspFloatType v) {
			switch(port) {
				case PORT_CUTOFF: setCutoff(v); break;
				case PORT_RESONANCE: setResonance(v); break;
				case PORT_LP: type = LP; break;
				case PORT_HP: type = HP; break;
			}
		}
		void process(DspFloatType input, DspFloatType dt) {
			stepRK4(0, dt, state, 4, [&](DspFloatType t, const DspFloatType x[], DspFloatType dxdt[]) {
				DspFloatType inputt = crossfade(this->input, input, t / dt);
				DspFloatType inputc = clip(inputt - resonance * x[3]);
				DspFloatType yc0 = clip(x[0]);
				DspFloatType yc1 = clip(x[1]);
				DspFloatType yc2 = clip(x[2]);
				DspFloatType yc3 = clip(x[3]);

				dxdt[0] = omega0 * (inputc - yc0);
				dxdt[1] = omega0 * (yc0 - yc1);
				dxdt[2] = omega0 * (yc1 - yc2);
				dxdt[3] = omega0 * (yc2 - yc3);
			});

			this->input = input;
		}

		DspFloatType lowpass() {
			return state[3];
		}
		DspFloatType highpass() {
			return clip((input - resonance * state[3]) - 4 * state[0] + 6 * state[1] - 4 * state[2] + state[3]);
		}

		DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
			process(I,1.0/sampleRate);
			if(type == HP) return A * highpass();
			return A * lowpass();	
			
		}
	};
}
