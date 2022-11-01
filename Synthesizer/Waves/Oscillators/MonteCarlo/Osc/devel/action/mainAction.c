
/*******************************************************************************
* File mainAction.c
* It computes the action for two different initial configurations: 
* the Cold configuration (xx[i]=0 for all i), 
* and the Hot configuration (xx[i]=10*(random[i]-0.5)).
* 
* Comparing their values it's possible to know if the thermalization time (ThermalT=500)
* is big enough to reach the same values of the action.
*********************************************************************************/
#define MAIN_PROGRAM

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "string.h"
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
  }
}


/*MAIN*/
int main(int argc, char* argv[])
{
  int i,j;                                /*Indices.*/
  double A, DS, S;                        /*A is the variable which adds to xx[i] for the proposal of the new coordinate xx[i]'=xx[i]+A, S is the action, DS the variation of the action.*/
  float *r;                               /*r is a vector of 2*N random numbers which needs for both the proposal of the new coordinate and the accept/reject step.*/
  char hot_action[70], cold_action[70];   /*They are variables for the names of the data files*/
  FILE *dataCold, *dataHot, *dataxx;      /*Data files. dataxx contains the positions xx[i], just for fun.*/
  

  /*It makes sure that an infile is used.*/
  if (argc != 2) 
  {
    fprintf(stderr, "Number of arguments not correct\n");
    fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
    exit(1);
  }

  /*It gets the parameters from the infile.*/
  read_input(argv[1]);

  /*Memory allocation for xx[N] and r[2*N]*/
  f(N * sizeof(double));
  r=malloc(2*N * sizeof(float));

  /*In the data files names the parameters of the computation are specified.*/
  sprintf(hot_action, "Data/mainAction/AHot_%d_%f_%f_%d_%d",N,M,W,in.ThermalT,in.Nsweep);
  sprintf(cold_action, "Data/mainAction/ACold_%d_%f_%f_%d_%d",N,M,W,in.ThermalT,in.Nsweep);
  dataHot=fopen(hot_action,"w+");
  dataCold=fopen(cold_action,"w+");
  dataxx=fopen("Data/Positions","w+");
  
  /*Initialization of the random numebers generator.*/
  rlxs_init(0,5234);
  
  /******* Initialization of the COLD configuration. ******/
  for(i=0; i<N;i++)
  {
    xx[i]=0;
  }

  /*Cold sweep: for each i, first a new path is proposed and can be acepted through the Metropolis algorithm, then the action is printed.*/
  for(i=0; i< in.ThermalT; i++)
  {
    ranlxs(r,2*N); 

    /*Metropolis: for each physical time j a new position is randomly proposed, if the variation of the action is quite little it is acepted with a certain problability.*/
    for(j=0; j<N; j++)
    {
      A=(double)in.L*(r[j+N]-0.5);
      DS=DeltaS(j,A,N,M,W);

      if(exp(-1*DS) > r[j])
      {
        xx[j]=xx[j]+A;
      }
      
    }

    S=Action(N,M,W);
    fprintf(dataCold,"%d \t %e\n",i,S);
  }

  
  /*The action have to be initializate to zero before to start with the Hot configuration.*/
  S=0;

  /******** Initialization of the HOT configuration. ********/
  for(i=0; i<N; i++)
  { 
    xx[i]=10*(r[i]-0.5);
  }
  
  /*Hot sweep*/
  for(i=0; i< in.ThermalT; i++)
  {
    ranlxs(r,2*N);
    for(j=0; j<N; j++)
    {
      A=(double)in.L*(r[j+N]-0.5);
      DS=DeltaS(j,A,N,M,W);

      if(exp(-1*DS) > r[j])
      xx[j]=xx[j]+A;
    }

    S=Action(N,M,W);
    fprintf(dataHot,"%d \t %e\n",i,S);
  }

  /*It prints xx[i].*/
  for(i=0; i<N;i++)
  {
   fprintf(dataxx,"%d\t%f\n",i,xx[i]);
  }

  fclose(dataxx);
  fclose(dataCold);
  fclose(dataHot);

  return 0;
}
