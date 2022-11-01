/**
  \file dsf.h
  \author Sebastian Zimmermann
  \date September 2020
  \brief function declarations for dsf.c

  */
#ifndef DSF_H
#define DSF_H

#include "dsflib.h"
#include "m_pd.h" 

void dsf_run(dsf *x, OUTPRECISION *out1, OUTPRECISION *out2, int vector_size); 

void dsf_set_fundamental(dsf *x, float frequency);

void dsf_set_distance(dsf *x, float distance);

void dsf_set_weight(dsf *x, float weight);

void dsf_set_num_of_sines(dsf *x, int num_of_sines);

void dsf_set_frequency(dsf *x, float frequency);

void dsf_set_ratio(dsf *x, float ratio);


#endif
