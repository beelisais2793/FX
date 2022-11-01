#ifndef _CLUSTER_
#define _CLUSTER_

#include "oscillator.h"

struct Cluster{
	int N;
	Oscillator* os;
	Real r; //order parameter
	Real psi;
	Real kappa;
	Phase alpha;

	Cluster(int input_N, Real omega, Real D);
	Cluster(int input_N);
	Cluster(const Cluster& c);
	~Cluster();

	void Set_Omega(const Real &);
	void Set_D(const Real &);
	void Set_kappa(const Real &);
	void Set_alpha(const Real &);

	void Reset();
	void Self_Interact();
	void Interact(Cluster& c);
	void All_Interactions(Cluster& c);

	void Runge_Kutta2_Move_1();
	void Runge_Kutta2_Move_2();

	void Find_Order_Parameter();

	void Make_Random();
	void Make_Ordered(const Real input_phase);

	void Copy_Interaction(Cluster& c);

	void Write_Interactions(ostream& out);

	const Cluster& operator= (const Cluster& c);
	const Cluster& operator+= (const Cluster& c);
	friend std::ostream& operator<<(std::ostream& out_s, const Cluster& box); // Save
};

Cluster::Cluster(int input_N, Real omega = 0, Real D = 0)
{
	N = input_N;
	os = new Oscillator[N];
	Set_Omega(omega);
	Set_D(D);
}

Cluster::Cluster(const Cluster& c)
{
	N = c.N;
	os = new Oscillator[N];
	for (int i = 0; i < N; i++)
		os[i] = c.os[i];
}

Cluster::~Cluster()
{
	if (N != 0)
		delete [] os;
}

void Cluster::Set_Omega(const Real& omega)
{
	for (int i = 0; i < N; i++)
		os[i].Set_omega(omega);
}

void Cluster::Set_D(const Real& D)
{
	for (int i = 0; i < N; i++)
		os[i].Set_D(D);
}

void Cluster::Set_kappa(const Real& input_kappa)
{
	kappa = input_kappa;
}

void Cluster::Set_alpha(const Real& input_alpha)
{
	alpha = input_alpha;
}

void Cluster::Reset()
{
	#pragma omp parallel for default(shared)
	for (int i = 0; i < N; i++)
		os[i].Reset();
}

void Cluster::Self_Interact()
{
	for (int i = 0; i < N; i++)
		os[i].Interact(r, psi, alpha);
}

void Cluster::Interact(Cluster& c)
{
	for (int i = 0; i < N; i++)
		os[i].Interact(kappa*c.r, c.psi, alpha);
}

void Cluster::All_Interactions(Cluster& c)
{
	#pragma omp parallel for default(shared)
	for (int i = 0; i < N; i++)
	{
		os[i].Interact(r,psi,alpha);
		os[i].Interact(kappa*c.r, c.psi, alpha);
	}
}

void Cluster::Runge_Kutta2_Move_1()
{
	for (int i = 0; i < N; i++)
		os[i].Runge_Kutta2_Move_1();
}

void Cluster::Runge_Kutta2_Move_2()
{
	for (int i = 0; i < N; i++)
		os[i].Runge_Kutta2_Move_2();
}

void Cluster::Find_Order_Parameter()
{
	Real C = 0;
	Real S = 0;
	#pragma omp parallel for default(shared) reduction(+:C,S)
	for (int i = 0; i < N; i++)
	{
		C += cos(os[i].phi);
		S += sin(os[i].phi);
	}
	C /= N;
	S /= N;
	r = sqrt(C*C + S*S);
	psi = atan2(S,C);
}

void Cluster::Make_Random()
{
	for (int i = 0; i < N; i++)
	{
		os[i].phi = gsl_ran_flat(Phase::gsl_r, 0, 2*M_PI);
		os[i].phi.Transform();
	}
}

void Cluster::Make_Ordered(const Real input_phase)
{
	for (int i = 0; i < N; i++)
	{
		os[i].phi = input_phase;
		os[i].phi.Transform();
	}
}

void Cluster::Write_Interactions(ostream& out)
{
	for (int i = 0; i < N; i++)
		out << os[i].int_sum / 2 << endl;
}

const Cluster& Cluster::operator= (const Cluster& c)
{
	#pragma omp parallel for default(shared)
	for (int i = 0; i < N; i++)
		os[i] = c.os[i];
	return (*this);
}

const Cluster& Cluster::operator+= (const Cluster& c)
{
	#pragma omp parallel for default(shared)
	for (int i = 0; i < N; i++)
		os[i].int_sum += c.os[i].int_sum;
	return (*this);
}

std::ostream& operator<<(std::ostream& out_s, const Cluster& c)
{
	for (int i = 0; i < c.N; i++)
		out_s << c.os[i].phi - psi << endl;
}

#endif
