
/*******************************************************************************
* File mainEnergy.c
*
* It computes the binned correlator and then it builds the Jackknife cluster.
* Finally, it computes the energy gap DeltaE*a=(E1-E0)*a and the matrix element <E0|x|E1> with thier errors.
*
*******************************************************************************/
#define MAIN_PROGRAM

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "start.h"
#include "random.h"
#include "global.h"
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



int main(int argc, char* argv[])
{
  int i, j, t;                                  /*Indices.*/
  int B;                                        /*B=Nsweep/Dbin.*/
  double *C_time, *Energy, *DeltaE;             /*C_time is the average correlator at each time, Energy and ElementM are the energy gap and the matrix element for each time.*/
  double *ElementM, *DeltaM;                    /*DeltaE and DeltaM are required to compute the errors.*/ 
  double **c, *pc;                              /*pc needs to initialize c which is the correlator matrix c(i,t).*/
  double Error=0, ErrorM=0;                     /*Error is the error of the energy gap and ErrorM that one of the matrix element.*/
  char mean_energy[80], matrix_elements[80];    /*They are variables for the names of the data files.*/
  FILE *fdataE, *fdataM;                        /*Data files.*/

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
  sprintf(mean_energy, "Data/MeanEnergyDS_%d_%f_%f_%d_%d_%d",N,M,W,in.ThermalT,in.Nsweep,in.Dbin);
  sprintf(matrix_elements, "Data/MatrixElementsDS_%d_%f_%f_%d_%d_%d",N,M,W,in.ThermalT,in.Nsweep,in.Dbin);
  fdataE=fopen(mean_energy,"w+");
  fdataM=fopen(matrix_elements,"w+");

  /*Memory allocation for xx[N] and initialization of B.*/
  f(N * sizeof(double));
  B=in.Nsweep/in.Dbin;
 
  /*Initialization of the random numebers generator and thermalization.*/
  rlxs_init(0,1543);
  Term(N,M,W,in.ThermalT,in.L);

  /*Memory allocation for C_time, Energy, ElementM, DeltaE, DeltaM, pc and c. This time c is initializated with "B" because there is not a large amount of RAM required.*/
  C_time=calloc(N,sizeof(double));
  if(C_time == NULL) {printf("Non ho abbastanza memoria per l'allocazione C_time\n"); exit(1); } 
  Energy=calloc(N,sizeof(double));
  if(Energy == NULL) {printf("Non ho abbastanza memoria per l'allocazione Energy\n"); exit(1); } 
  ElementM=calloc(N,sizeof(double));
  if(ElementM == NULL) {printf("Non ho abbastanza memoria per l'allocazione ElementM\n"); exit(1); } 
  DeltaE=calloc(N,sizeof(double));
  if(DeltaE == NULL) {printf("Non ho abbastanza memoria per l'allocazione DeltaE\n"); exit(1); } 
  DeltaM=calloc(N,sizeof(double));
  if(DeltaM == NULL) {printf("Non ho abbastanza memoria per l'allocazione DeltaM\n"); exit(1); } 
  pc=malloc((B)*N*sizeof(double));
  if(pc == NULL) {printf("Non ho abbastanza memoria per l'allocazione pc\n"); exit(1); } 
  c=malloc((B)*sizeof(double*));
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
        c[i][t] += (Correlator(t,N)/(double)in.Dbin);
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
  
  printf("Average binned correlator computation started...\n");

  /*Average binned correlator computation.*/
  for(i=0; i<B; i++)
  {
    for(t=0; t<N; t++)
    {
      C_time[t] += c[i][t]/((double)B);
    }
  }

  printf("Creating clusters Jackknife...\n");

  /*It builds the Jackknife cluster for the correlator.*/
  for(i=0; i<B; i++)
  {
    for(t=0; t<N; t++)
    {
      c[i][t]=C_time[t]-(c[i][t]-C_time[t])/((double)(B-1));
    }
  }

  printf("Computing Energy and matrix elements...\n");
  
  /*It computes (E_1-E_0)*a.*/
  for(t=0; t<N; t++)
  {
    Energy[t] = D_energy(C_time[(t+1)%N],C_time[(t-1+N)%N],C_time[t]);
  }

  /*It computes the matrix element.*/
  for(t=0; t<N; t++)
  {
    ElementM[t]=sqrt((C_time[t]*exp(N*0.5*Energy[t]))/(2*cosh((N*0.5-t)*Energy[t])));
  }

  printf("Computing variances...\n");

  /*It computes the variances.*/
  for(i=0; i<B; i++)
  {
    for(t=0; t<N; t++)
    {
      DeltaE[t]+= ( D_energy(c[i][(t+1)%N],c[i][(t-1+N)%N],c[i][t]) - Energy[t] ) * ( D_energy(c[i][(t+1)%N],c[i][(t-1+N)%N],c[i][t]) - Energy[t] );

      DeltaM[t]+=(sqrt((c[i][t]*exp(N*0.5* D_energy(c[i][(t+1)%N],c[i][(t-1+N)%N],c[i][t])))/(2*cosh((N*0.5-t)* D_energy(c[i][(t+1)%N],c[i][(t-1+N)%N],c[i][t]))))-ElementM[t])*(sqrt((c[i][t]*exp(N*0.5* D_energy(c[i][(t+1)%N],c[i][(t-1+N)%N],c[i][t])))/(2*cosh((N*0.5-t)* D_energy(c[i][(t+1)%N],c[i][(t-1+N)%N],c[i][t]))))-ElementM[t]);
    }
  }

  /*It prints the energy gap and the matrix element with their errors.*/
  for(t=0; t<N; t++)
  {
    Error=sqrt(((double)(B-1)/(B))*DeltaE[t]);
    fprintf(fdataE,"%e\t%e\t\n",Energy[t],Error);

    ErrorM=sqrt(((double)(B-1)/(B))*DeltaM[t]);
    fprintf(fdataM,"%e\t%e\t\n",ElementM[t],ErrorM);
  }

  printf("Computation completed!\n");

  fclose(fdataE);
  fclose(fdataM);
  free(C_time);
  free(Energy);
  free(ElementM);
  free(DeltaM);
  free(pc);
  free(c);

 return 0;
}
