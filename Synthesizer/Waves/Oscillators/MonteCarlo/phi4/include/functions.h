#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "stdio.h"
#include "stdlib.h"
#include "lattice.h"
#include "ranlxd.h"
#include "math.h"

void GaussianMom();
void GaussianMomCheck();
double Hamiltonian(double act);
void MovePhi(double Nstep, double tau);
void MoveMom(double Nstep, double tau, double kappa, double lambda);
void LeapFrog(int Nstep, int tau, double kappa, double lambda);
void Termalization(int Nstep, int tau, double kappa, double lambda, int Ntherm);
void MonteCarlo(int Nstep, int tau, double kappa, double lambda);
double MonteCarloD(int Nstep, int tau, double kappa, double lambda);
void Magnetization_w(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm);
void Magnetization_wo(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm);
void MagnetizationAbs_w(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm);
void MagnetizationAbs_wo(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm);
void Susceptibility(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm);
void BinderCumulant(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm);
void Results(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm);
#endif
