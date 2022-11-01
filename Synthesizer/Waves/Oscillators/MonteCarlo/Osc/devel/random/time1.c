
/*******************************************************************************
*
* File time1.c
*
* Measurement of the processor time required to produce single-precision
* random numbers using ranlxs
*
* Author: Martin Luescher <luscher@mail.cern.ch>
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "random.h"

#define N 100
#define NLOOPS 100000


int main()
{
   int k,level;
   float t1,t2,dt;
   float r[N];

   printf("\n");
   printf("Timing of ranlxs (average time per random number in microsec)\n");
   printf("\n");

   for (level=0;level<=2;level++)
   {
      rlxs_init(level,1);

      t1=(float)clock();
      for (k=1;k<=NLOOPS;k++) 
         ranlxs(r,N);
      t2=(float)clock();
      
      dt=(t2-t1)/(float)(CLOCKS_PER_SEC);
      dt*=1.0e6f/(float)(N*NLOOPS);

      printf("%4.3f (level %1d)  ",dt,level);
   }

   printf("\n\n");
   exit(0);
}


