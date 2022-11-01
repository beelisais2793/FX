#pragma once

namespace Analog::Filters::RKLadderFilter
{
	static double clip(double x) {
		// return std::tanh(x);
		// Pade approximant of tanh
		x = clamp(x, -3.f, 3.f);
		return x * (27 + x * x) / (27 + 9 * x * x);
	}


	inline double crossfade(double a, double b, double p) {
		return a + (b - a) * p;
	}

	/** Solves an ODE system using the 4th order Runge-Kutta method */

	void stepRK4(double t, double dt, double x[], int len, std::function<void (double,const double[],double[])> f) {
		double k1[len];
		double k2[len];
		double k3[len];
		double k4[len];
		double yi[len];

		f(t, x, k1);

		for (int i = 0; i < len; i++) {
			yi[i] = x[i] + k1[i] * dt / double(2);
		}
		f(t + dt / double(2), yi, k2);

		for (int i = 0; i < len; i++) {
			yi[i] = x[i] + k2[i] * dt / double(2);
		}
		f(t + dt / double(2), yi, k3);

		for (int i = 0; i < len; i++) {
			yi[i] = x[i] + k3[i] * dt;
		}
		f(t + dt, yi, k4);

		for (int i = 0; i < len; i++) {
			x[i] += dt * (k1[i] + double(2) * k2[i] + double(2) * k3[i] + k4[i]) / double(6);
		}
	}



	struct LadderFilter {
		double omega0;
		double resonance = 1;
		double state[4];
		double input;

		LadderFilter() {
			reset();
			setCutoff(0);
		}

		void reset() {
			for (int i = 0; i < 4; i++) {
				state[i] = 0;
			}
		}

		void setCutoff(double cutoff) {
			omega0 = 2 * double(M_PI) * cutoff;
		}

		void process(double input, double dt) {
			stepRK4(0, dt, state, 4, [&](double t, const double x[], double dxdt[]) {
				double inputt = crossfade(this->input, input, t / dt);
				double inputc = clip(inputt - resonance * x[3]);
				double yc0 = clip(x[0]);
				double yc1 = clip(x[1]);
				double yc2 = clip(x[2]);
				double yc3 = clip(x[3]);

				dxdt[0] = omega0 * (inputc - yc0);
				dxdt[1] = omega0 * (yc0 - yc1);
				dxdt[2] = omega0 * (yc1 - yc2);
				dxdt[3] = omega0 * (yc2 - yc3);
			});

			this->input = input;
		}

		double lowpass() {
			return state[3];
		}
		double highpass() {
			return clip((input - resonance * state[3]) - 4 * state[0] + 6 * state[1] - 4 * state[2] + state[3]);
		}
	};
}
