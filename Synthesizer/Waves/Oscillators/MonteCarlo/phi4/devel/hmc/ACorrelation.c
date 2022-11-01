/***********************************************************************************************
 * File ACorrelation.c
 * 
 * It prints the values of the autocorrelation function gamma[m] in which m is the Markov time. 
 * The value of Dbin is choosen as 10xM such that gamma[M]<0.3. 
 *   
 ***********************************************************************************************/

#define CONTROL
#include "phi4.h"
#include "string.h"
#include "functions.h"


/*
 *  data structures to store all the parameters of the algorithm,
 *  and action defined in phi4.h
 *  seed for initializing the random numbers
 */

static hmc_params_t hmc_params;
static act_params_t act_params;
static int seed;

static int Nstep, Ntraj, tau, Ntherm;
static double kappa, lambda;

/* It computes the action S[phi] for the global field phi in
*  lattice.h and the parameters kappa and lambda from act_params.
*  S = Sum_x [ -2*kappa*sum_mu phi_x phi_{x+mu}+phi_x^2+lambda(phi_x^2-1)^2 ].*/
double action(void)
{
    int i,mu;
    double phin,S,phi2;
    double kappa =act_params.kappa;
    double lambda=act_params.lambda;

    S=0;

    /* loop over all sites */
    for (i=0;i<V;i++)
    {
	/*sum over neighbors in positive direction*/
	phin=0;
	for (mu=0;mu<D;mu++) phin+=phi[hop[i][mu]];

	phi2=phi[i]*phi[i];
	S+=-2*kappa*phin*phi[i]+phi2+lambda*(phi2-1.0)*(phi2-1.0);
    }

    return S;
}

/* Routine which reads one line from the input file.
*  Format of the lines is <keyword> <value>.
*  Checks if the keyword in string str matches,
*  then gets the value according to the format in fmt.*/
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

/* Parses the input file (format as specified in get_val)
*  and prints the parameters onto the screen. Currently
*  it reads the basic values for the action and also for the
*  future HMC and the seed of the random number generator.*/
static int read_input(char *input)
{
    FILE* fp;

    fp=fopen(input,"r");
    if (fp==NULL) {
	fprintf(stderr, "Cannot open input file %s \n",input);
	exit(1);
    }

    get_val(fp, "kappa",       "%lf",&act_params.kappa  );
    get_val(fp, "lambda",      "%lf",&act_params.lambda );
    get_val(fp, "ntherm",      "%i" ,&hmc_params.ntherm );
    get_val(fp, "ntraj",       "%i" ,&hmc_params.ntraj  );
    get_val(fp, "traj_length", "%i",&hmc_params.tlength);
    get_val(fp, "nstep",       "%i" ,&hmc_params.nstep  );
    get_val(fp, "seed",        "%i" ,&seed   );

    Nstep=hmc_params.nstep;
    Ntraj=hmc_params.ntraj;
    tau=hmc_params.tlength;
    Ntherm=hmc_params.ntherm;
    kappa=act_params.kappa;
    lambda=act_params.lambda;

    printf("PARAMETERS\n");
    printf("L              %i\n", L);
    printf("DIM            %i\n", D);
    printf("kappa          %f\n", act_params.kappa);
    printf("lambda         %f\n", act_params.lambda);
    printf("ntherm         %i\n", hmc_params.ntherm);
    printf("ntraj          %i\n", hmc_params.ntraj);
    printf("traj_length    %i\n", hmc_params.tlength);
    printf("nstep          %i\n", hmc_params.nstep);
    printf("END PARAMETERS\n");

    return 0;
}


int main(int argc, char* argv[])
{
    int i, j, m, T=0;                          /* Indices. */
    double *M, Mmean=0.0, Mmean2=0.0, gamma;   /* M[i] = <m^2>/V^2 as a fuction of the Markov time, Mmean is the mean of M[i] and Mmean2 is the mean of M[i]^2. */
    char Auto[100], time[100];                 /* Data file names. */
    FILE *fAuto, *Atime;                       /* Data files. */

    if (argc != 2) 
    {
        fprintf(stderr, "Number of arguments not correct\n");
        fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
        exit(1);
    }

    /* Get the parameters from the input file. */
    read_input(argv[1]);

    /* In the data files names the parameters of the computation are specified. */
    sprintf(Auto, "Data/ACorrelation/Auto_w_%d_%d_%d_%d_%d_%f",L,Nstep,tau,Ntraj,Ntherm,lambda);
    /*sprintf(time, "Data/ACorrelation/TimeK_%d_%d_%d_%d_%d_%f",L,Nstep,tau,Ntraj,Ntherm,lambda);*/
    /*sprintf(time, "Data/ACorrelation/TimeL_%d_%d_%d_%d_%f_%f",Nstep,tau,Ntraj,Ntherm,lambda,kappa);*/
    sprintf(time, "Data/ACorrelation/TimeTau_%d_%d_%d_%d_%f_%f",L,tau,Ntraj,Ntherm,lambda,kappa);

    fAuto=fopen(Auto,"w+");
    Atime=fopen(time,"a");

    /* Initialize random number generator. */
    rlxd_init(1,seed);

    /* Initialize the nearest neighbor field. */
    hopping(hop);

    /* Initialize phi field and save it. */
    ranlxd(phi,V);

    /* Thermalization. */
    Termalization(Nstep,tau,kappa,lambda,Ntherm);

    /****************** AUTOCORRELATION CHECK ***************/
    
    /* Dynamic allocation of memory for M. */
    M=malloc(Ntraj*sizeof(double));
    if (M==NULL){printf("Dynamically allocation of memory failed for M\n");}

    /* Computation of the correlator M[i]=Sum_x m(x), the mean Mmean=<M> and the mean of the square Mmean2= <M^2>. */
    for(i=0; i<Ntraj; i++)
	{
        M[i]=0.0;
        MonteCarlo(Nstep,tau,kappa,lambda);       

        for(j=0; j<V; j++)
        {
            m+=phi[j];
        }

        M[i]+= m*m/((double)V*(double)V);
        m=0.0;
		
		Mmean+=M[i]/Ntraj;
        Mmean2+=(M[i]*M[i])/Ntraj;
	}

    
    /* Computation of the autocorrelation. */
    for(m=0; m<500; m++)
    {
        for(i=0; i<Ntraj; i++)
        {
            gamma+=(M[i]*M[(i+m)%(int)Ntraj]-Mmean*Mmean)/((Mmean2-Mmean*Mmean)*Ntraj);
        }

        fprintf(fAuto,"%d\t%e\n",m,gamma);
        if(T==0)
        {
            if(gamma<0.3)
            {
                T=m;
            }
        }
        gamma=0;
    }

    /*fprintf(Atime,"%f\t%d\n",kappa,T);*/
    /*fprintf(Atime,"%d\t%d\n",L,T);*/
    fprintf(Atime,"%d\t%d\n",Nstep,T);
    fclose(Atime);
    fclose(fAuto); 
    free(M);
    return 0;
}
