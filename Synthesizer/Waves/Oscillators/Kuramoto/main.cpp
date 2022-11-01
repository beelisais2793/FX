#include "simulator.h"
#include <cstdlib>

void Single_Run(int argc, char* argv[])
{
	int N1 = atoi(argv[1]);
	int N2  = atoi(argv[2]);
	Real dw = atof(argv[3]);
	Real w1 = -dw/2;
	Real w2 = dw/2;
	Real D1 = atof(argv[4]);
	Real D2 = atof(argv[5]);
	Real sim_t = atof(argv[6]);
	Phase::Init_Rand(time(NULL));
	Simulator sim(N1,N2,w1,w2,D1,D2,1);
	sim.Run(sim_t, 0.01, 0.05); // simulation time, time steps, saving interval (zero means no save)
}

void Omega_Chnage(int argc, char* argv[])
{
	int N1 = atoi(argv[1]);
	int N2  = atoi(argv[2]);
	Real domega = atof(argv[3]);
	Real domega_end = atof(argv[4]);
	Real ddw = atof(argv[5]);
	Real D = atof(argv[6]);
	Real eq_t = atof(argv[7]);
	Real sim_t = atof(argv[8]);
	Phase::Init_Rand(time(NULL));
	Real w1 = -domega/2;
	Real w2 = domega/2;
	Simulator sim(N1,N2,w1,w2,D,D,1);
	sim.Run(eq_t, 0.05, 0.1);
	if (domega_end > domega)
		while (domega <= domega_end)
		{
			w1 = -domega/2;
			w2 = domega/2;
			sim.Set_Omega(w1,w2);
			sim.Run(sim_t, 0.05, 0.1);
			domega += ddw;
		}
	else
		while (domega >= domega_end)
		{
			w1 = -domega/2;
			w2 = domega/2;
			sim.Set_Omega(w1,w2);
			sim.Run(sim_t, 0.05, 0.1);
			domega -= ddw;
		}
		
}

int main(int argc, char* argv[])
{
//	Omega_Chnage(argc, argv);
	Single_Run(argc, argv);
}
