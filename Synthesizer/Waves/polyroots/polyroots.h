/* Starting from version 7.8, MATLAB BLAS expects ptrdiff_t arguments for integers */
#include <stddef.h>
#include <stdlib.h>

int cpoly(double *, double *, ptrdiff_t *, double *, double *, ptrdiff_t *);
void free_cpoly();

int rpoly(double *, ptrdiff_t *, double *, double *, ptrdiff_t *);
void free_rpoly();