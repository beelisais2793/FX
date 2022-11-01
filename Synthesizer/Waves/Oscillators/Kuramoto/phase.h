#ifndef _PHASE_
#define _PHASE_

#include <iostream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <cmath>


typedef double Real;

using namespace std;

struct Phase{
	Real value;
	static gsl_rng* gsl_r;

	static void Init_Rand (long int seed = 0)
	{
		gsl_rng_env_setup();
		const gsl_rng_type * T;
		T = gsl_rng_default;
		gsl_r = gsl_rng_alloc (T);
		gsl_rng_set(gsl_r, seed);
	}
	
	Phase();
	Phase(Real input_phase);
	Phase(const Phase& p);

	Phase Transform()
	{
		int n = floor(0.5 + value / (2*M_PI));
		value -= 2*n*M_PI;
	}

	Phase Add_Noise(const Real& amplitude)
	{
		Real r = gsl_ran_gaussian(gsl_r, amplitude);
		value += r;
		Transform();
	}
	
	const Phase operator+ (const Phase& p1) const
	{
		Phase result(p1);
		result.value += value;
		result.Transform();
		return result;
	}

	const Phase operator+ (const Real& p1) const
	{
		Phase result(p1);
		result.value += value;
		result.Transform();
		return result;
	}

	const Phase operator* (const Real& p1) const
	{
		Phase result(p1);
		result.value *= p1;
		result.Transform();
		return result;
	}

	void operator+= (const Real& p1)
	{
		value += p1;
		Transform();
	}

	void operator+= (const Phase& p1)
	{
		value += p1.value;
		Transform();
	}

	const Phase operator- (const Phase& p1) const
	{
		Phase result(0);
		result.value = value - p1.value;
		result.Transform();
		return result;
	}

	friend std::ostream& operator<<(std::ostream& os, const Phase t)
	{
		os << t.value;
		return (os);
	}


	friend Real sin(const Phase& p);
	friend Real cos(const Phase& p);
};

Phase::Phase()
{
	value = gsl_ran_flat (gsl_r, 0, 2*M_PI);
}

Phase::Phase(Real input_phase)
{
	value = input_phase;
}

Phase::Phase(const Phase& p)
{
	value = p.value;
}

Real sin(const Phase& p)
{
	return sin(p.value);
}

Real cos(const Phase& p)
{
	return cos(p.value);
}

gsl_rng * Phase::gsl_r;

#endif
