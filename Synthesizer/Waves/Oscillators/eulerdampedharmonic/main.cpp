#include <iostream>
#include <fstream>    //includes commands related to creating, reading and writing files
#include <math.h>

using namespace std;

double vdot(double, double, double, double, double);    // preparing the computer to receive a function

int main() {

    ofstream outputFile;                                                    // this creates the output variable name
    outputFile.open("spring.csv",ios::app);                       // creates the output file

    double b = 5;                                                               // b is the coefficient of resistance,
    double m;                                                                  // m is the mass
    double k;                                                                     // k is the spring constant
    double told = 0;
    double tnew = 0;
    double vold = 0;
    double vnew = 0;
    double xold = 1;
    double xnew = 0;
    double dt = .01;                                                             // time increments

    cout << "Please input the mass of the object: " << endl;                            // Prompting the user for inputs
    cin >> m;
    cout << "Please input the spring constant: " << endl;
    cin >> k;

    for(int i=0; i<1001; i++) {                                                                        // a for loop which will generate data points for oscillator

        outputFile << told << "," << xold << "," << vold << endl;                     // sending info to the file
        cout << told << ", " << xold << ", " << vold << endl;                            // sending info to screen

        xnew = xold + vold*dt;                                                                        // calculating the new position from old position added to product of velocity and time
        vnew = vold + vdot(k, xold, m, b, vold)*dt;                                         // calculating the new velocity from old velocity added to product of acceleration function and time
        tnew = told + dt;                                                                                 // calculating new time
        xold = xnew;
        vold = vnew;                                                                                       // updating position, velocity and time
        told = tnew;

    }    //closes for loop

    outputFile.close();                                                                                   // closes created file
    
    return 0;
}

double vdot(double k, double x, double m, double b, double v) {                // defines the function

    return -(k*x/m)-(b*v/m);                                                                            // acceleration function from the spring force and damping force
} //closes function
