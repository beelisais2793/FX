/**
 * \file dsflib_defines.h
 * \author Sebastian Zimmermann, based on a file by <br>
 * Thomas Resch <br>
 * Audiocommunication Group, Technical University Berlin <br>
 * University of Applied Sciences Nordwestschweiz (FHNW), Music-Academy, Research and Development <br>
 * Preprocessor Instructions for the dsf library

   \brief Define macros to customize compilation

   Macros for pi and float precision.
 */

#ifndef DSF_DEFINES_H
#define DSF_DEFINES_H


#define PI 3.1415926535898


/** Data Type Macro for flexible input vector single or double floating point precision */
#define DSF_INPUTVECTOR_USE_FLOAT
#define DSF_OUTPUTVECTOR_USE_FLOAT

#ifdef DSF_INPUTVECTOR_USE_FLOAT
typedef float INPRECISION;
#else
typedef double INPRECISION;
#endif

/** Data Type Macro for flexible output vector single or double floating point precision */
#ifdef DSF_OUTPUTVECTOR_USE_FLOAT
typedef float OUTPRECISION;
#else
typedef double OUTPRECISION;
#endif

#endif /* DSF_DEFINES_H */
