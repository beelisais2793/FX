#ifndef _OSCILLATOR_
#define _OSCILLATOR_

#include "phase.h"

struct Oscillator{
	Phase phi;
	Phase phi_old;
	Phase delta_phi;
	Real omega;
	static Real D;
	static Real noise_amplitude;
	static Real dt;
	static Real half_dt;
	Real int_sum; //interaction summation

	static Real K;

	Oscillator(const Real& input_omega, const Real&, const Real&);
	Oscillator(const Real& input_omega, const Real& input_D);
	Oscillator();
	Oscillator(const Oscillator&);

	static void Set_D(const Real);
	static void Set_dt(const Real);
	void Set_omega(const Real);

	void Reset();
	void Interact(const Real& factor, const Phase& psi, const Phase& alpha);
	void Noise_Gen();
	void Runge_Kutta2_Move_1();
	void Runge_Kutta2_Move_2();

	Oscillator& operator= (const Oscillator& c);
};

Real Oscillator::D = 0;
Real Oscillator::noise_amplitude = 0;
Real Oscillator::dt = 1.0/64;
Real Oscillator::half_dt = 1.0/128;
Real Oscillator::K = 1;

Oscillator::Oscillator(const Real& input_omega, const Real& input_D, const Real& input_phi) : phi(input_phi), omega(input_omega)
{
	Set_omega(input_omega);
	Set_D(input_D);
	Reset();
}

Oscillator::Oscillator(const Real& input_omega, const Real& input_D) : omega(input_omega)
{
	Set_D(input_D);
	Reset();
}

Oscillator::Oscillator()
{
	Set_omega(0);
	Set_D(0);
	Reset();
}

Oscillator::Oscillator(const Oscillator& os)
{
	phi = os.phi;
	omega = os.omega;
	int_sum = os.int_sum;
}

void Oscillator::Reset()
{
	int_sum = omega;
}

void Oscillator::Set_omega(const Real input_omega)
{
	omega = input_omega;
}

void Oscillator::Set_dt(const Real input_dt)
{
	dt = input_dt;
	half_dt = 0.5*dt;
	noise_amplitude = sqrt(2*D*dt);
}

void Oscillator::Set_D(const Real input_D)
{
	D = input_D;
	noise_amplitude = sqrt(2*D*dt);
}

void Oscillator::Interact(const Real& factor, const Phase& psi, const Phase& alpha)
{
	int_sum += factor*K*sin(psi - phi + alpha);
}

void Oscillator::Noise_Gen()
{
	delta_phi = gsl_ran_gaussian(Phase::gsl_r, noise_amplitude);
	delta_phi.Transform();
}

void Oscillator::Runge_Kutta2_Move_1()
{
	phi_old = phi;

	Noise_Gen();
	phi += int_sum*half_dt;
	phi += delta_phi*0.5;
	phi.Transform();

	Reset();
}

void Oscillator::Runge_Kutta2_Move_2()
{
	phi_old = phi;

	phi = phi_old + int_sum*dt + delta_phi;
	phi.Transform();

	Reset();
}

Oscillator& Oscillator::operator= (const Oscillator& c)
{
	phi = c.phi;
	int_sum = int_sum;
	return (*this);
}

#endif
