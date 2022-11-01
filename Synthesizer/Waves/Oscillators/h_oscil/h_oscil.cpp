#include "h_oscil.h"
#include <math.h>
#include <stdio.h>

double signum(double x) {
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}

void CHarmonic_Oscillator::Update(void) {
	// Calculate extension in spring
	double extension = (sqrtf(this->x*this->x + this->y*this->y) - sqrt(this->l_0x*this->l_0x + this->l_0y*this->l_0y));
	//this->v += (F / this->m);
	// Find v_x and v_y
	double v_x = v * cos(theta);
	double v_y = v * sin(theta);
	// find the angle made by the force
	double phi = 0.0f;
	// Division by zero error-prevention
	if(this->x == 0 && this->y == 0)
		phi = 0;
	else
		phi = atan2f(this->y, this->x);
	
	double F = - this->k * extension;
	// F_x and F_y
	double F_x = F * cos (phi);
	double F_y = F * sin (phi);
	v_x += F_x / this->m;
	v_y += F_y / this->m;
	this->theta = atan2f(v_y, v_x);
	this->v = sqrtf(v_x*v_x + v_y*v_y);
	this->x += v_x;
	this->y += v_y;
	this->T = (0.5)*(this->m)*(this->v)*(this->v);
	this->V = (0.5)*(this->k)*(extension*extension);
}

CHarmonic_Oscillator::CHarmonic_Oscillator(double m, double k, double l_0x, double l_0y, double i, double theta) {
	this->m = m;
	this->k = k;
	this->v = i / this->m; // Impulse delivered
	this->theta = (M_PI_2/180)*theta;
	this->l_0x = l_0x;
	this->l_0y = l_0y;
	this->U = (0.5)*(this->m)*(this->v)*(this->v);
	this->T = this->U;
	this->V = 0;
	this->y = this->l_0y;
	this->x = this->l_0x;
}
