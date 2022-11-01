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

/******************************************************************************/
/* FFT HEADER 							    	      */
/******************************************************************************/
#ifndef FFT_H_
#define FFT_H_

/* LIBRARIES */
#include <stdlib.h>
#include <math.h>

/* MACROS */
#define TWO_PI 6.2831853071795864769252867665590057683943L

class CFFT {
	public:
	void fft        ( int , double (*time)[2], double (*freq)[2] );	
	void ifft       ( int , double (*time)[2], double (*freq)[2] );
	private:
	void _iteration ( int N, int offset, int delta, double (*x)[2], double (*X)[2], double (*XX)[2] );
};
#endif
