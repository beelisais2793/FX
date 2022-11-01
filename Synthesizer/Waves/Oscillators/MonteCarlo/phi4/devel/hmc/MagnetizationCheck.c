/**********************************************************************
 * File MagnetizationCheck.c
 *  
 * It prints <m^2>/V and <|m|> with and without the Accept/Reject step.
 * With: m=Sum_i phi[i] and M=m/V.
 * 
 **********************************************************************/

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
    int j;                                                  /* Index. */
    double dt;                                              /* delta_tau^2. */
    char Mabs_w[100], Mabs_wo[100],M2_w[100],M2_wo[100];    /* Data file names. */
    FILE *fMabs_w, *fMabs_wo, *fM2_w, *fM2_wo;              /* Data files. */
    
    /* It makes sure that an infile is used. */
    if (argc != 2) 
    {
        fprintf(stderr, "Number of arguments not correct\n");
        fprintf(stderr, "Usage: %s <infile> \n",argv[0]);
        exit(1);
    }

    /* It gets the parameters from the input file. */
    read_input(argv[1]);

    /* In the data files names the parameters of the computation are specified. Nstep misses since the idea is to run many times this program to cath the behavior of the ossevable with Nstep. */
    sprintf(Mabs_w, "Data/MagnetizationCheck/Mabs_w_%d_%d_%d_%d_%f_%f",L,hmc_params.tlength,hmc_params.ntraj,hmc_params.ntherm,act_params.kappa,act_params.lambda);
    sprintf(Mabs_wo, "Data/MagnetizationCheck/Mabs_wo_%d_%d_%d_%d_%f_%f",L,hmc_params.tlength,hmc_params.ntraj,hmc_params.ntherm,act_params.kappa,act_params.lambda);
    sprintf(M2_w, "Data/MagnetizationCheck/M2_w_%d_%d_%d_%d_%f_%f",L,hmc_params.tlength,hmc_params.ntraj,hmc_params.ntherm,act_params.kappa,act_params.lambda);
    sprintf(M2_wo, "Data/MagnetizationCheck/M2_wo_%d_%d_%d_%d_%f_%f",L,hmc_params.tlength,hmc_params.ntraj,hmc_params.ntherm,act_params.kappa,act_params.lambda);
    fMabs_w=fopen(Mabs_w,"a");
    fMabs_wo=fopen(Mabs_wo,"a");
    fM2_w=fopen(M2_w,"a");
    fM2_wo=fopen(M2_wo,"a");


    /* Initialize random number generator and the nearest neighbor field. */
    rlxd_init(1,seed);
    hopping(hop);

    /* Initialize phi and dt. */
    ranlxd(phi,V);
    dt=1/((double)hmc_params.nstep*hmc_params.nstep);


    /* Thermalization with A/R. */
    Termalization(hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda,hmc_params.ntherm);
    
    /*****MAGNETIZATION CHECH <M^2> with AR****/
    Magnetization_w(hmc_params.ntraj,hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda,hmc_params.ntherm);
    fprintf(fM2_w,"%e\t%e\t%e\n",dt,mean,error);

    /*****MAGNETIZATION CHECH <abs(M)> with AR****/
    MagnetizationAbs_w(hmc_params.ntraj,hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda,hmc_params.ntherm);
    fprintf(fMabs_w,"%e\t%e\t%e\n",dt,mean,error);

    /* Initialize phi. */
    ranlxd(phi,V);
    
    /* Thermalization without AR. */
    for(j=0;j<hmc_params.ntherm;j++)
	{
        GaussianMom();
        LeapFrog(hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda);
	}

    /*****MAGNETIZATION CHECH <M^2> without AR****/
    Magnetization_wo(hmc_params.ntraj,hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda,hmc_params.ntherm);
    fprintf(fM2_wo,"%e\t%e\t%e\n",dt,mean,error);

    /*****MAGNETIZATION CHECH <abs(M)> without AR****/
    MagnetizationAbs_wo(hmc_params.ntraj,hmc_params.nstep,hmc_params.tlength,act_params.kappa,act_params.lambda,hmc_params.ntherm);
    fprintf(fMabs_wo,"%e\t%e\t%e\n",dt,mean,error);


    fclose(fMabs_w);
    fclose(fMabs_wo);
    fclose(fM2_w);
    fclose(fM2_wo);

    return 0;
}

