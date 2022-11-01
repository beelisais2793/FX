
/*
Alex Greenhalgh
C++ program for Euler Scheme
*/

#include <iostream>
#include <cmath> 
#include <math.h>
#include <fstream>
#include <iomanip>
using namespace std;

//Change to pass by reference
double Func1(double tn, double Y1n, double Y2n) {
    // Define Constants
	double k = 5.0e7;
    double x_star = 0.08;
    double u = 1e-5;
    double c_star = 7.52e-7;
    double L_flip = 4.000e-3;
    double c0 = 1.05 * c_star;


	double c_t = c0 + (u * x_star * x_star * x_star) - (u * xt * xt * xt);
	double c_crit = c_star * exp(L_flip / xt);
    return k * (c_t - c_crit);
}

double Func2(double tn, double Y1n, double Y2n) {
    // Define Constants
	double k = 5.0e7;
    double x_star = 0.08;
    double u = 1e-5;
    double c_star = 7.52e-7;
    double L_flip = 4.000e-3;
    double c0 = 1.05 * c_star;


	double c_t = c0 + (u * x_star * x_star * x_star) - (u * xt * xt * xt);
	double c_crit = c_star * exp(L_flip / xt);
    return k * (c_t - c_crit);
}

// double IntFunc(double tn, double Y1n, double Y2n) {
        // return cosh(tn);
// }

void Euler(double t0,double x_star_1,double x_star_2,double tend, int Nsteps) {

	double tn, delT, x1n, x2n;
	
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
		 << "n" << setw(10)
         << "tn" << setw(10)
         << "x1" << setw(10)
		 << "x2" << endl;
         //<< "Yexactn" << setw(10)
		 //<< "ERRn" << setw(10) << endl;
		 

	// Calculating timestep increment
	delT = (tend - t0) / Nsteps;
	//ERRmax = 0; 
    x1n = x_star_1;	
	x2n = x_star_2;

	//
	for(int i=0; i <= Nsteps; i++) {
		
		//Advance Euler Scheme
		tn = t0 + i * delT;		

		//yexact = IntFunc(tn,Y1n, Y2n);
		//Errn = abs(yexact - Y1n);
		
		
		//Update errmax
		//if (Errn > ERRmax) {
		//	ERRmax = Errn;
		//}
		
		// Output result of increment
		fout << setw(10)
			 << fixed
			 << i << setw(10)
			 << tn << setw(10)
			 << x1n << setw(10)
			 << x2n << endl;
			 //<< scientific << endl;
			 //<< Errn << setw(10) << endl;
			 
		x1n += delT * Func1(tn, Y1n, Y2n);
		x2n += delT * Func2(tn, Y1n, Y2n);
		
	}
	//fout << "\nDONE: Max Error is =" << ERRmax;
	fout.close();

}

int main() {
	//Function inputs	
    double t0 = 0;
	double x_star_1 = 0.08;
	double x_star_2 = 0.08;
	double tend = 0.5;
	int Nsteps = 10000;
		
	//Launch Newton Rootfinder
	Euler( t0, x_star_1, x_star_2, tend, Nsteps);  
	   
    return 0;
}


