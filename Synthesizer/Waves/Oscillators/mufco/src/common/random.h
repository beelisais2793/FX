/*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*    Copyright (C) 2013  Manuel Castillo Cagigal
*
*    Authors: Manuel Castillo Cagigal <manuel.castillo@upm.es>
*             Eduardo Matallanas de Avila <eduardo.matallanas@upm.es>
*	      Alvaro Gutierrez Martin <aguti@etsit.upm.es>
*
*    Developed in Robolabo <http://www.robolabo.etsit.upm.es/>
*/

/********************************************/
/* RANDOM NUMBER GENERATOR HEADER 	    */
/********************************************/
#ifndef CRandom_H
#define CRandom_H

#include <gsl/gsl_rng.h>
#include <math.h>
#include <assert.h>
#include <iostream>

/* position of right-most step */
#define PARAM_R 3.44428647676

using namespace std;

class CRandom {
	private:

	char*     m_pchParamsFile;	
	gsl_rng*  m_rng;
	double    m_rng_norm;
	int       m_nseed;

	static const double        ytab[128];
	static const unsigned long ktab[128];
	static const double        wtab[128];	

	double        _gsl_ran_gaussian_ziggurat ( gsl_rng* , double );
	unsigned long _gsl_rng_uint32            ( gsl_rng* );
  
	public:

	CRandom  ( );
	CRandom  ( int );
	~CRandom ( void ); 
  
  	// uniformly distributed double in [0,1)
  	double nextDouble() { return gsl_rng_get (m_rng) * m_rng_norm;}
  
  	// uniformly distributed double in [lbound, ubound)
  	double nextDouble(double lbound, double ubound);
  
  	// uniformly distributed double in [0, ubound)
  	double nextDouble(double ubound);
  
  	// uniformly distributed integer in {lbound, ..., ubound-1}
  	int nextInt(int lbound, int ubound);
  
  	// uniformly distributed integer in {0, ..., ubound-1}
  	int nextInt(int ubound);
  
	//Gaussian object
	double nextGaussian (double mean, double sigma);
  
  	// uniformly distributed boolean
 	bool nextBoolean();
  
};



#endif
