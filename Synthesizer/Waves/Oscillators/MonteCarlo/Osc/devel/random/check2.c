
/*******************************************************************************
*
* File check2.c
*
* Writes the state of ranlxs to a file together with the next 147 random
* numbers. Then reads the data back in and checks the correct reinitialization
* of the generator
*
* Author: Martin Luescher <luscher@mail.cern.ch>
*
*******************************************************************************/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "random.h"

#define N 147


static void write_state(void)
{
   FILE *fp;
   int k,ns,*state;
   float base,r[N];

   ns=rlxs_size();
   state=malloc(ns*sizeof(int));
   base=(float)(ldexp(1.0,24));
   rlxs_init(1,1234567);

   for (k=0;k<10;k++) 
      ranlxs(r,N);

   rlxs_get(state);
   ranlxs(r,N);

   fp=fopen(".tmp","w");

   for (k=0;k<ns;k++)
      fprintf(fp,"%d\n",state[k]);

   for (k=0;k<N;k++)
      fprintf(fp,"%12.1f\n",base*r[k]);

   fclose(fp);
}


static void read_state(void)
{
   FILE *fp;
   int k,ns,*state;
   float base,r[N],r_old[N];

   ns=rlxs_size();
   state=malloc(ns*sizeof(int));   
   base=(float)(ldexp(1.0,24));

   fp=fopen(".tmp","r");

   for (k=0;k<ns;k++)
      fscanf(fp,"%d",&state[k]);

   for (k=0;k<N;k++)
      fscanf(fp,"%f",&r_old[k]);

   fclose(fp);
   remove(".tmp");
   
   rlxs_reset(state);
   ranlxs(r,N);
   
   for (k=0;k<N;k++)
   {
      if (r_old[k]!=(base*r[k]))
      {
         printf("\n");
         printf("Error: state of ranlxs has not been properly reset\n");
         printf("\n");
         exit(1);
      }
   }
   

}


int main(void)
{
   write_state();
   read_state();
   
   printf("\n");
   printf("State properly reset, generated random numbers are correct\n\n");
   exit(0);   
}
