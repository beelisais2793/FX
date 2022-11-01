#ifndef _SIMULATOR_
#define _SIMULATOR_

#include <sstream>
#include <fstream>
#include <iomanip>
#include "cluster.h"

struct Simulator{
	Cluster c1;
	Cluster c2;

	Real t;

	stringstream info;
	ofstream output;
//	ofstream speed_output;

	Simulator(const Real dt, const int N1, const  int N2, const Real omega1, const Real omega2, const Real D, const Real K);
	~Simulator();

	void Set_dt(const Real input_dt);
	void Set_D(const Real input_D);
	void Set_kappa(const Real input_kappa);
	void Set_alpha(const Real input_alpha);

	void Set_Output();
	void One_Step();
	void Run(Real duration, const Real& save_interval);

	void Set_Omega(const Real& omega1, const Real& omega2);

	void Write();
};

Simulator::Simulator(const Real dt, const int N1, const  int N2, const Real omega1, const Real omega2, const Real D, const Real K): c1(N1,omega1,D), c2(N2,omega2,D), t(0)
{
	Set_dt(dt);
	Oscillator::K = K;

	c1.Find_Order_Parameter();
	c2.Find_Order_Parameter();
}

Simulator::~Simulator()
{
	output.close();
}

void Simulator::Set_dt(const Real input_dt)
{
	Oscillator::Set_dt(input_dt);
}

void Simulator::Set_D(const Real input_D)
{
	Oscillator::Set_D(input_D);
}

void Simulator::Set_kappa(const Real input_kappa)
{
	c1.Set_kappa(input_kappa);
	c2.Set_kappa(input_kappa);
}

void Simulator::Set_alpha(const Real input_alpha)
{
	c1.Set_alpha(input_alpha);
	c2.Set_alpha(input_alpha);
}

void Simulator::One_Step()
{
	c1.Find_Order_Parameter();
	c2.Find_Order_Parameter();

	c1.All_Interactions(c2);
	c2.All_Interactions(c1);

	c1.Runge_Kutta2_Move_1();
	c2.Runge_Kutta2_Move_1();

	c1.Find_Order_Parameter();
	c2.Find_Order_Parameter();

	c1.All_Interactions(c2);
	c2.All_Interactions(c1);

	c1.Runge_Kutta2_Move_2();
	c2.Runge_Kutta2_Move_2();

	t += Oscillator::dt;
}


void Simulator::Run(Real duration, const Real& save_interval = 0)
{
	long int steps = (int) round(duration / Oscillator::dt);
	int save_steps = (int) round(save_interval / Oscillator::dt);
	if (save_interval == 0)
		save_steps = 0;

	c1.Find_Order_Parameter();
	c2.Find_Order_Parameter();

	for (int i = 0; i < steps; i++)
	{
		if ((i % save_steps == 0) && (save_steps != 0))
		{
			if (i % (100*save_steps) == 0)
				cout << "\r" << "time is: " << t << flush;
			Write();
		}
		One_Step();
	}
	cout << endl;
}

void Simulator::Set_Omega(const Real& omega1, const Real& omega2)
{
	t = 0;
	c1.Set_Omega(omega1);
	c2.Set_Omega(omega2);
}

void Simulator::Set_Output()
{
	output.close();
	info.str("");
	info << "N_" << c1.N+c2.N << "_alpha_" << c1.alpha << "_D_" << c1.os[0].D << "_kappa_" << c1.kappa << ".dat";
	output.open(info.str().c_str());
	cout << info.str() << endl;

//	speed_output.close();
// 	info.str("");
// 	info << "speed_N_" << c1.N+c2.N << "_alpha_" << c2.alpha << "_w1_" << c1.os[0].omega << "_w2_" << c2.os[0].omega << "_D1_" << c1.os[0].D << "_D2_" << c2.os[0].D << "_K_" << Oscillator::K << ".dat";
// 	speed_output.open(info.str().c_str());
}

void Simulator::Write()
{
	c1.Find_Order_Parameter();
	c2.Find_Order_Parameter();
	output << std::setprecision(3) << t << "\t" << c1.r << "\t" << c2.r << "\t" << c1.psi << "\t" << c2.psi << endl;
}

#endif
