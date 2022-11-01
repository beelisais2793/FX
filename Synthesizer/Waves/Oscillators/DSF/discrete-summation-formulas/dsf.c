/**
  \file dsf.c
  \author Sebastian Zimmermann
  \date September 2020
  \brief wrapping dsflib-functions to user-friendly pd object

  Main functionality is found in dsflib.c.
  dsf.c produces a pd object that manages parameter
  changes at control rate via messages.


  \see James A. Moorer, "The Synthesis of Complex Audio Spectra by Means of Discrete Summation Formulas", 1976
  \see https://ccrma.stanford.edu/files/papers/stanm5.pdf
  \see Tim Stilson,  Julius Smith, "Alias-Free Digital Synthesis of Classic Analog Waveforms", 1996
  \see https://ccrma.stanford.edu/~stilti/papers/blit.pdf
  */

#include "dsf.h"
#include "dsflib.h"



/**
  \brief set frequency of dsf

  Sets fundamental and distances relative to a new frequency.
  This keeps the overtone structure and corresponds with
  setting the oscillating frequency of a conventional oscillator.

  \param x dsf variables
  \param frequency Frequency to set the dsf to
  */ 
void dsf_set_frequency(dsf *x, float frequency)
{
    frequency = fabs(frequency);
    if(frequency != 0.0)
    { 
        float ratio = frequency / x->frequency;
        float new_distance = x->distance * ratio;
        dsf_set_fundamental(x, frequency);
        dsf_set_distance(x, new_distance); 
    }
}

/**
  \brief set ratio of overtones

  All overtones are spaced with equal distance.
  This function derives that distance as a ratio
  from the fundamental frequency.

  \param x dsf variables
  \param ratio Ratio of fundamental frequency to
  overtone spacing
  \see dsf_set_fundamental()
  \see dsf_set_distance()
  */ 
void dsf_set_ratio(dsf *x, float ratio)
{
    float new_distance = ratio * x->frequency; 
    dsf_set_distance(x, new_distance); 
}


/**
  \brief set fundamental frequency

  This sets the fundamental frequency without changing
  the distance between the overtones.
  
  \param x dsf variables
  \param frequency New fundamental frequency
  \see dsf_set_ratio()
  \see dsf_set_frequency()
  */
void dsf_set_fundamental(dsf *x, float frequency)
{
    frequency = fabs(frequency);
    x->frequency = frequency; 
    // new frequency: adjust number of generated sines
    // to avoid aliasing by increased frequency
    dsf_set_num_of_sines(x, x->usr_num_of_sines); 

    set_increment_to_freq(x->increment_a, x->frequency, x->sr_inv);

    adjust_phasor(x->phasor_a);
    adjust_phasor(x->increment_a);
}


/**
  \brief set distance between overtones

  This sets the distance between overtones
  without changing the fundamental frequency.  
  
  \param x dsf variables
  \param distance New distance between overtones
  \see dsf_set_ratio()
  \see dsf_set_frequency()
  */
void dsf_set_distance(dsf *x, float distance)
{ 
    distance = fabs(distance);
    x->distance = distance; 
    // new distance: adjust number of generated sines
    // to avoid aliasing by increased gaps between sines
    dsf_set_num_of_sines(x, x->usr_num_of_sines); 

    set_increment_to_freq(x->increment_b, x->distance, x->sr_inv);

    adjust_phasor(x->phasor_b);
    adjust_phasor(x->increment_b);
}


/**
  \brief set diminishing factor of overtones

  The overtone series is tailing of by a weight
  factor \f$ w \f$, where the nth overtone is scaled by
  \f$ w^n \f$.

  \param x dsf variables
  \param weight New value for \f$ w \f$
  */ 
void dsf_set_weight(dsf *x, float weight)
{
    weight = clip(-0.5, 1.5, weight);
    x->weight = weight; 
}

/**
  \brief set total number of partials to generate

  This function takes user input x and 
  calculates the maximum number of 
  partials y that can be produced without aliasing
  and sets the number of actually produced partials
  to the lower of x and y.

  \param x dsf variables
  \param num_of_sines Number of partials to produce
 */ 
void dsf_set_num_of_sines(dsf *x, int num_of_sines)
{
    num_of_sines = num_of_sines > 0 ? num_of_sines : 1;
    x->usr_num_of_sines = num_of_sines;
    if(x->distance)
    {
        int max_num_of_sines = (int) (((x->sr / 2.0) - x->frequency) / x->distance) + 1;
        x->num_of_sines = min(max_num_of_sines, num_of_sines);
    }
    else
    {
        // x->distance == 0: only one frequency
        x->num_of_sines = 1; 
    } 
} 


/**
  \brief calculate audio signal

  This function is called for every block of audio
  that the host needs to generate.
  For every sample it updates the phasors by
  multiplication with the respective increment phasor,
  so that the dsf synthesis keeps track of two
  oscillations: one for the fundamental frequency,
  one for the overtones.

  Those two phasors are then put together in a
  geometric series which is calculated in closed
  form. The result is normalized and written sample
  by sample in the provided output buffer.

  One output buffer is filled with the real part
  of the signal, the other one with the imaginary
  part, creating two signals in quadrature.

  \param x dsf variables
  \param out1 Pointer to output buffer for real part of signal
  \param out2 Pointer to output buffer for imaginary part of signal
  \param vector_size Size of output buffer

  */ 
void dsf_run(dsf *x, OUTPRECISION *out1, OUTPRECISION *out2, int vector_size)
{
    for(int i = 0; i < vector_size; i++) { 

        multiply_complex(x->phasor_a, x->increment_a, x->phasor_a);
        multiply_complex(x->phasor_b, x->increment_b, x->phasor_b);

        complex_nr result; 
        geometric_series(x, &result); 


        INPRECISION norm = norm_factor(x->weight, x->num_of_sines); 
        out1[i] = result.re * norm;
        out2[i] = result.im * norm;
    } 
}

