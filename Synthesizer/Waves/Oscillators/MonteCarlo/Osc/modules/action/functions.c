
/*******************************************************************************
* File functions.c
*
* All function the program needs.
*
*******************************************************************************/
#define FUNCTIONS_C

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "global.h"
#include "start.h"
#include "random.h"

int i,j;
double A,DS,C;
float r[2048];

/*It computes the Lagrangian of the system.*/
double Lagrangian(double x, double y,double M,double W)
{
  return (M*0.5)*(x-y)*(x-y)+(0.25)*M*W*W*(x*x+y*y);
}

/*It computes the action of the system.*/
double Action(int N, double M, double W)
{
  double S=0;
  int j,i;

  for(i=0;i<N;i++)
  {
    j=(i+1)%N;
    S=S+Lagrangian(xx[j],xx[i],M,W);
  }
  return S;
}

/*It computes the variation of the action of the system  with respect to xx[i]=xx[i]+A.*/
double DeltaS(int i, double A, int N, double M, double W)
{
  double DeltaS=0;

  DeltaS= M*(2*A*xx[i%N]+A*A-A*(xx[(i+1)%N]+xx[(i-1+N)%N]))+0.5*M*W*W*(2*A*xx[i%N]+A*A);

  return DeltaS;

}

/*It computes the energy gap E_1-E_0.*/
double D_energy(double Cplus, double Cminus, double C)
{
  return acosh( (Cplus + Cminus)/(2*C) );
}

/*It runs ThermalT steps of  Metropolis.*/
void Term(int N, double M, double W, int ThermalT, double L)
{
  /*Inizzializzo xx a 0 */
  for(i=0; i<N; i++)
  {
    xx[i]=0;
  }

  /*Accetta o rigetta finche non si termalizza */
  for(i=0; i<ThermalT; i++)
  {
    ranlxs(r,2*N);
    for(j=0; j<N; j++)
    {
      A=(double)L*(r[j+N]-0.5);
      DS=DeltaS(j,A,N,M,W);

      if(exp(-1*DS) > r[j])
      xx[j]=xx[j]+A;
    }
  }
} 

/*Metropolis step.*/
void AggCatena(int N, double M, double W, double L)
{
  ranlxs(r,2*N);
  for(j=0; j<N; j++)
  {
    A=(double)L*(r[j+N]-0.5);
    DS=DeltaS(j,A,N,M,W);

    if(exp(-1*DS) > r[j])
    xx[j]=xx[j]+A;
  }
}

/*It computes the correlator: < xx[t] xx[0] >.*/
double Correlator(int t, int N)
{
  C=0;

  for(i=0; i<N; i++)
  {
    C = C + xx[i]*xx[(i+t)%N];
  }

  return C/(double)N;
}
