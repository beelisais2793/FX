
/*******************************************************************************
*
* File start.c
*
* Collection of programs that serve to initialize the fields
*
* The externally accessible functions are
*
*   void start_ranlux(int level,int seed)
*     Initializes the random number generators ranlxs and ranlxd.
*     The luxury level should be 0 (recommended) or 1 (exceptional),
*     and the seed can be any integer in the range from 1 to 2^31-1 
*
* Author: Leonardo Giusti <Leonardo.Giusti@cern.ch>
*
*******************************************************************************/

#define START_C

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "random.h"
#include "start.h"

void start_ranlux(int level,int seed)
{
   error((level<0)||(level>1)||(seed<1),1,"start_ranlux [utils.c]",
         "Parameters out of range (level should be 0 or 1, and seed>0)");

   rlxs_init(level,seed);
   rlxd_init(level+1,seed);
}



