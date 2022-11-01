
/*******************************************************************************
*
* File gauss.c
* 
* Generation of Gaussian random numbers
*
* The externally accessible functions are
*
*   void gauss(float r[],int n)
*     Generates n single-precision Gaussian random numbers x with distribution
*     proportional to exp(-x^2) and assigns them to r[0],..,r[n-1]
*
*   void gauss_dble(double rd[],int n)
*     Generates n double-precision Gaussian random numbers x with distribution
*     proportional to exp(-x^2) and assigns them to rd[0],..,rd[n-1]
*
* Version 1.0
* Author: Martin Luescher <luscher@mail.cern.ch>
*
*******************************************************************************/

#define GAUSS_C

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "random.h"

#define PI 3.141592653589793


void gauss(float r[],int n)
{
   int k;
   float u[2];
   double x1,x2,rho,y1,y2;

   for (k=0;k<n;)
   {
      ranlxs(u,2);
      x1=(double)u[0];
      x2=(double)u[1];

      rho=-log(1.0-x1);
      rho=sqrt(rho);
      x2*=2.0*PI;
      y1=rho*sin(x2);
      y2=rho*cos(x2);
      
      r[k++]=(float)y1;
      if (k<n)
         r[k++]=(float)y2;
   }
}


void gauss_dble(double rd[],int n)
{
   int k;
   double ud[2];
   double x1,x2,rho,y1,y2;

   for (k=0;k<n;)
   {
      ranlxd(ud,2);
      x1=ud[0];
      x2=ud[1];

      rho=-log(1.0-x1);
      rho=sqrt(rho);
      x2*=2.0*PI;
      y1=rho*sin(x2);
      y2=rho*cos(x2);
      
      rd[k++]=y1;
      if (k<n)
         rd[k++]=y2;
   }
}

