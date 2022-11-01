static inline double dsp_osc_step(double freq, unsigned int rate)
{
	return rate ? freq / rate : 0.0;
}

/**
 * Increment the time using the step.
 *   @t: The time.
 *   @step: The step.
 */

static inline double dsp_osc_inc(double t, double step)
{
	return fmod(t + step, 1.0);
}

/**
 * Decrement the time using the step.
 *   @t: The time.
 *   @step: The step.
 */

static inline double dsp_osc_dec(double t, double step)
{
	return fmod(fmod(t - step, 1.0) + 1.0, 1.0);
}

/**
 * Generate a sine wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

static inline double dsp_osc_sine(double t)
{
	return sin(2.0 * M_PI * t);
}

/**
 * Generate a square wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

static inline double dsp_osc_square(double t)
{
	return (t < 0.5) ? 1.0 : -1.0;
}

/**
 * Generate a sawtooth wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

static inline double dsp_osc_saw(double t)
{
	return fmod(1.0 + 2.0 * t, 2.0) - 1.0;
}

/**
 * Generate a reverse sawtooth wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

static inline double dsp_osc_rsaw(double t)
{
	return fmod(3.0 - 2.0 * t, 2.0) - 1.0;
}

/**
 * Generate a triangle wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

static inline double dsp_osc_tri(double t)
{
	return 2.0 * fabs(2.0 * t - 1.0) - 1.0;
}
