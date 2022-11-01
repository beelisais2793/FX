#include <cmath>

namespace Distortion::Diode
{
	// https://github.com/a-carson/DiodeClipper
	template<class temp>
	class DiodeClipper : public FunctionProcessor
	{
	public:

		DiodeClipper()
		{
			// initialise
			x = 0.0f;
			y = 0.0f;

			// default parameters
			R = 1000.0f;
			C = 33e-9f;
			fs = 48000.0f;
			Is = 2.52e-9f;
			Vt = 25.83e-3f;
			Ni = 1.752f;
		};

		void setSampleRate(temp sampleRate)
		{
			fs = sampleRate;
		}

		void setCircuitParameters(temp resistance, temp capacitance)
		{
			R = resistance;
			C = capacitance;
		}

		void setDiodeParameters(temp saturationCurrent, temp thermalVoltage, temp idealityFactor)
		{
			Is = saturationCurrent;
			Vt = thermalVoltage;
			Ni = idealityFactor;
		}

		void initialise()
		{
			cap = Ni * Vt * acosh((2.0f*fs*R*C + 1.0f) * Ni * Vt / (2.0f * Is * R));
		}

		// Non linear function
		temp func(temp Y, temp p)
		{
			return Y * R * C + (Y + (2.0f * Is * R) * sinh(Y /(Ni * Vt)) - p) / (2.0f * fs);
		}

		// Derivative of Non-linear function
		temp dfunc(temp Y)
		{
			return R * C + (1 + (2.0f * Is * R / (Ni * Vt)) * cosh(Y / (Ni * Vt))) / (2.0f * fs);
		}

		// Fast approximation of function
		temp fastFunc(temp Y, temp p)
		{
			temp sinhy = sinh(Y / (Ni * Vt));
			return Y * R *C + (Y + (2.0f * Is * R) * sinhy - p) / (2.0f*fs);

		}

		// Fast aproximation of derivative
		temp fastDfunc(temp Y)
		{
			temp coshy = cosh(Y / (Ni * Vt));
			return R * C + (1 + (2 * Is * R / (Ni * Vt))*coshy) / (2.0f * fs);
		}

		// Main Process
		temp process(temp in)
		{
			iter = 0;
			const temp p = x + in;		
			y = Ni * Vt * asinh(p / (2 * Is * R));
			temp res = func(y, p);
			temp J = dfunc(y);
			temp step = res / J;
			temp cond = fabsf(step);

			while ((cond > tol) && (iter < maxIters))
			{
				// Cap step size if necessary
				if (step > cap)
				{
					step = cap;
				}
				if (step < -1.0f * cap)
				{
					step = -1.0f * cap;
				}

				// Newton step
				y -= step;
				
				// Check argument
				temp arg = y / (Ni * Vt);

				// Compute residual and jacobian
				if (fabsf(arg) < 5)
				{
					res = fastFunc(y, p);
					J = fastDfunc(y);
				}
				else
				{
					res = func(y, p);
					J = dfunc(y);
				}

				// Calculate step
				step = res / J;

				iter++;
				cond = fabsf(step);
			}

			// fail safe
			if (y != y)
			{
				y = 0;
			}

			// update state variable
			x =  4.0f * fs * y * R * C - x;
			return y;
		}

		double Tick(double I, double A=1, double X=1, double Y=1)
		{
			return A*process(I);
		}

	private:

		// Sample Rate
		temp fs;

		// state variable
		temp x;

		// output variable
		temp y;

		// Circuit parameters
		temp C;									// capactance
		temp R;									// resistance
		temp Is;								// saturation current
		temp Vt;								// thermal voltage
		temp Ni;								// ideality factor

		// Newton raphson parameters
		temp cap;
		const temp tol = 1e-7;						// tolerance
		const unsigned int maxIters = 25;  // maximum number of iterations
		unsigned int iter = 0;
	};
}


