#ifndef PHI4_H
#define PHI4_H
#include "stdio.h"
#include "stdlib.h"
#include "lattice.h"
#include "ranlxd.h"
#include "math.h"

void hopping(int h[V][2*D]);
double action(void);

/*data structure to store all the parameters of the algorithm*/
typedef struct {
                int tlength;  /*trajectory length*/
	            int nstep;       /*leapfrog steps per trajectory*/
                int ntherm ;     /*number of thermalization steps*/
	            int ntraj ;      /*number of trajectories after thermalization*/
              } hmc_params_t;

/*data structure to store all the parameters of the action*/
typedef struct {
                double kappa;
	            double lambda;
              } act_params_t;

#endif
