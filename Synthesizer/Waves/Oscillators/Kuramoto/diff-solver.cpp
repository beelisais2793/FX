#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "phase.h"

using namespace std;

Real alpha = 1.5;

Real K = 1;
Real kappa = 0.5;
Real D1 = 0.01;
Real D2 = 0.01;
Real D = 0.01;
Real dw = 0.2;
Real w1 = -dw/2;
Real w2 = dw/2;
Real dt = 1.0/1024;
Real r1,r2, r1_new, r2_new;
Real psi1,psi2, psi1_new, psi2_new;

ofstream output ;

static gsl_rng* gsl_r;

static void Init_Rand (long int seed = 0)
{
	gsl_rng_env_setup();
	const gsl_rng_type * T;
	T = gsl_rng_default;
	gsl_r = gsl_rng_alloc (T);
	gsl_rng_set(gsl_r, seed);
}

void Transform()
{
	int n1 = floor(psi1 / (2*M_PI));
	int n2 = floor(psi2 / (2*M_PI));
	psi1 -= n1*2*M_PI;
	psi2 -= n2*2*M_PI;
}

void One_Step(Real dt)
{
	Real noise_amplitude = D*sqrt(2*dt) / 1024;
	Real dr1 = gsl_ran_gaussian(gsl_r, noise_amplitude);
	Real dr2 = gsl_ran_gaussian(gsl_r, noise_amplitude);
	Real dpsi1 = gsl_ran_gaussian(gsl_r, noise_amplitude);
	Real dpsi2 = gsl_ran_gaussian(gsl_r, noise_amplitude);

	Real psi1_integrand = w1 + 0.5*K*(r1*r1*r1*r1 + 1)*(r1*sin(alpha) + kappa*r2*sin(psi2 - psi1 + alpha));
	Real psi2_integrand = w2 + 0.5*K*(r2*r2*r2*r2 + 1)*(r2*sin(alpha) + kappa*r1*sin(psi1 - psi2 + alpha));
	Real r1_integrand = 0.5*K*(1-r1*r1*r1*r1)*(r1*cos(alpha) + kappa*r2*cos(psi2 - psi1 + alpha)) - D1*r1;
	Real r2_integrand = 0.5*K*(1-r2*r2*r2*r2)*(r2*cos(alpha) + kappa*r1*cos(psi2 - psi1 - alpha)) - D2*r2;

	psi1_new = psi1 + 0.5*(psi1_integrand*dt + dpsi1);
	psi2_new = psi2 + 0.5*(psi2_integrand*dt + dpsi2);
	r1_new = r1 + 0.5*(r1_integrand*dt + dr1);
	r2_new = r2 + 0.5*(r2_integrand*dt + dr2);

	Real psi1_new_integrand = w1 + 0.5*K*(r1_new*r1_new*r1_new*r1_new + 1)*(r1_new*sin(alpha) + kappa*r2_new*sin(psi2_new - psi1_new + alpha));
	Real psi2_new_integrand = w2 + 0.5*K*(r2_new*r2_new*r2_new*r2_new + 1)*(r2_new*sin(alpha) + kappa*r1_new*sin(psi1_new - psi2_new + alpha));

	Real r1_new_integrand = 0.5*K*(1-r1_new*r1_new*r1_new*r1_new)*(r1_new*cos(alpha) + kappa*r2_new*cos(psi2_new - psi1_new + alpha)) - D1*r1_new;
	Real r2_new_integrand = 0.5*K*(1-r2_new*r2_new*r2_new*r2_new)*(r2_new*cos(alpha) + kappa*r1_new*cos(psi2_new - psi1_new - alpha)) - D2*r2_new;

	psi1 += psi1_new_integrand*dt + dpsi1;
	psi2 += psi2_new_integrand*dt + dpsi2;
	r1 += r1_new_integrand*dt + dr1;
	r2 +=r2_new_integrand*dt + dr2;

	Transform();
}

void Eq(const Real& duration, const Real& dt, const Real& interval, bool time_info_flag = false)
{
	int savestep, steps;
	steps = (int) round(duration / dt);
	if (interval == 0)
		savestep = 200000;
	else
		savestep = (int) round(interval / dt);
	for (int i = 0; i < steps; i++)
	{
		if (i % savestep == 0)
			output << i*dt << "\t" << r1 << "\t" << r2 << "\t" << psi1 << "\t" << psi2 << endl;
		if (i % 100000 == 0 && time_info_flag)
			cout << "\r" << "Step is at: " << i << flush;
		One_Step(dt);
	}
	if (time_info_flag)
		cout << "\tDone" << endl;
}

void Ave(const  int& steps, const Real dt, Real& mr1, Real& mr2)
{
	mr1 = mr2 = 0;
	int counter = 0;
	for (int i = 0; i < steps; i++)
	{
		One_Step(dt);
		if (i % 10 == 0)
		{
			mr1 += r1;
			mr2 += r2;
			counter++;
		}
	}
	mr1 /= counter;
	mr2 /= counter;
}

void Set_Output()
{
	output.close();
	stringstream address;
	address.str("");
	address << "kappa_" << kappa << "_alpha_" << alpha << "_D_" << std::setprecision(10) << D << ".dat";
	output.open(address.str().c_str());
	cout << address.str() << endl;
}

void Single_Run(int argc, char* argv[])
{

//	psi1 = 2*M_PI*rand() / RAND_MAX;
//	psi2 = 2*M_PI*rand() / RAND_MAX;
	alpha  = atof(argv[1]);
	dw = atof(argv[2]);
	D1 = atof(argv[3]);
	D2 = atof(argv[4]);
	K = 1;
	Real sim_t = atof(argv[5]);

	w1 = -dw/2;
	w2 = dw/2;

	r1 = r2 = 0.1;

	Set_Output();

	Eq(sim_t, 0.001, 0.01,true);
}

void Omega_Chnage(int argc, char* argv[])
{
	alpha  = atof(argv[1]);
	Real domega = atof(argv[2]);
	Real domega_end = atof(argv[3]);
	Real ddw = atof(argv[4]);
	D = atof(argv[5]);

	Real eq_t = atof(argv[6]);
	Real sim_t = atof(argv[7]);

	K = 1;
	w1 = -domega/2;
	w2 = domega/2;
	D1 = D2 = D;

	r1 = r2 = 0.9;

	Real dt = 0.001;
	Eq(eq_t, dt, 0.1);

	if (domega_end > domega)
		while (domega <= domega_end)
		{
			dw = domega;
			w1 = -domega/2;
			w2 = domega/2;

			Set_Output();
			Eq(sim_t, dt, 0.1);
			domega += ddw;
		}
	else
		while (domega >= domega_end)
		{
			w1 = -domega/2;
			w2 = domega/2;

			Set_Output();
			Eq(sim_t, dt, 0.1);
			domega -= ddw;
		}
}

void Noise_Chnage(int argc, char* argv[])
{
	kappa = atof(argv[1]);
	alpha  = atof(argv[2]);
	Real dD = atof(argv[3]);
	Real D_end = atof(argv[4]);

	Real eq_t = atof(argv[5]);
	Real sim_t = atof(argv[6]);

	K = 1;
	
	w1 = 0;
	w2 = 0;



	r1 = r2 = 0.9;

	Real dt = 1.0 / 1024;
	Eq(eq_t, dt, 0.1);

	D = 0;

	if (D_end > dD)
		while (D <= D_end)
		{
			D1 = D2 = D;

			Set_Output();
			Eq(sim_t, dt, 0.1);
			D += dD;
		}
	else
		while (D >= D_end)
		{
			D1 = D2 = D;

			Set_Output();
			Eq(sim_t, dt, 0.1);
			D -= dD;
		}
}

Real tmax = 100000;
Real r2c = 0.7;

Real Go_Till_Spike(const Real& dt)
{
	Real t = 0;
	while (r2 > r2c && t < (tmax+1))
	{
		One_Step(dt);
		t += dt;
	}
	while (r2 < r2c)
	{
		One_Step(dt);
		t += dt;
	}
	return t;
}

Real Spike_Interval(const Real& omega, const Real& D, const Real& eq_t)
{
	w1 = omega;
	D1 = D2 = D;
	r1 = r2 = 0.99;

	Real dt = 0.01;
	Eq(eq_t, dt, 1000);

	Real t = Go_Till_Spike(dt);
	if (t > tmax)
		t = Go_Till_Spike(dt);
	if (t < tmax)
		t = Go_Till_Spike(dt);
	return (t);
}

Real Spike_Interval(int argc, char* argv[])
{
	alpha  = atof(argv[1]);
	Real D = atof(argv[2]);
	Real eq_t = atof(argv[3]);
	Real omega = atof(argv[4]);
	Real factor = atof(argv[5]);
	tmax = atof(argv[6]);
	Real omega_c, domega;

	K = 1;
	w2 = 0;

	Real t = tmax+1;

	while (t > tmax)
	{
		cout << "#" << setprecision(20) << omega << endl;
		omega_c = omega;
		omega *= factor;
		t = Spike_Interval(omega, D, eq_t);
	}
	domega = omega - omega_c;

	cout << setprecision(10) << domega << "\t" << t << endl;

	factor = 1.5;
	while (t < tmax && t > 10)
	{
		domega *= factor;
		omega = omega_c + domega;
		t = Spike_Interval(omega, D, eq_t);
		if (t < tmax)
			cout << setprecision(20) << omega - omega_c << "\t" << t << endl;
	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

	Init_Rand(time(NULL));

//	Single_Run(argc, argv);
//	Omega_Chnage(argc,argv);
	Noise_Chnage(argc,argv);
//	Real t = Spike_Interval(argc, argv);

//	Trace_k(100000,100000, dt);
}
