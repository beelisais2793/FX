/**************************************************************************************************************
 * File DeltaHCheck.c
 * 
 * It prints the average, over the Markov time, of abs(Delta_H) in which Delta_H is the difference of energy
 * between the initial field configuration and the new one proposed through the MD equations. 
 * Such value quantify the violation of energy consevation produced by the Leap Frog, with and without AR.
 * It prints the expectation value <e^(-Delta_H)>, with and without AR, the closer it is to 1 the better it is.    
 *
 **************************************************************************************************************/

#define CONTROL
#include "phi4.h"
#include "string.h"
#include "functions.h"

/* Data structures to store all the parameters of the algorithm,
*  and action defined in phi4.h
*  seed for initializing the random numbers.*/
static hmc_params_t hmc_params;
static act_params_t act_params;
static int seed;

/*Auxiliary variables.*/
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
    int i,j;                                                        /* Indices.*/
    double act_0, act_1, ham_0, ham_1;                              /* Variables for the action and the hamiltonian.*/
    double DHmean=0.0, Exp=0.0, sum=0.0, Err, dt;                   /* Mean values of Delta_H and e^(-Delta_H), two useful variables for the means and errors. dt is delta_tau^2.*/   
    double *DeltaH;                                                 /* Values of Delta_H at each sweep.*/
    char DHw[100], DHwo[100], Expw[100], Expwo[100];                /* Data files names.*/
    FILE *fDHcheck_wo, *fDHcheck_w, *fExpcheck_wo, *fExpcheck_w;    /* Data files.*/
   
    /* It makes sure that an infile is used. */
    if (argc != 2) 
    {
        fprintf(stderr, "Number of arguments not correct\n");
        fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
        exit(1);
    }

    /* It gets the parameters from the input file. Intialization of delta_tau^2. */
    read_input(argv[1]);
    dt=1/((double)Nstep*Nstep);

    /* In the data files names the parameters of the computation are specified.
    *  Nstep misses since the idea is to run many times this program to cath the behavior of the ossevable with Nstep. */
    sprintf(DHw, "Data/DeltaHCheck/DHcheck_w_%d_%d_%d_%d_%f_%f",L,tau,Ntraj,Ntherm,kappa,lambda);
    sprintf(DHwo, "Data/DeltaHCheck/DHcheck_wo_%d_%d_%d_%d_%f_%f",L,tau,Ntraj,Ntherm,kappa,lambda);
    sprintf(Expw, "Data/DeltaHCheck/Expcheck_w_%d_%d_%d_%d_%f_%f",L,tau,Ntraj,Ntherm,kappa,lambda);
    sprintf(Expwo, "Data/DeltaHCheck/Expcheck_wo_%d_%d_%d_%d_%f_%f",L,tau,Ntraj,Ntherm,kappa,lambda);
    fDHcheck_wo=fopen(DHwo,"a");
    fDHcheck_w=fopen(DHw,"a");
    fExpcheck_wo=fopen(Expwo,"a");
    fExpcheck_w=fopen(Expw,"a");

    /*Dynamic allocation of memory for DeltaH.*/
    DeltaH=malloc((Ntraj/Dbin)*sizeof(double));
    if(DeltaH == NULL) {printf("Dynamically allocation of memory failed for DeltaH\n"); exit(1); } 

    /* Initialize random number generator. */
    rlxd_init(1,seed);

    /* Initialize the nearest neighbor field. */
    hopping(hop);

    /****************** DELATA H CHECK without Accept/Reject ***************/
    /* Initialize phi. */
    ranlxd(phi,V);

    /* Thermalization without A/R */
    for(j=0;j<Ntherm;j++)
	{
        GaussianMom();
        LeapFrog(Nstep,tau,kappa,lambda);
	}
    
    /* Computation of DeltaH[j]. */
    for(j=0;j<Ntraj/Dbin;j++)
    {
        DeltaH[j]=0.0;

	    for(i=0;i<Dbin;i++)
	    {
            GaussianMom();

            act_0=action();
            ham_0=Hamiltonian(act_0);

            LeapFrog(Nstep,tau,kappa,lambda);

            act_1=action();
            ham_1=Hamiltonian(act_1);

            DeltaH[j]+= (ham_1 - ham_0)/Dbin;

	    }

        DHmean+=fabs(DeltaH[j])/(Ntraj/Dbin);
        Exp+=exp(-DeltaH[j])/(Ntraj/Dbin);

    }

    /* It prints delta_tau^2, the error and the average of abs(Delta_H). */
    for(j=0;j<Ntraj/Dbin;j++)
    {
        sum+=(fabs(DeltaH[j])-DHmean)*(fabs(DeltaH[j])-DHmean);
    }

    Err =  sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );
    sum=0.0;
    fprintf(fDHcheck_wo,"%e\t%e\t%e\n",dt,DHmean,Err);
    fclose(fDHcheck_wo);


    /* It prints delta_tau^2, the error and the average of e^(-DeltaH). */
    for(j=0;j<Ntraj/Dbin;j++)
    {
        sum+=(exp(-DeltaH[j])-Exp)*(exp(-DeltaH[j])-Exp);
    }

    Err =  sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );
    sum=0.0;
    fprintf(fExpcheck_wo,"%e\t%e\t%e\n",dt,Exp,Err); 
    fclose(fExpcheck_wo);
    
    
    /****************** DELATA H CHECK with Accdept/Reject ***************/
    /* Initialize variables and phi. */
    DHmean=0.0;
    Exp=0.0;
    ranlxd(phi,V);
    
    /* Thermalization with A/R. */
    Termalization(Nstep,tau,kappa,lambda,Ntherm);
    
    /* Computation of DeltaH[j]. */
    for(j=0;j<Ntraj/Dbin;j++)
    {
        DeltaH[j]=0.0;

	    for(i=0;i<Dbin;i++)
	    {
            DeltaH[j]+= MonteCarloD(Nstep,tau,kappa,lambda)/Dbin;
	    }

        DHmean+=fabs(DeltaH[j])/(Ntraj/Dbin);
        Exp+=exp(-DeltaH[j])/(Ntraj/Dbin);
    }

    /* It prints delta_tau^2, the error and the average of abs(Delta_H). */
    for(j=0;j<Ntraj/Dbin;j++)
    {
        sum+=(fabs(DeltaH[j])-DHmean)*(fabs(DeltaH[j])-DHmean);
    }

    Err=sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) ); 
    sum=0.0;
    fprintf(fDHcheck_w,"%e\t%e\t%e\n",dt,DHmean,Err);
    fclose(fDHcheck_w);

    /* It prints delta_tau^2, the error and the average of e^(-DeltaH). */
    for(j=0;j<Ntraj/Dbin;j++)
    {
        sum+=(exp(-DeltaH[j])-Exp)*(exp(-DeltaH[j])-Exp);
    }

    Err =  sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );
    sum=0.0;
    fprintf(fExpcheck_w,"%e\t%e\t%e\n",dt,Exp,Err); 
    fclose(fExpcheck_w);

    return 0;
}


