/*****************************************************************************************************
 * File GaussCheck.c
 * 
 * It prints mom[i] so we can check if it is gaussian distributed.
 * It prints the action starting both from the Hot and Cold configuration, either with and without AR.
 *   
 *****************************************************************************************************/

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
    int i,j,count=0;                                          /* Indices and a counter for the acceptance. */
    double r[V],S;                                            /* A vector of random numbers, required to initialize the field to the Hot configuration, and a variable for the action. */
    double act_old, ham_old,act_new, ham_new, DeltaH;         /* Variables for the action, the hamiltonian and Delta_H. */
	double phi_old[V];                                        /* It stores the old field in case that the new one is rejected. */
	double rand[1000];                                        /* A vector of random numbers required for the accept/reject step. */
    char Scoldwo[100], Shotwo[100],Shotw[100],Scoldw[100];    /* Data file names. */
    FILE *fScold_wo_AR,*fShot_wo_AR,*fScold_w_AR,*fShot_w_AR; /* Data files. */
	
    /* It makes sure that an infile is used. */
    if (argc != 2) 
    {
        fprintf(stderr, "Number of arguments not correct\n");
        fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
        exit(1);
    }

    /* It gets the parameters from the input file. */
    read_input(argv[1]);
    
    /* In the data files names the parameters of the computation are specified. */
    sprintf(Scoldwo, "Data/GaussCheck/Thermalization/Scold_wo_%d_%d_%d_%d_%d_%f_%f",L,Nstep,tau,Ntraj,Ntherm,kappa,lambda);
    sprintf(Shotwo, "Data/GaussCheck/Thermalization/Shot_wo_%d_%d_%d_%d_%d_%f_%f",L,Nstep,tau,Ntraj,Ntherm,kappa,lambda);
    sprintf(Scoldw, "Data/GaussCheck/Thermalization/Scold_w_%d_%d_%d_%d_%d_%f_%f",L,Nstep,tau,Ntraj,Ntherm,kappa,lambda);
    sprintf(Shotw, "Data/GaussCheck/Thermalization/Shot_w_%d_%d_%d_%d_%d_%f_%f",L,Nstep,tau,Ntraj,Ntherm,kappa,lambda);
    fScold_wo_AR=fopen(Scoldwo,"w+");
    fShot_wo_AR=fopen(Shotwo,"w+");
    fScold_w_AR=fopen(Scoldw,"w+");
    fShot_w_AR=fopen(Shotw,"w+");


    /* Initialize random number generator and the nearest neighbor field..*/
    rlxd_init(1,seed);
    hopping(hop);   

    /* Initialize phi field and save it. */
    ranlxd(phi,V);
    ranlxd(rand,Ntherm);

    /****************** GAUSSIAN MOMENTUM CHECK ***************/
    GaussianMomCheck();


    /**************** THERMALIZATION without A/R *************/
    
    /* Cold initialization of phi field. */
    for(i=0;i<V;i++)
    {
        phi[i]=1.0;
    }

    /* Thermalization: it prints the action. */
    for(j=0;j<Ntherm;j++)
	{
        S=action();
        fprintf(fScold_wo_AR,"%d\t%e\n",j,S);
        GaussianMom();
        LeapFrog(hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda);
	}
    

    /* Hot initialization of phi field. */
    ranlxd(r,V);
    for(i=0;i<V;i++)
    {
        phi[i]=4*(r[i] -0.5);
    }

    /* Thermalization: it prints the action. */
    for(j=0;j<Ntherm;j++)
	{
        S=action();
        fprintf(fShot_wo_AR,"%d\t%e\n",j,S);  
        GaussianMom();
        LeapFrog(hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda);      
	}


    /****************** THERMALIZATION with A/R ***************/

    /* Cold initialization of phi field. */
    for(i=0;i<V;i++)
    {
        phi[i]=1.0;
    }

    /* Thermalization. */
	for(j=0; j<Ntherm; j++)
	{

		for(i=0; i<V; i++)
		{
			phi_old[i]=phi[i];
		}
		
		act_old= action();
        fprintf(fScold_w_AR,"%d\t%e\n",j,act_old);
		GaussianMom();
		ham_old= Hamiltonian(act_old);

		LeapFrog(Nstep,tau,kappa,lambda);

		act_new= action();
		ham_new= Hamiltonian(act_new);
		DeltaH=ham_new-ham_old;
    
		if(DeltaH<0)
		{
			count++;
		}
		else
		{
			if(exp(-DeltaH)>rand[j])
			{
				count++;
			}
				else
				{
					for(i=0; i<V; i++)
					{
						phi[i]=phi_old[i];
					}
				}
		}

	}

	printf("Accettabilità = %d\n",count);
    count=0;	    
    
    /* Hot initialization of phi field. */
    ranlxd(r,V);
    for(i=0;i<V;i++)
    {
        phi[i]=4*(r[i] - 0.5);
    }

    /* Thermalization. */
    for(j=0; j<Ntherm; j++)
	{

		for(i=0; i<V; i++)
		{
			phi_old[i]=phi[i];
		}
		
		act_old= action();
        fprintf(fShot_w_AR,"%d\t%e\n",j,act_old);
		GaussianMom();
		ham_old= Hamiltonian(act_old);

		LeapFrog(Nstep,tau,kappa,lambda);

		act_new= action();
		ham_new= Hamiltonian(act_new);
		DeltaH=ham_new-ham_old;

		if(DeltaH<0)
		{
			count++;
		}
		else
		{
			if(exp(-DeltaH)>rand[j])
			{
				count++;
			}
            else
            {
                for(i=0; i<V; i++)
                {
                    phi[i]=phi_old[i];
                }
            }
		}

	}

	printf("Accettabilità = %d\n",count);
    
    fclose(fScold_wo_AR);
    fclose(fShot_wo_AR);
    fclose(fScold_w_AR);
    fclose(fShot_w_AR);

    return 0;

}

