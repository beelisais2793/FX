#ifndef LATTICE_H
#define LATTICE_H

/* Dimension of the lattice */
#define D 3
/* spatial extend of the lattice */
#define L 8
/* lattice volume, needs to be adjusted according to number of dimensions*/
#define V (L*L*L)
/* Lenght of the bin. */
#define Dbin 1000
#define PI 3.1415926535897932

#ifdef CONTROL
#define EXTERN
#undef CONTROL
#else
#define EXTERN extern
#endif

EXTERN double phi[V];
EXTERN double mom[V];
EXTERN int    hop[V][2*D];      /* Hopping field. */
EXTERN double mean, error;      /* Global variables used to print <|M|> and its error. */
EXTERN double Xmean, Xerror;    /* Global variables used to print X and its error. */
EXTERN double Umean, Uerror;    /* Global variables used to print U and its error. */

#endif
