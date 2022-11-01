
/*******************************************************************************
*
* File mainAction.h
*
*******************************************************************************/

#ifndef MAINACTION_H
#define MAINACTION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void f(unsigned int size);

/*Data structure to store all the parameters of the algorithm.*/
typedef struct {
	            double a;       
                int ThermalT;     
	            int Nsweep;
                int Dbin;
                double L;      
               } params;


#endif

