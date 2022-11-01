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
/* FFT METHODS 							    	      */
/******************************************************************************/

#include "fft.h"

/****************************************************************/
void CFFT::fft( int size, double (*time)[2], double (*freq)[2] ){	
	double partial [size][2];  	
  	_iteration( size , 0 , 1 , time , freq , &partial[0]);  	
	return;
};

/****************************************************************/
void CFFT::ifft( int size, double (*time)[2], double (*freq)[2]) {
  	int hsize = size/2;      
  	double tmp0, tmp1;    
	fft( size , freq , time );
	time[0][0]     = time[0][0]/size;    
	time[0][1]     = time[0][1]/size;
	time[hsize][0] = time[hsize][0]/size;  
	time[hsize][1] = time[hsize][1]/size;
	for( int i = 1 ; i < hsize ; i++){
		tmp0            = time[i][0]/size; 
	        tmp1            = time[i][1]/size;
      		time[i][0]      = time[ size - i ][0]/size;  
	        time[i][1]      = time[ size - i ][1]/size;
      		time[size-i][0] = tmp0;       
		time[size-i][1] = tmp1;
    	}
	return;
};

/*****************************************************************/
void CFFT::_iteration( int size, int offset, int delta, double (*x)[2], double (*X)[2], double (*XX)[2]){
	int    hsize = size/2; 	          
	int    k00, k01, k10, k11;  
	double cs, sn , tmp0, tmp1;       
	if( size != 2 ){
		/* Divide FFT in two FFTs */
		float tmp_arg;
		_iteration( hsize , offset         , 2 * delta , x , XX , X );
		_iteration( hsize , offset + delta , 2 * delta , x , XX , X );		
		for( int k = 0 ; k < hsize ; k++ ){
          		k00  = offset + k*delta;    
			k01  = k00    + hsize*delta;
          		k10  = offset + 2*k*delta;  
			k11  = k10    + delta;
			tmp_arg = TWO_PI * k / (double)size;
          		cs   = cos ( tmp_arg ); 
			sn   = sin ( tmp_arg );
          		tmp0 = cs * XX[k11][0] + sn * XX[k11][1];
          		tmp1 = cs * XX[k11][1] - sn * XX[k11][0];
          		X[k01][0] = XX[k10][0] - tmp0;
          		X[k01][1] = XX[k10][1] - tmp1;
         		X[k00][0] = XX[k10][0] + tmp0;
          		X[k00][1] = XX[k10][1] + tmp1;
       		}
    	}
  	else{
      		k00       = offset; 
		k01       = k00 + delta;
      		X[k01][0] = x[k00][0] - x[k01][0];
      		X[k01][1] = x[k00][1] - x[k01][1];
      		X[k00][0] = x[k00][0] + x[k01][0];
      		X[k00][1] = x[k00][1] + x[k01][1];
    	}
};


