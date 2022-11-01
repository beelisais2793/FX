
/*
Alex Greenhalgh
 Modelling of Ostwald batch ripening process of crystals of N sizes in C++ using Multivariate Euler Method
*/

#include <iostream>
#include <cmath> 
#include <math.h>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

//Function
void Func(vector<double>& xt_vec) {
    // Define Constants
	double k = 5.0e7;
    double x_star_1 = 0.05;
	double x_star_2 = 0.09;
    double u = 1e-3;
    double c_star = 7.52e-7;
    double L_flip = 4.000e-3;
    double c0 = 1.05 * c_star;
	
	double c1 =  c0 + (u * x_star_1 * x_star_1 * x_star_1) + (u * x_star_2 * x_star_2 * x_star_2);
	double xt_sum = (u * xt_vec[0] * xt_vec[0] * xt_vec[0]) + (u * xt_vec[1] * xt_vec[1] * xt_vec[1]);
	double c_t = c1 - xt_sum;
	
	for(int i = 0;i < xt_vec.size();i++) {
		double c_crit = c_star * exp(L_flip / xt_vec[i]);
		double new_xt = k * (c_t - c_crit);
		xt_vec[i] = new_xt;
	}
	cout << "\n The new values of x are " << xt_vec[0] << "and " << xt_vec[1] << "\n";
}


void Euler(double t0,double x_star_1,double x_star_2,double tend, int Nsteps) {

	double tn, delT, x1n, x2n;
	vector<double> xt_vec;
	//Prepare output file and formatting
	ofstream fout;
	fout.open("Euler_out.txt");
	fout << "Euler Multivariate Scheme Lab 6 MATH 475\n"
		 << "Initial Time t0= " << t0 << endl
		 << "Initial Value F(x)= " << y0 << endl
		 << "Ending Time= " << tend << endl
		 << "Steps= " << Nsteps << endl
		 << "Begin Method...\n"
		 << left << setw(10)
		 << "n" << setw(12)
         << "tn" << setw(12) << " "
         << "x1" << setw(12) << " "
		 << "x2" << endl;
         //<< "Yexactn" << setw(10)
		 //<< "ERRn" << setw(10) << endl;
		 

	// Calculating timestep increment
	delT = (tend - t0) / Nsteps;
	
    xt_vec.push_back(x_star_1);	
	xt_vec.push_back(x_star_2);	
	double xt1 = x_star_1;
	double xt2 = x_star_2;
	
	for(vector<double>::iterator it=xt_vec.begin();it!=xt_vec.end();it++) {
		cout << *it;
	}
	

	//
	for(int i=0; i <= Nsteps; i++) {
		
		//Advance Euler Scheme
		tn = t0 + i * delT;		

		
		// Output result of increment
		fout << setw(10)
			 << fixed
			 << i << setw(10) << setprecision(8)
			 << tn << setw(12) << setprecision(10) << " "
			 << xt1 << setw(12) << " "
			 << xt2 << endl;
		cout << "debug: The delt is:" << delT << "\n";
		cout << "debug: The xt_vec[0] is:" << xt_vec[0] << "\n";
		cout << "debug: The xt_vec[1] is:" << xt_vec[1] << "\n";

		Func(xt_vec);
		xt1 += delT * xt_vec[0];
		xt2 += delT * xt_vec[1];
		xt_vec[0] = xt1;
		xt_vec[1] = xt2;
		if (xt1 <= 0.0 || xt2 <= 0.0 ) {
			break;
		}
		
	}
	fout.close();

}

int main() {
	//Function inputs	
    double t0 = 0;
	double x_star_1 = 0.05;
	double x_star_2 = 0.09;
	double tend = 0.02;
	int Nsteps = 10000;
		
	Euler( t0, x_star_1, x_star_2, tend, Nsteps);  
	   
    return 0;
}


