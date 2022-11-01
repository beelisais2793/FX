/***************************************************************************
                                LaguerreMethod.h
                             -------------------
    begin                : Jan 19 2017
    copyright            : (C) 2017 by Andres Martinez-Mera
    email                : andresmartinezmera@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#pragma once

#include <complex>
#include <deque>

using namespace std;

#define MAX_IT 1000
#define MT 10
#define EPSS 1e-7
#define EPS 2e-6


/*
  This class implements the Laguerre method for polyinomial root solving:
  
  Given P(x) = (p_N-1)·x^N + (p_N-2)·x^N-2 + ... + (p_0)
  the problem is to find x_k, k = {1,N} such as P(x_k) = 0
  The Laguerre method guarantees convergence to some root regardless the initial guess. In this sense, this code is based on calling the
  Lagurre method several times until the N roots are found.
  Since P(x) can be expressed in terms of its roots, P(x) = (x-x_N)·(x-x_N-1)· ... · (x-x_1), we split the products into sums by taking
  the natural logarithm of P(x), so
  ln(P(x)) = ln(|x - x_N|) + ln(|x - x_N-1|) + ... + ln(|x - x_1|)
  Now, G(x) and H(x) are the first and the second derivatives of ln(P(x)):
  G(x) = d(ln(P(x)))/dx = 1/(x-x_N) + 1/(x-x_N-1) + ... 1/(x-x_1)
  H(x) = d^2(ln(P(x)))/dx^2 = 1/(x-x_N)^2 + 1/(x-x_N-1)^2 + ... 1/(x-x_1)^2
  At this point, the Laguerre's method assumes that each root, x_k, is isolated from the others, which are bunched. The distance between
  x_k and the other roots is denoted as dx and this can be used for deriving the following formula for finding the root. This process is
  repeated until dx -> 0 which means that the algorithm converged to the root.
 
  The price of the algorithm simplicity is that Laguerre's method has more computational load than, e.g., Jenkins-Traub method which is
  the standard method for root solving in most commercial software packages.
  Notice that the code structure as well as the way the formulas were implemented was inspired by [2]
  References:
  [1] Numerical Methods that Work. Forman S. Acton. The mathematical association of America. 1990
  [2] Numerical Recipes in C. The art of scientific computing. Second Edition. William H. Press et al. Cambridge University Press. 1992
*/
class LaguerreMethod
{
public:
  LaguerreMethod(deque<complex<double>>);//Class constructor
  deque<complex<double>> solve_roots();

private:
  complex<double> laguerre_core(deque<complex<double>>, complex<double>);//Core function for root calculation
  deque<complex<double>> PolyCoeffs;// Polynomial coefficients
};

// P are the polynom coefficients
LaguerreMethod::LaguerreMethod(deque<complex<double>> P)
{
 PolyCoeffs = P;
}


// This function implements the Laguerre method (see [1], pages 187-188). Given a initial guess, this function finds
// a root of the given polynom. Since it finds only one root, it requires a driver routine (below) that removes the roots
// from the initial polynom, P(x), as they are found by the core.
// Notice that the code structure way inspired by [2]
complex<double> LaguerreMethod::laguerre_core(deque<complex<double>> P, complex<double> x)
{
  deque<double> frac={0,0.5, 0.25, 0.75, 0.13, 0.38, 0.62, 0.88, 1.};//Fractional steps for moving the root guess
  complex<double> m (P.size(),0);
  complex<double> dx, x1, g, gp, gm, h, b, d, f;
  double err;
  for (unsigned int it = 1; it <= MAX_IT; it++)
  {
    b = P[m.real()];
    err = abs(b);
    d = complex<double>(0,0);
    f = complex<double>(0,0);
    for ( int j=m.real()-1; j>=0; j--)
    {
      f = x*f + d;//Second derivative
      d = x*d + b;//First derivative
      b = x*b + P[j];//Polynom evaluation
      err = abs(b) + abs(x)*err;//Error term
    }
    err *= EPSS;
    if (abs(b) < err) return x;//x is already a root
     
    //Laguerre method
    g = d/b;// G(x_k) = P'(x_k) / P(x_k). ([1], eq. 7.10)
    h = g*g - 2.*f/b;// H(x_k) = G(x_k)^2 - (P''(x_k)/P(x_k)). ([1], eq. 7.11)
    gp = g + sqrt((m-1.)*(m*h-g*g));
    gm = g - sqrt((m-1.)*(m*h-g*g));
    if (abs(gp) < abs(gm)) gp = gm;
    if (max(abs(gp), abs(gm)) > 0.) dx = m/gp;//([1], eq. 7.12)
    else dx = (1.+abs(x))*complex<double>(cos(1.*it), sin(1.*it));
    x1 = x - dx;
    //This procedure is reapeated until dx -> 0 or the maximum number of iterations is exceeded
    if ((x.real() == x1.real()) && (x.imag() == x1.imag())) return x;
    if (it % MT) x = x1;
    else x = x - complex<double>(frac[it/MT],0)*dx;
  }
  return x;
}


// Driver routine for the Laguerre method
deque<complex<double>> LaguerreMethod::solve_roots()
{
 deque<complex<double>> temp_poly, roots;
 complex<double> x(0,0), b, c;

 temp_poly = PolyCoeffs;

 for (int j = PolyCoeffs.size()-1; j >= 1; j--)
 {
   x=complex<double>(0,0);
   x = laguerre_core(temp_poly, x);
   if (abs(x.imag()) < 2.0*EPS*abs(x.real())) x=complex<double>(x.real(),0);//Pure real
   roots.push_back(x);//Every time laguerre_core() is called, it retrieves a root, no matter where the initial guess is

   //Before finding the next root, it is necessary to remove the current root from the polynomial and i.e. the coefficients at
   // each iteration will differ from P(x)
   b = temp_poly[j];
   for (int jj = j-1; jj>=0; jj--)
   {
     c = temp_poly[jj];
     temp_poly[jj] = b;
     b = x*b + c;
   }
   temp_poly.erase(temp_poly.begin());
 }
  //Root polishing. Laguerre's method is used again using every root as starting point. This should refine the previous result
  for (int j=1; j <= PolyCoeffs.size()-1; j++) roots[j] = laguerre_core(PolyCoeffs, roots[j]);
  return roots;
}







