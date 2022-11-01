
/*******************************************************************************
* File mainCor.c
* It computes the correlator <x(t)x(0)> and the corresponding autocorrelation function "gamma".
* 
* From "gamma" it's possible to fix the lenght of the bin Dbin.
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
* this because a is fixed through the infile.*/
void f(unsigned int size)
{
    if(!xx) 
    {
        xx = malloc(size);
        if (xx == NULL) {printf("Non ho abbastanza memoria per l'allocazione xx\n");exit(1); } 
    }
}


int main(int argc, char* argv[])
{
  int i, j, t, m;                     /*Indices.*/
  int n;                              /*Reduced number of sweep: n=Nsweep/100. This is required to reduce the use of RAM.*/
  double *C, *Cor;                    /*C(t) is the average correlator at each time. Cor(i) is the correlator at t=1 for each sweep.*/
  double C_time=0, C_time2=0;         /*C_time is the average correlator at t=1. C_time2 is the average squared correlator at t=1.*/
  double gamma=0;                     /*Average autocorrelation function.*/
  double *pc, **c;                    /*pc needs to initialize c which is the correlator matrix c(i,t).*/
  int T_M=0;                          /*Markov time at which gamma becomes smaller than 0.3.*/
  char correlator_mean_values[100], autocorrelation[100], lambda[100]; /*They are variables for the names of the data files*/
  FILE *fdataS, *fdataA, *flambda;    /*Data files*/
  

  /*It makes sure that an infile is used.*/
  if (argc != 2) 
  {
    fprintf(stderr, "Number of arguments not correct\n");
    fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
    exit(1);
  }

  /*It gets the parameters from the input file */
  read_input(argv[1]);

  /*In the data files names the parameters of the computation are specified.*/
  sprintf(correlator_mean_values, "Data/mainCor/CorM_%d_%f_%f_%d_%d",N,M,W,in.ThermalT,in.Nsweep);
  sprintf(autocorrelation, "Data/mainCor/Auto_%d_%f_%f_%d_%d",N,M,W,in.ThermalT,in.Nsweep);
  sprintf(lambda, "Data/mainCor/Lambda_%d_%d",N,in.Nsweep);
  fdataS=fopen(correlator_mean_values,"w+");
  fdataA=fopen(autocorrelation,"w+");
  flambda=fopen(lambda,"a");

  /*Memory allocation for xx[N], C and Cor.*/
  f(N * sizeof(double));
  C=calloc(N,sizeof(double));
  Cor=malloc(in.Nsweep*sizeof(double));
  if(C == NULL) {printf("Non ho abbastanza memoria per l'allocazione C\n"); exit(1); } 
  if(Cor == NULL) {printf("Non ho abbastanza memoria per l'allocazione Cor\n"); exit(1); } 

  /*Initialization of the random numebers generator and thermalization.*/
  rlxs_init(0,5234);
  Term(N,M,W,in.ThermalT,in.L);

  /*Initialization of the reduced variable.*/
  n=in.Nsweep/100;

  /*Starting point of the loop.*/
  for(j=0;j<100;j++)
  {  
    
    /*It initialize pc and c with "n" not "Nsweep".*/
    pc = malloc((size_t)n*N*sizeof(double));
    c = malloc(n*sizeof(double*));
    if (pc == NULL) {printf("Non ho abbastanza memoria per l'allocazione pc\n");exit(1); } 
    if (c == NULL) {printf("Non ho abbastanza memoria per l'allocazione c\n");exit(1); } 
    for(i=0; i<n; i++)
    {
      c[i]=pc+N*i;
    }

    printf("N= %d j= %d : Correlator computation started...\n",N,j);

    /*Correlator computation.*/
    for(i=0; i<n; i++)
    {
      /*Metropolis*/
      AggCatena(N,M,W,in.L);

      for(t=0; t<N; t++)
      {
        c[i][t]=Correlator(t,N);
      }
      
      /*Percentuale di completamento
      if(i==n/4)
      printf("N=%d j= %d : 25/100 done...\n",N,j);
      if(i==n/2)
      printf("N=%d j= %d : 50/100 done...\n",N,j);
      if(i==(n/4)*3)
      printf("N=%d j= %d : 75/100 done...\n",N,j);
      if(i==n-1)
      printf("N=%d j= %d : 100/100 done...\n",N,j);*/
    }

    printf("N=%d j= %d : Average correlator computation started...\n",N,j);

    /*Average correlation computation.*/
    for(t=0; t<N; t++)               
    {
      for(i=0; i<n; i++)
      {
        C[t]+= c[i][t]/(double)in.Nsweep;
      }  
  
     /*Percentuale di completamento
      if(i==N/4)
      printf("N=%d j= %d : 25/100 done...\n",N,j);
      if(t==N/2)
      printf("N=%d j= %d : 50/100 done...\n",N,j);
      if(t==(N/4)*3)
      printf("N=%d j= %d : 75/100 done...\n",N,j);
      if(t==N-1)
      printf("N=%d j= %d : 100/100 done...\n",N,j);*/
    }
   

    /*Average correlation and squared correlator at t=1 computation.*/
    for(i=0; i<n; i++)
    {
      C_time+=c[i][1]/(double)in.Nsweep;             
      C_time2+=c[i][1]*c[i][1]/(double)in.Nsweep;    

      Cor[i+n*j]=c[i][1];                         /*Correlator at t=1 for each sweep.*/
    }

    free(pc);
    free(c);
  }

  printf("N=%d : Autocorrelation computation started...\n",N);

  /*Autocorrelation and T_M computation. "m" fixed such that m >> T_M.*/
  for(m=0; m<3000; m++)
  {
    for(i=0; i<in.Nsweep; i++)
    {
      gamma+=(Cor[i]*Cor[(i+m)%in.Nsweep]-C_time*C_time)/((C_time2-C_time*C_time)*(double)in.Nsweep);
    }
    
    fprintf(fdataA,"%d\t%e\n",m,gamma);
    
    if(T_M==0 && gamma < 0.3)
    {
      T_M=m;
      fprintf(flambda,"%f\t%d\n",in.L,T_M);
    }
    gamma=0;
  }   

  printf("N=%d : Autocorrelation computation completed!\n",N);

  /*It prints the average correlator for each time.*/
  for(t=0;t<N;t++)
  {
    fprintf(fdataS,"%d\t%e\n",t,C[t]);
  }

  printf("Computation completed!\n");

  fclose(fdataA); 
  fclose(fdataS);
  fclose(flambda);
  free(xx);
  free(C);
  free(Cor);
  
  return 0;
}