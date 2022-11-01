#include "LPF.hpp"
#include "../../utils/audio/Audio.hpp"


Sample LPF::Apply(Sample s)
{	
	x[0] = s;
	double Q = 1;
	double w0 = (2 * M_PI) * (cutoff / (double) SAMPLE_RATE);
	double cos = std::cos(w0);
	double a = (std::sin(w0)) / (2.0 * Q);
	double b0 = (1.0 - cos) / 2.0;
	double b1 = (1.0 - cos);
	double b2 = b0;
	double a0 = 1.0 + a;
	double a1 = -2.0 * cos;
	double a2 = 1.0 - a;

	y[0] = (b0/a0) * x[0] + (b1/a0) * x[1] + (b2/a0) * x[2] - (a1/a0) * y[1] - (a2/a0) * y[2];

	for (int i = 0; i < sizeof(y) / sizeof(double) - 1; i++)
		y[i + 1] = y[i];

	for (int i = 0; i < sizeof(x) / sizeof(double) - 1; i++)
		x[i + 1] = x[i];

	return y[0];
}
