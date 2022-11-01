/*******************************************************************************
*
* File types.h
*
* Type definitions and macros for complex numbers
*
* Author: Leonardo Giusti <Leonardo.Giusti@cern.ch>
*
*******************************************************************************/

#ifndef TYPES_H
#define TYPES_H

typedef struct
{
   float re,im;
} complex;

typedef struct
{
   double re,im;
} complex_dble;

/*
* a=b*c (a,b,c complex)
*/

#define _complex_mul(a,b,c) \
   (a).re=((b).re*(c).re-(b).im*(c).im); \
   (a).im=((b).re*(c).im+(b).im*(c).re)


/*
* a=b+c (a,b,c complex)
*/

#define _complex_add(a,b,c) \
   (a).re=((b).re+(c).re); \
   (a).im=((b).im+(c).im)


/*
* a=b-c (a,b,c complex)
*/

#define _complex_sub(a,b,c) \
   (a).re=((b).re-(c).re); \
   (a).im=((b).im-(c).im)


/*
* a=b/c (a,b,c complex)
*/

#define _complex_div(a,b,c) \
   (a).re=(((b).re*(c).re+(b).im*(c).im)/((c).re*(c).re+(c).im*(c).im)); \
   (a).im=(((b).im*(c).re-(b).re*(c).im)/((c).re*(c).re+(c).im*(c).im))


#endif





