
/*******************************************************************************
*
* File global.h
*
* Global parameters and arrays
*
*******************************************************************************/
#include <stddef.h>
#ifndef GLOBAL_H
#define GLOBAL_H



#if defined MAIN_PROGRAM
  #define EXTERN
#else
  #define EXTERN extern
#endif

EXTERN double* xx;    /*Positions at each physical time.*/

#undef EXTERN

#endif

