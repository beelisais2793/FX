/**
  \file dsffm.c
  \author Sebastian Zimmermann
  \date September 2020
  \brief wrapping dsflib-functions to performance-friendly pd object

  Main functionality is found in dsflib.c.
  dsffm.c produces a pd object that manages parameter
  changes at signal rate, enabling the creation of 
  complex spectra with little effort.



  \see James A. Moorer, "The Synthesis of Complex Audio Spectra by Means of Discrete Summation Formulas", 1976
  \see https://ccrma.stanford.edu/files/papers/stanm5.pdf
  \see Tim Stilson,  Julius Smith, "Alias-Free Digital Synthesis of Classic Analog Waveforms", 1996
  \see https://ccrma.stanford.edu/~stilti/papers/blit.pdf
  */

#include "dsffm.h"
#include "dsflib.h"
#include "dsf.h"

void dsffm_set_fundamental(dsf *x, float frequency)
{
    set_increment_to_freq(x->increment_a, frequency, x->sr_inv); 
}

void dsffm_run(dsf *x, 
        INPRECISION *in1, INPRECISION *in2, INPRECISION *in3,
        OUTPRECISION *out1, OUTPRECISION *out2, 
        int vector_size)
{
    for(int i = 0; i < vector_size; i++) { 

        // process all inputs:
        float freq = mtof(in1[i]); 
        dsf_set_frequency(x, freq); 
        dsf_set_ratio(x, in2[i] + 1.0); 
        float weight = clip(0.0, 1.5, x->weight + in3[i]); 
        dsf_set_weight(x, weight);


        multiply_complex(x->phasor_a, x->increment_a, x->phasor_a); 
        multiply_complex(x->phasor_b, x->increment_b, x->phasor_b); 

        complex_nr result; 
        geometric_series(x, &result); 

        INPRECISION norm = norm_factor(x->weight, x->num_of_sines); 
        out1[i] = result.re * norm;
        out2[i] = result.im * norm;
    } 
}
