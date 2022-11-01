#ifndef _LINEAR_SINE_H_
#define _LINEAR_SINE_H_
#include <cmath>
#include <vector>
#include "sineTest.h"

// In c++, structs are much like classes, except they default to public
// class LinearSine : public SinePrototype would also work here
struct LinearSine : SinePrototype
{
    private:
        vector<double> xx;
        vector<double> sineX;
        int Npoints;
        int Nintervals;

    public:
    // Define your functions here 
    // Class Constructor
	LinearSine(const int nn) : SinePrototype()  
    {
        // Initialize and prepare interpolation of sine
        Nintervals = nn;
        Npoints = nn + 1;
        xx.resize(Npoints);
        sineX.resize(Npoints);
        for(int i = 0; i < Npoints; i++) {
            xx[i] = i*2.*M_PI/Nintervals;
            sineX[i] = sin(xx[i]); 
        }
    }

    // Your sine function
	double sine(double x) {
        int j = (int)(x / (2. * M_PI) * Nintervals);
        // This could probably be faster if we remove the if statement
        if (j>=Nintervals) j = Nintervals-1;
        return sineX[j] + ((x-xx[j])/(xx[j+1]-xx[j]))*(sineX[j+1]-sineX[j]);
	}

};
#endif