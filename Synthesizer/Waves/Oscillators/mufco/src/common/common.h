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
/* COMMON HEADER 							      */
/* It contains common data types and structures				      */
/******************************************************************************/
#ifndef COMMON_H_
#define COMMON_H_

/******************************************************************************/
/* LIBRARIES 								      */
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <complex>
#include <algorithm>
#include "tinyxml2.h"
#include "fft.h"
#include "random.h"

using namespace std;
using namespace tinyxml2;

/* TYPES */
typedef vector<int>      TVInt;

typedef vector<float>    TVFloat;
typedef vector<TVFloat>  TMFloat;
typedef vector<TVFloat*> TMFloat_pt;

struct SFreqCmp {	
	float amp;
	float phs;	
	float period;	
};
typedef vector<SFreqCmp>  TVFreqCmp;
typedef vector<TVFreqCmp> TMFreqCmp;

struct SFFTst {
	float   FFTave;
	float   FFTvar;	
	float   FFTmax;
	float   FFTff;
	TVFloat HAve;
	TVFloat HVar;
	TVFloat HRel;
	TMFloat HAmp;
};
#endif



