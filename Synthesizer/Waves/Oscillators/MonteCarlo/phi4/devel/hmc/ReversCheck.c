/************************************************************************************************************
 * File ReversCheck.c
 *  
 * It checks the Leap Frog reversibility: starting from a random field phi_in[i], MD equations are integrated
 * for a time tau, then the sign of the momentum is changed to going back to the initial field phi[i]. 
 * The the difference, abs(phi_in[i]-phi[i])/V is printed to terminal. 
 *
 ************************************************************************************************************/

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
    int i;                               /* Index. */
    double tau;                          /* Auxiliary varialble.*/
    double phi_in[V], Delta_phi=0.0;     /* The initial field and its variation. */

    /* It makes sure that an infile is used. */
    if (argc != 2) 
    {
        fprintf(stderr, "Number of arguments not correct\n");
        fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
        exit(1);
    }

    /* It gets the parameters from the input file. */
    read_input(argv[1]);
    tau=hmc_params.tlength;

    /* Initialize random number generator, the nearest neighbor field and phi.*/
    rlxd_init(1,seed);
    hopping(hop);
    ranlxd(phi,V);

    /****************** REVESIBILITY CHECK ***************/
    
    /* It saves initial field. */
    for(i=0; i<V; i++)
    {
    	phi_in[i]=phi[i];
    }

    /* It initializes mom field and then it evolves the system for a time tau. */
    GaussianMom();
    LeapFrog(hmc_params.nstep,tau,act_params.kappa,act_params.lambda);

    /*It reverses the momentum.*/
    for(i=0; i<V; i++)
    {
        mom[i]= -1.0*mom[i];
    }

    /*It evolves the system for a time tau.*/
    LeapFrog(hmc_params.nstep,tau,act_params.kappa,act_params.lambda);

    /*It computes the difference between the initial field and the new field.*/
    for(i=0; i<V; i++)
    {
        Delta_phi+=fabs(phi_in[i]-phi[i])/V;
    }

    printf("Delta_phi = %e\n",Delta_phi);


    return 0;
}
