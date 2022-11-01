
/*******************************************************************************
*
* File FUNCTIONS.h
*
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifndef FUNCTIONS_C

extern double Lagrangian(double x,double y,double M, double W);
extern double Action(int N,double M, double W);
extern double DeltaS(int i, double A, int N, double M, double W);
extern double D_energy(double Cplus, double Cminus, double C);
extern void Term(int N, double M, double W, int ThermalT, double L);
extern void AggCatena(int N, double M, double W, double L);
extern double Correlator(int t, int N);
#endif
#endif

