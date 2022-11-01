/***************************************************************
 * File functions.c
 * 
 * It contains all the useful functions for the main programs.
 * 
 **************************************************************/

#define CONTROL
#include "phi4.h"
#include "string.h"
#include "functions.h"
#include <time.h>


/* It inizializes the momentum with a gaussian distributions. */
void GaussianMom()
{
	int i;
	double x_1[V], x_2[V];

	ranlxd(x_1,V);
	ranlxd(x_2,V);

    for(i=0; i<V; i++)
    {
        mom[i]= sqrt( -2*log(1-x_1[i]) )*cos( 2*PI*(1-x_2[i]) );
    }
}

/* It prints mom[i] to check if it is gaussian distributed. */
void GaussianMomCheck()
{
	int i;
	double x_1[V], x_2[V];
	FILE *fgauss=fopen("Data/GaussCheck/Gauss","w+");

	ranlxd(x_1,V);
	ranlxd(x_2,V);

    for(i=0; i<V; i++)
    {
        mom[i]= sqrt( -2*log(1-x_1[i]) )*cos( 2*PI*(1-x_2[i]) );
        fprintf(fgauss,"%e\n",mom[i]);
    }
    fclose(fgauss);
}

/* It computes the Hamiltonian.*/
double Hamiltonian(double act)
{
    int i;
    double mom2=0;

    /* loop over all sites */
    for (i=0;i<V;i++)
    {
		mom2+=mom[i]*mom[i];
    }

    return mom2*0.5 + act;
}

/* It evolves the field for a time tau/Nstep. */
void MovePhi(double Nstep, double tau)
{
    int i;

	for(i=0; i<V; i++)
	{
		phi[i] += ((double)tau/Nstep)*mom[i];
	}

}

/* It evolves the momentum for a time tau/Nstep. */
void MoveMom(double Nstep, double tau, double kappa, double lambda)
{

	int i,mu;
	double F[V], phi2, A;

	for(i=0; i<V; i++)
	{
		A=0;
		for (mu=0;mu<D;mu++) A+= phi[hop[i][mu]] + phi[hop[i][D+mu]];
		phi2=phi[i]*phi[i];

		F[i] = -2.0*kappa*A + 2.0*phi[i] + 4.0*lambda*( phi2-1.0  )*phi[i];
	}

	for(i=0; i<V; i++)
	{
		mom[i] = mom[i] - (double)(tau/Nstep)*F[i];
	}
}

/* It integrates the Hamilton equation, evolving the system for a time tau. */
void LeapFrog(int Nstep, int tau, double kappa, double lambda)
{
	int i;

    MoveMom((double)Nstep,(double)tau*0.5,kappa,lambda);
    for(i=0; i<Nstep-1; i++)
    {
		MovePhi((double)Nstep,(double)tau);
		MoveMom((double)Nstep,(double)tau,kappa,lambda);
    }
    MovePhi((double)Nstep,(double)tau);
    MoveMom((double)Nstep,(double)tau*0.5,kappa,lambda);
}

/* Thermalization. */
void Termalization(int Nstep, int tau, double kappa, double lambda, int Ntherm)
{
	int i,j,count=0;
	double act_old, ham_old,act_new, ham_new, DeltaH;
	double phi_old[V];
	double rand[1000];
	
	ranlxd(rand,Ntherm);

	for(j=0; j<Ntherm; j++)
	{

		for(i=0; i<V; i++)
		{
			phi_old[i]=phi[i];
		}
		
		act_old= action();
		GaussianMom();
		ham_old= Hamiltonian(act_old);

		LeapFrog(Nstep,tau,kappa,lambda);

		act_new= action();
		ham_new= Hamiltonian(act_new);
		DeltaH=ham_old-ham_new;

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
}

/* Hybrid MonteCarlo step. */
void MonteCarlo(int Nstep, int tau, double kappa, double lambda)
{
	int i;
	double act_old, ham_old,act_new, ham_new,DeltaH;
	double phi_old[V];
	double rand[1];

	ranlxd(rand,1);
	for(i=0; i<V; i++)
	{
		phi_old[i]=phi[i];
	}

	act_old= action();
	GaussianMom();
	ham_old= Hamiltonian(act_old);

	LeapFrog(Nstep,tau,kappa,lambda);

	act_new= action();
	ham_new= Hamiltonian(act_new);
	DeltaH=ham_new-ham_old;

	if(DeltaH<0){}
	else
	{
		if(exp(-DeltaH)>rand[0]){}
		else
		{
			for(i=0; i<V; i++)
			{
				phi[i]=phi_old[i];
			}
		}
	}

}

/* The same HMC step but it returns Delta_H. This is required in DeltaHCheck.c. */
double MonteCarloD(int Nstep, int tau, double kappa, double lambda)
{
	int i;
	double act_old, ham_old,act_new, ham_new,DeltaH;
	double phi_old[V];
	double rand[1];

	ranlxd(rand,1);
	for(i=0; i<V; i++)
	{
		phi_old[i]=phi[i];
	}

	act_old= action();
	GaussianMom();
	ham_old= Hamiltonian(act_old);

	LeapFrog(Nstep,tau,kappa,lambda);

	act_new= action();
	ham_new= Hamiltonian(act_new);
	DeltaH=ham_new-ham_old;

	if(DeltaH<0){}
	else
	{
		if(exp(-DeltaH)>rand[0]){}
		else
		{
			for(i=0; i<V; i++)
			{
				phi[i]=phi_old[i];
				DeltaH=Hamiltonian(act_old)-Hamiltonian(act_old);
				
			}
		}
	}

	return DeltaH;
}

/* It computes the magnetization squared <M^2> with A/R.*/
void Magnetization_w(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm)
{
	int i,j,k;
	double m=0.0,Mmean=0.0;
	double *M;
	double Err,sum=0.0;

	M=malloc((Ntraj/Dbin)*sizeof(double));    

	for(k=0; k<Ntraj/Dbin; k++)
	{
		M[k]=0.0;

		for(j=0; j<Dbin; j++)
		{
			MonteCarlo(Nstep,tau,kappa,lambda);        
			for(i=0; i<V; i++)
			{
				m+=phi[i];
			}

			M[k]+=(m*m)/((double)V*(double)V*(double)Dbin);
			m=0.0;
		}

		Mmean+=M[k];
	}

    Mmean=Mmean/(Ntraj/(double)Dbin);

    for(j=0;j<Ntraj/Dbin;j++)
    {
    	sum+=(M[j]-Mmean)*(M[j]-Mmean);
    }

    Err=sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );

	mean=Mmean;
	error=Err;

}

/* It computes the magnetization squared <M^2> without A/R.*/
void Magnetization_wo(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm)
{
	int i,j,k;
	double m=0.0,Mmean=0.0;
	double *M;
	double Err,sum=0.0;

	M=malloc((Ntraj/Dbin)*sizeof(double));

	for(k=0; k<Ntraj/Dbin; k++)
	{
		M[k]=0.0;

		for(j=0; j<Dbin; j++)
		{
			for(i=0; i<V; i++)
			{
				m+=phi[i];
			}

			M[k]+=(m*m)/((double)V*(double)V*(double)Dbin);
			m=0.0;

			GaussianMom();
			LeapFrog(Nstep,tau,kappa,lambda);   
		}

		Mmean+=M[k];
	}

    Mmean=Mmean/(Ntraj/(double)Dbin);

    for(j=0;j<Ntraj/Dbin;j++)
    {
    	sum+=(M[j]-Mmean)*(M[j]-Mmean);
    }

    Err=sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );

  	mean=Mmean;
	error=Err;

}

/* It computes the absolute value of the magnetizatio <|M|> with A/R.*/
void MagnetizationAbs_w(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm)
{
	int i,j,k;
	double m=0.0,Mabsmean=0.0;
	double *Mabs;
	double Err,sum=0.0;
	
	Mabs=malloc((Ntraj/Dbin)*sizeof(double));

	for(k=0; k<Ntraj/Dbin; k++)
	{
		Mabs[k]=0.0;

		for(j=0; j<Dbin; j++)
		{
			MonteCarlo(Nstep,tau,kappa,lambda);       
			for(i=0; i<V; i++)
			{
				m+=phi[i];
			}

			Mabs[k]+=fabs(m)/((double)V*Dbin);
			m=0.0;
		}

		Mabsmean+=Mabs[k];
	}

	Mabsmean=Mabsmean/(Ntraj/(double)Dbin);
	for(j=0;j<Ntraj/Dbin;j++)
	{
		sum+=(Mabs[j]-Mabsmean)*(Mabs[j]-Mabsmean);
	}

	Err=sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );

	mean=Mabsmean;
	error=Err;
}

/* It computes the absolute value of the magnetizatio <|M|> without A/R.*/
void MagnetizationAbs_wo(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm)
{
	int i,j,k;
	double m=0.0,Mabsmean=0.0;
	double *Mabs;
	double Err,sum=0.0;

	Mabs=malloc((Ntraj/Dbin)*sizeof(double));

	for(k=0; k<Ntraj/Dbin; k++)
	{
		Mabs[k]=0.0;

		for(j=0; j<Dbin; j++)
		{
			for(i=0; i<V; i++)
			{
				m+=phi[i];
			}

			Mabs[k]+=fabs(m)/((double)V*Dbin);
			m=0.0;

			GaussianMom();
			LeapFrog(Nstep,tau,kappa,lambda);  
		}

		Mabsmean+=Mabs[k];
	}

	Mabsmean=Mabsmean/(Ntraj/(double)Dbin);
	for(j=0;j<Ntraj/Dbin;j++)
	{
		sum+=(Mabs[j]-Mabsmean)*(Mabs[j]-Mabsmean);
	}

	Err=sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );

	mean=Mabsmean;
	error=Err;

}

/* It computes the susceptibility: X=(<m^2>-<|m|>^2)/V with its errors using the cluster Jacknife. */
void Susceptibility(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm)
{
	int i,j,k;
	double m=0.0,Mabsmean=0.0,M2mean=0.0,Xm;
	double *Mabs,*M2, *X;
	double Err,sum=0.0;
	
	Mabs=malloc((Ntraj/Dbin)*sizeof(double));
	M2=malloc((Ntraj/Dbin)*sizeof(double));
	X=malloc((Ntraj/Dbin)*sizeof(double));    

	for(k=0; k<Ntraj/Dbin; k++)
	{
		Mabs[k]=0.0;
		M2[k]=0.0;

		for(j=0; j<Dbin; j++)
		{
			MonteCarlo(Nstep,tau,kappa,lambda);         
			for(i=0; i<V; i++)
			{
				m+=phi[i];
			}

			Mabs[k]+=fabs(m)/((double)Dbin);
			M2[k]+=(m*m)/((double)Dbin);
			m=0.0;
		}

		Mabsmean+=Mabs[k];
		M2mean+=M2[k];
	}

	/* Averages.*/
	Mabsmean=Mabsmean/(Ntraj/(double)Dbin);
	M2mean=M2mean/(Ntraj/(double)Dbin);
	Xm=( M2mean - Mabsmean*Mabsmean)/V;

	/* Jacknife */
	for(i=0;i<Ntraj/(double)Dbin;i++)
	{
		M2[i]=M2mean - ( M2[i] - M2mean )/( Ntraj/(double)Dbin - 1  );
		Mabs[i]=Mabsmean - ( Mabs[i] - Mabsmean )/( Ntraj/(double)Dbin - 1  );
	}

	for(i=0;i<Ntraj/(double)Dbin;i++)
	{
		X[i]=( M2[i] - Mabs[i]*Mabs[i] )/V;
	}

	for(i=0;i<Ntraj/Dbin;i++)
	{
		sum+=(X[i]-Xm)*(X[i]-Xm);
	}

	Err=sqrt( sum*( (double)(Ntraj/Dbin-1)/(Ntraj/Dbin) ) );
	mean=Xm;
	error=Err;
}

/* It computes the Binder Cumulant: U=<m^4>/<m^2>^2 with its errors using the cluster Jacknife. */
void BinderCumulant(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm)
{
	int i,j,k;
	double m=0.0,M4mean=0.0,M2mean=0.0,Um;
	double *M4,*M2, *U;
	double Err,sum=0.0;
	
	M4=malloc((Ntraj/Dbin)*sizeof(double));
	M2=malloc((Ntraj/Dbin)*sizeof(double));
	U=malloc((Ntraj/Dbin)*sizeof(double));
   
	for(k=0; k<Ntraj/Dbin; k++)
	{
		M4[k]=0.0;
		M2[k]=0.0;

		for(j=0; j<Dbin; j++)
		{
			MonteCarlo(Nstep,tau,kappa,lambda);       
			for(i=0; i<V; i++)
			{
				m+=phi[i];
			}
			M4[k]+=(m*m*m*m)/((double)Dbin);
			M2[k]+=(m*m)/((double)Dbin);
			m=0.0;
		}

		M4mean+=M4[k];
		M2mean+=M2[k];
	}

	/* Averages. */
	M4mean=M4mean/(Ntraj/(double)Dbin);
	M2mean=M2mean/(Ntraj/(double)Dbin);
	Um = M4mean/(M2mean*M2mean);

	/* Jacknife. */
	for(i=0;i<Ntraj/(double)Dbin;i++)
	{
		M2[i]=M2mean - ( M2[i] - M2mean )/( Ntraj/(double)Dbin - 1  );
		M4[i]=M4mean - ( M4[i] - M4mean )/( Ntraj/(double)Dbin - 1  );
	}

	for(i=0;i<Ntraj/(double)Dbin;i++)
	{
		U[i]=M4[i]/(M2[i]*M2[i]);
	}

	for(i=0;i<Ntraj/Dbin;i++)
	{
		sum+=(U[i]-Um)*(U[i]-Um);
	}

	Err=sqrt( sum*( (double)(Ntraj/Dbin-1)/(Ntraj/Dbin) ) );

	mean=Um;
	error=Err;
}

/* It computes U, X, and <|M|> with their errors using the Jacknife. */
void Results(int Ntraj, int Nstep, int tau, double kappa, double lambda,int Ntherm)
{
	int i,j,k;
	double m=0.0,MXmean=0.0,Mmean=0.0,M2mean=0.0,M4mean=0.0,Um,Xm;
	double *M,*M2,*M4,*U,*X;
	double Err,Err1,sum=0.0,sum1=0.0;
	
	M=malloc((Ntraj/Dbin)*sizeof(double)); 
	M2=malloc((Ntraj/Dbin)*sizeof(double));
	M4=malloc((Ntraj/Dbin)*sizeof(double));
	U=malloc((Ntraj/Dbin)*sizeof(double));
	X=malloc((Ntraj/Dbin)*sizeof(double));

	for(k=0; k<Ntraj/Dbin; k++)
	{
		M[k]=0.0;
		M2[k]=0.0;
		M4[k]=0.0;

		for(j=0; j<Dbin; j++)
		{
			MonteCarlo(Nstep,tau,kappa,lambda);         
			for(i=0; i<V; i++)
			{
				m+=phi[i];
			}

			M[k]+=fabs(m)/((double)Dbin);
			M2[k]+=(m*m)/((double)Dbin);
			M4[k]+=(m*m*m*m)/((double)Dbin);
			m=0.0;
		}

		Mmean+=M[k];
		MXmean+=M[k];
		M2mean+=M2[k];
		M4mean+=M4[k];
	}

	/* MAGNETIZATION */
	Mmean=Mmean/V;
	Mmean=Mmean/(Ntraj /(double)Dbin);
	for(j=0;j<Ntraj/Dbin;j++)
	{
		sum+=((M[j]/V)-Mmean)*((M[j]/V)-Mmean);
	}

	Err=sqrt( sum/( (Ntraj/Dbin)*(Ntraj/Dbin-1) ) );
	mean=Mmean;
	error=Err;
	sum=0.0;


	/* SUSCEPTIBILITY + BINDER CUMULANT*/
	MXmean=MXmean/(Ntraj/(double)Dbin);
	M2mean=M2mean/(Ntraj/(double)Dbin);
	M4mean=M4mean/(Ntraj/(double)Dbin);
	Xm=( M2mean - MXmean*MXmean)/V;
	Um = M4mean/(M2mean*M2mean);

	/* Jacknife */
	for(i=0;i<Ntraj/(double)Dbin;i++)
	{
		M[i]=MXmean - ( M[i] - MXmean )/( Ntraj/(double)Dbin - 1  );
		M2[i]=M2mean - ( M2[i] - M2mean )/( Ntraj/(double)Dbin - 1  );
		M4[i]=M4mean - ( M4[i] - M4mean )/( Ntraj/(double)Dbin - 1  );
	}

	for(i=0;i<Ntraj/(double)Dbin;i++)
	{
		X[i]=( M2[i] - M[i]*M[i] )/V;
		U[i]=M4[i]/(M2[i]*M2[i]);
	}

	for(i=0;i<Ntraj/Dbin;i++)
	{
		sum+=(X[i]-Xm)*(X[i]-Xm);
		sum1+=(U[i]-Um)*(U[i]-Um);
	}

	Err=sqrt( sum*( (double)(Ntraj/Dbin-1)/(Ntraj/Dbin) ) );
	Err1=sqrt( sum1*( (double)(Ntraj/Dbin-1)/(Ntraj/Dbin) ) );
	Xmean=Xm;
	Xerror=Err;
	Umean=Um;
	Uerror=Err1;

}




