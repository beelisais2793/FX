#ifndef H_OSCIL
#define H_OSCIL

#include <stdint.h>
// A classical harmonic oscillator
class CHarmonic_Oscillator {
public:
	double m; // Rest mass
	double k; // Hooke's Constant
	double v; // Velocity
	double theta; // Angle with the horizontal
	double x; // Current position (equilibria at 0)
	double y; 
	double T; // Kinetic Energy
	double V; // PE
	double U; // Totaldouble t; // Implementation variable, frequency of calling.
	double l_0x, l_0y; // Natural length from pivot
	
	CHarmonic_Oscillator(double m, double k, double l_0x, double l_0y, double i, double theta); // i = impulse
	void Update(void); // Updates the oscillator
	
};

#endif
