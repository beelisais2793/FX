
/*
Alex Greenhalgh
C++ program for Newton Raphson
*/

#include <iostream>
#include <cmath> 
#include <fstream>
using namespace std;


class NewtonRaphson {
        public:
                //Constructor and Destructor
                NewtonRaphson();
                ~NewtonRaphson();
                
                //Public Member Functions
                void print();
                void readin();
                double Func(double xn);
                double derFunc(double xn);
                void Newton1D(double dx);
                void printallvar();
                double x0;
                int maxIT;
                double TOL;
                int niter;
                double dx;
                double xn;
                double DFn;
                double Fxn;
};

// Constructor 
NewtonRaphson::NewtonRaphson() {
        //Initializingd as something large
        double dx = 1000;

        int niter = 0;
        int maxIT = 20;
        double TOL = 10^-14;
        double x0 = 0;
        double xn = x0;
}

// Destructor 
NewtonRaphson::~NewtonRaphson() {
        ;
}

// Printing output function
void NewtonRaphson::print() {
        //print value:s to output file
        ofstream fout("NR_output.txt");
        fout << "Newton Raphson:\nn " << "xn " << "F(xn)\n";
        //Output array into text file
        fout << niter << " " << xn << " " << Fxn << "\n";
}

void NewtonRaphson::printallvar() {
        cout << dx << " " << niter << " " << maxIT << " "  << TOL << endl;

}
// Read in user Input 
void NewtonRaphson::readin() {
        //cin << x0;
        //cin << maxIT;
        maxIT = 20;
        TOL = 10^-14;
        //cin << TOL;
}

void NewtonRaphson::Newton1D(double dx) {
        cout << abs(dx) << endl;
        cout << maxIT << endl;
        if (niter == 0)
                xn = x0;
                cout << "Condition met!";
        // Perform Newton Raphson method until convergence is reached
        for (int i=0;i<20;i++) {

                // Perform individual iteration and display output
                Fxn = Func(xn);
                cout << Fxn << "\n";
                DFn = derFunc(xn);

                cout << "Condition met!" << endl;
                print();

                cout << abs(dx) << endl;
                // Check for convergence or error
                if (abs(dx) < TOL) {
                        if (abs(Fxn) < TOL) {
                                cout << "DONE\n";
                                break;
                        }
                        else {
                                cout << "STUCK\n";
                                break;
                        }
                }
                
                dx = -Fxn / DFn;
                xn += dx;
                niter++;
        }
}

double NewtonRaphson::Func(double xn) {
        return xn * xn - 3.0;
}


double NewtonRaphson::derFunc(double xn) {
        return 2.0 * xn;
}        


int main() {
        // Read in User input
        
        // Initialize the Newton Raphson object
        NewtonRaphson Root;
        Root.printallvar();
        //Root.Newton1D(1000);
       
        return 0;
        


}
