
/*******************************************************************************
* File mainBin.c
*
* It computes the correlator <x(t)x(0)> with the binned data and the 
* corresponding autocorrelation function "gamma".
*  
*******************************************************************************/
#define MAIN_PROGRAM

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "global.h"
#include "start.h"
#include "random.h"
#include "functions.h"
#include "mainAction.h"


static params in;
static int N;
static double M,W;

/*It copies strings from a file to a variable.*/
static int get_val(FILE* fp, char *str, char* fmt,  void* val)
{
    char c[128];

    if(1!=fscanf(fp,"%s",c))
    {
      fprintf(stderr,"Error reading input file at %s\n",str);
      exit(1);
    }

    if(strcmp(str,c)!=0)
    {
      fprintf(stderr,"Error reading input file expected %s found %s\n",str,c);
      exit(1);
    }

    if(1!=fscanf(fp,fmt,val))
    {
      fprintf(stderr,"Error reading input file at %s\n",str);
      fprintf(stderr,"Cannot read value format %s\n",fmt);
      exit(1);
    }

    return 0;

}

/*It copies the parameters values form the infile writing them on the data structure params,
* finally it prints on the terminal such values.*/
static int read_input(char *input)
{
    FILE* fp;

    fp=fopen(input,"r");
    if (fp==NULL)
    {
      fprintf(stderr, "Cannot open input file %s \n",input);
      exit(1);
    }

    get_val(fp, "a",         "%lf" ,&in.a );
    get_val(fp, "ThermalT",  "%i" ,&in.ThermalT  );
    get_val(fp, "Nsweep",    "%i",&in.Nsweep );
    get_val(fp, "Dbin",      "%i" ,&in.Dbin  );
    get_val(fp, "Lambda",    "%lf" ,&in.L  );

    /*The physical mass and the frequency are fixed to 1, M and W are effective parameters
    * which carry the dependence of the action on the lattice space. The physical time interval is fixed 
    * T=aN=64*/
    N=(int)64/in.a;
    M=1/in.a;
    W=in.a;
  
    printf("PARAMETERS\n");
    printf("a              %f\n", in.a);
    printf("N              %i\n", N);
    printf("M              %f\n", M);
    printf("W              %f\n", W);
    printf("ThermalT       %i\n", in.ThermalT);
    printf("Nsweep         %i\n", in.Nsweep);
    printf("Dbin           %i\n", in.Dbin);
    printf("Lambda         %lf\n", in.L);
    printf("END PARAMETERS\n");

    return 0;
}

/*It allocates dynamically the memory for the global variable xx[N],
* this because "a" is fixed through the infile.*/
void f(unsigned int size)
{
    if(!xx) 
    {
        xx = malloc(size);
        if (xx == NULL) {printf("Non ho abbastanza memoria per l'allocazione xx\n");exit(1); } 
    }
}

/*MAIN*/
int main(int argc, char* argv[])
{
  int i, j, t, m;                         /*Indices.*/
  int B;                                  /*B=Nsweep/Dbin.*/
  double C_time=0, C_time2=0;             /*C_time is the average binned correlator at t=1. C_time2 is the average binned squared correlator at t=1.*/
  double gamma=0;                         /*Average autocorrelation function.*/
  double *C, *C_err, sum=0.0;             /*c is the average binned correlator at each time and C_err its error. sum needs to compute C_err.*/
  double **c, *pc;                        /*pc needs to initialize c which is the correlator matrix c(i,t).*/
  char correlator_values[70], autocorrelation[70];  /*They are variables for the names of the data files*/
  FILE *fdataC, *fdataA;                  /*Data files.*/

  /*It makes sure that an infile is used.*/
  if (argc != 2) 
  {
    fprintf(stderr, "Number of arguments not correct\n");
    fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
    exit(1);
  }

  /*It gets the parameters from the infile.*/
  read_input(argv[1]);
  
  /*In the data files names the parameters of the computation are specified.*/
  sprintf(correlator_values, "Data/mainBin/CorMBIN_%d_%f_%f_%d_%d_%d",N,M,W,in.ThermalT,in.Nsweep,in.Dbin);
  sprintf(autocorrelation, "Data/mainBin/AutoBIN_%d_%f_%f_%d_%d_%d",N,M,W,in.ThermalT,in.Nsweep,in.Dbin);
  fdataC=fopen(correlator_values,"wb");
  fdataA=fopen(autocorrelation,"w+");

  /*Memory allocation for xx[N] and initialization of B.*/
  f(N * sizeof(double));
  B=in.Nsweep/in.Dbin;

  /*Initialization of the random numebers generator and thermalization.*/
  rlxs_init(0,5234);
  Term(N,M,W,in.ThermalT,in.L);
  
  /*Memory allocation for C, C_err, pc and c. This time c is initializated with "B" because there is not a large amount of RAM required.*/
  C=calloc(N,sizeof(double));
  C_err=calloc(N,sizeof(double));
  pc=malloc((B)*N*sizeof(double));
  c=malloc((B)*sizeof(double*));
  if(C == NULL) {printf("Non ho abbastanza memoria per l'allocazione C\n"); exit(1); } 
  if(C_err == NULL) {printf("Non ho abbastanza memoria per l'allocazione C_err\n"); exit(1); } 
  if(pc == NULL) {printf("Non ho abbastanza memoria per l'allocazione pc\n"); exit(1); } 
  if(c == NULL) {printf("Non ho abbastanza memoria per l'allocazione c\n"); exit(1); } 
  for(i=0; i<B; i++)
  {
    c[i]=pc+N*i;
  }

  printf("Binned correlator computation started...\n");

  /*It computes the binned correlator*/
  for(i=0; i<B; i++)     
  {
    for(j=0; j<in.Dbin; j++)
    {
	    for(t=0; t<N; t++)
      {
        c[i][t]=c[i][t] + (Correlator(t,N)/(double)in.Dbin);
      }

      /*Metropolis*/
	    AggCatena(N,M,W,in.L);
	  }

    /*Percentuale di completamento
      if(i==B/4)
      printf("25/100 done...\n");
      if(i==B/2)
      printf("50/100 done...\n");
      if(i==(B/4)*3)
      printf("75/100 done...\n");
      if(i==B-1)
      printf("100/100 done...\n");*/
  }

  /*Average binned correlator computation.*/
  for(t=0; t<N; t++)    
  {
    for(i=0; i<B; i++)
    {
      C[t]+=c[i][t]/B;
    }
  }

  /*It prints the average correlator with its error.*/
  for(t=0; t<N; t++)    
  {
    for(i=0; i<B; i++)
    {
      sum+=(c[i][t]-C[t])*(c[i][t]-C[t]);
  	  C_err[t] =sqrt( sum/( B*(B-1) ) );
    }
    fprintf(fdataC,"%d\t%e\t%e\n",t,C[t],C_err[t]);
    sum=0.0;
  }

  printf("Binned autocorrelation computation started...\n");

  /*Average correlation and squared correlator at t=1 computation.*/
  for(i=0; i<B; i++)   
  {
    C_time+=c[i][1]/(double)B;
    C_time2+=(c[i][1]*c[i][1])/(double)B;
  }

  /*Binned autocorrelation computation.*/
  for(m=0; m<300; m++)   
  {
    for(i=0; i<B; i++)
    {
      gamma+=(c[i][1]*c[(i+m)%B][1]-C_time*C_time)/((C_time2-C_time*C_time)*(double)B);
    }
    
    fprintf(fdataA,"%d\t%e\n",m,gamma);
    gamma=0;
  }

  printf("Computation completed!\n");

  fclose(fdataC);
  fclose(fdataA);
  free(pc);
  free(c);
  free(C);
  free(xx);

  return 0;
}
