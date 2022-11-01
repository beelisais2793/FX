/**
  \file dsffm.h
  \author Sebastian Zimmermann
  \date September 2020
  \brief function declarations for dsffm.c

  */
#ifndef DSFFM_H
#define DSFFM_H

#include "dsflib.h"
#include "m_pd.h"

void dsffm_run(dsf *x, 
        INPRECISION *in1, INPRECISION *in2, INPRECISION *in3,
        OUTPRECISION *out1, OUTPRECISION *out2, 
        int vector_size); 

void dsffm_set_fundamental(dsf *x, float frequency);


#endif
