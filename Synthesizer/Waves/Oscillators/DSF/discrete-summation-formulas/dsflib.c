
/**
  \file dsflib.c
  \author Sebastian Zimmermann
  \date September 2020
  \brief contains the main functionality of the dsf synthesis.

  \see James A. Moorer, "The Synthesis of Complex Audio Spectra by Means of Discrete Summation Formulas", 1976
  \see https://ccrma.stanford.edu/files/papers/stanm5.pdf
  \see Tim Stilson,  Julius Smith, "Alias-Free Digital Synthesis of Classic Analog Waveforms", 1996
  \see https://ccrma.stanford.edu/~stilti/papers/blit.pdf
  */

#include "dsflib.h"



/**
  \brief initialize a complex_nr 

  Allocates necessary memory and sets the 
  complex number to a degree so that it could
  be used as an increment to generate an oscillation
  at 440 Hz. This enables fast sanity checking.

  \return Pointer to initialized complex_nr
  */
complex_nr *new_complex_nr()
{
    complex_nr *new_nr = (complex_nr *)malloc(sizeof(complex_nr));
    new_nr->im = 0.03141;
    new_nr->re = 0.999506;

    adjust_phasor(new_nr);

    return new_nr;
} 


/**
  \brief initialize dsf memory

  The dsf struct holds all variables necessary for
  calculating the dsf.

  \return Pointer to allocated and initialized memory.
  \see dsf
  */ 
dsf *dsf_new()
{
    dsf *x = (dsf *)malloc(sizeof(dsf));

    x->phasor_a = new_complex_nr();
    x->increment_a = new_complex_nr();
    x->phasor_b = new_complex_nr();
    x->increment_b = new_complex_nr();


    x->weight = 0.5;
    x->usr_num_of_sines = 90;
    x->num_of_sines = 90;
    x->frequency = 220;
    x->distance = 220;

    return x; 
}


/**
  \brief free dsf struct memory
  */
void dsf_free(dsf *x)
{
    free(x->increment_a);
    free(x->phasor_a);
    free(x->increment_b);
    free(x->phasor_b);

    free(x);
} 


/**
  \brief get the smaller value of two integers
  \param a first value
  \param b second value
  \return smaller of a and b
  */
int min(int a, int b)
{
    int x = a < b ? a : b;
    return x; 
}

/**
  \brief clip float value between a min and a max value
  \param min Minimal value
  \param max Maximal value
  \param signal Signal to be clipped
  \return clipped signal
  */ 
float clip(float min, float max, float signal)
{
    float out = signal;

    if(signal < min)
    {
        out = min;
    }

    if(signal > max)
    {
        out = max;
    }

    return out;
}


/**
  \brief convert midi note values to frequencies

  \param note midi note value
  \return corresponding frequency
  */
float mtof(float note)
{
    float frequency = pow(2.0, (note - 69.0) / 12.0) * 440; 
    return frequency;
}


/**
  \brief set angle of increment to frequency

  Rotating phasors are being rotated by multiplications with
  a complex number called increment. set_increment_to_freq()
  sets the angle of
  an increment with respect to the sample frequency
  so that those multiplications produce
  the correct oscillation frequency of the rotating phasor.

  \param increment The complex number that is to be adjusted
  for the given frequency
  \param freq The frequency to which the increment is to be set
  \param sr_inv The inverted sample frequency 

  */
void set_increment_to_freq(complex_nr* increment, INPRECISION freq, INPRECISION sr_inv)
{
    // multiply by 2.0 to get fraction of Nyquist frequency
    double argument = (freq * sr_inv * 2.0) * PI; 
    set_phasor_to_argument(increment, argument);
}


/**
  \brief set angle of a phasor

  \param phasor The phasor whose angle is to be set
  \param argument The angle to which to set the phasor to.
  The range \f$[0 \dots 2\pi]\f$ describes a full rotation.
  */
void set_phasor_to_argument(complex_nr *phasor, double argument)
{
    phasor->im = sin(argument);
    phasor->re = cos(argument);
} 


/**
  \brief multiplying two complex numbers

  \param a Factor a
  \param b Factor b
  \param result Memory to which the result is being written
  */
void multiply_complex(complex_nr *a, complex_nr *b, complex_nr *result)
{
    INPRECISION re = (a->re * b->re) - (a->im * b->im);
    INPRECISION im = (a->re * b->im) + (a->im * b->re);
    result->re = re;
    result->im = im; 
}


/**
  \brief divide complex numbers

  \param numerator Numerator of division
  \param denominator Denominator of division
  \param result Pointer to memory where result of division is written to
  */
void divide_complex(complex_nr *numerator, complex_nr *denominator, complex_nr *result)
{ 
    complex_nr conjugate;
    conjugate.im = -denominator->im;
    conjugate.re = denominator->re;

    complex_nr expanded_num;
    multiply_complex(numerator, &conjugate, &expanded_num);

    complex_nr expanded_denom;
    multiply_complex(denominator, &conjugate, &expanded_denom); 

    result->im = expanded_num.im / expanded_denom.re;
    result->re = expanded_num.re / expanded_denom.re; 
}


/**
  \brief adjust phasor degeneration caused by digital noise

  Due to finite machine precision, over time phasors might
  diverge from the unit circle. In certain intervals
  adjust_phasor(complex_nr *phasor) is being called to 
  re-adjust the length of a phasor to 1.

  \param phasor Phasor to be adjusted.
  */
void adjust_phasor(complex_nr *phasor)
{
    double length = sqrt(phasor->im * phasor->im + phasor->re * phasor->re);
    double adjustment = 1.0 / length;
    phasor->im *= adjustment;
    phasor->re *= adjustment;
}


/**
  \brief calculate power of complex number

  \param x Base
  \param power Exponent
  \param result Pointer to memory where to write the result to
  */ 
void power_complex(complex_nr *x, int power, complex_nr *result)
{
    // stop recursion:
    if(power <= 1)
    {
        result->im = x->im;
        result->re = x->re; 
    }
    else
    {
        complex_nr intermediate;
        power_complex(x, power/2, &intermediate);

        multiply_complex(&intermediate, &intermediate, result);

        if(power % 2)
        {
            // uneven exponent: multiply result one more time with x
            multiply_complex(x, result, result); 
        } 
    } 
}


/**
  \brief calculate power of complex number

  This function is computationally very expensive.
  \param x Base
  \param power Exponent
  \param result Pointer to memory where to write the result to
  */ 
void power_complex_naiv(complex_nr *x, int power, complex_nr *result)
{
    result->im = x->im;
    result->re = x->re;
    for(int i = 1; i < power; i++)
    {
        multiply_complex(x, result, result);
    } 
}


/**
  \brief calculate geometric series by addition

  This function performs the actual addition of
  the geometric series and is thus computationally
  expensive.

  \param x dsf variables
  \param result Pointer to memory where to write the sum
  of the geometric series to
  \param norm_factor Pointer to memory where to store the
  maximal possible value of the sum to. This can be used to
  normalize the signal
  */
void calculate_series(dsf *x, complex_nr *result, double *norm_factor)
{
    INPRECISION powered_weight;

    complex_nr running_sum;
    complex_nr powered_b;

    // w^0 * b^0
    powered_b.im = 0.0;
    powered_b.re = 1.0; 

    running_sum.im = 0.0;
    running_sum.re = 1.0;

    powered_weight = 1.0;
    *norm_factor = 1.0;

    // increasing powers
    for(int i = 1; i < x->num_of_sines; i++)
    {
        // b^i
        multiply_complex(x->phasor_b, &powered_b, &powered_b); 
        // w^i
        powered_weight *= x->weight;

        // track norm factor
        *norm_factor += powered_weight;

        // add w^i * b^i to sum
        running_sum.im += powered_weight * powered_b.im;
        running_sum.re += powered_weight * powered_b.re; 
    } 

    multiply_complex(&running_sum, x->phasor_a, result); 
    // track length for phasor a:
    *norm_factor += 1.0; 
}


/**
  \brief calculate \f$ 1 - x, x \in \mathbb{C} \f$

  \param x Number to substract from 1
  \param result Pointer to memory where to write the result to
  */
void one_minus_complex(complex_nr *x, complex_nr *result)
{
    result->im = 0.0 - x->im;
    result->re = 1.0 - x->re; 
}


/**
  \brief calculate \f$ a \cdot x, x \in \mathbb{C} \f$

  \param x Complex number to scale
  \param scalar Scalar to scale x with 
  \param result Pointer to memory where to write the result to
  */
void scale_complex(complex_nr *x, double scalar, complex_nr *result)
{ 
    result->im = scalar * x->im;
    result->re = scalar * x->re; 
}


/**
  \brief calculate denominator of closed form of geometric series

  Closed form of geometric series is 
  \f$ a \cdot \frac{1 - (wb)^n}{1 - wb} \f$.
  This function calculates the denominator \f$ 1 - wb \f$.

  \param x dsf variables
  \param result Pointer to memory where to write the result to
  */
void geometric_series_denominator(dsf *x, complex_nr *result)
{
    // calculating (1 - w * b)
    // where w is x->weight
    // and b is current position of distance phasor x->phasor_b

    double w = x->weight;

    complex_nr wb;
    scale_complex(x->phasor_b, w, &wb); 

    one_minus_complex(&wb, result); 
}


/**
  \brief calculate numerator of closed form of geometric series

  Closed form of geometric series is 
  \f$ a \cdot \frac{1 - (wb)^n}{1 - wb} \f$.
  This function calculates the numerator \f$ 1 - (wb)^n \f$.

  \param x dsf variables
  \param result Pointer to memory where to write the result to
  */
void geometric_series_numerator(dsf *x, complex_nr *result)
{ 
    // calculating (1 - (w * b)^n) = (1 - (w^n * b^n))
    // where w is x->weight,
    // b is current position of distance phasor x->phasor_b,
    // and n is number of sines (or number of sines + 1?)
    int n = x->num_of_sines;
    double wn = pow(x->weight, n);

    complex_nr bn;
    power_complex(x->phasor_b, n, &bn);

    complex_nr wnbn;
    scale_complex(&bn, wn, &wnbn);

    one_minus_complex(&wnbn, result); 
}


/**
  \brief calculate factor of closed form of geometric series

  Closed form of geometric series is 
  \f$ a \cdot \frac{1 - (wb)^n}{1 - wb} \f$.
  This function calculates the factor \f$ \frac{1 - (wb)^n}{1 - wb} \f$.

  \param x dsf variables
  \param result Pointer to memory where to write the result to
  */
void geometric_series_factor(dsf *x, complex_nr *result)
{
    // calculating (1 - (wb)^k) / (1 - wb)
    complex_nr numerator;
    geometric_series_numerator(x, &numerator);

    complex_nr denominator;
    geometric_series_denominator(x, &denominator); 

    divide_complex(&numerator, &denominator, result);
}


/**
  \brief calculate closed form of geometric series

  Closed form of geometric series is 
  \f$ a \cdot \frac{1 - (wb)^n}{1 - wb} \f$.

  \param x dsf variables
  \param result Pointer to memory where to write the result to
  */
void geometric_series(dsf *x, complex_nr *result)
{
    complex_nr factor;

    if(x->phasor_b->re == 1 && x->weight == 1)
    {
        // following rule of l'hopital for "0/0":
        factor.im = 0;
        factor.re = 1;
    }
    else
    {
        geometric_series_factor(x, &factor); 
    }

    multiply_complex(x->phasor_a, &factor, result); 
}


/**
  \brief calculate maximal value of geometric series

  use the result to normalise a signal that was calculated
  with the geometric series.

  \param len This is the weight factor of the dsf synthesis
  \param num_of_sines Number of partials in synthesis
  \returns A scalar x that normalizes the signal s generated
  by the geometric series when applied like so: 
  \f$ s_{norm} = s \cdot x\f$.
  */ 
INPRECISION norm_factor(INPRECISION len, int num_of_sines)
{
    INPRECISION norm_factor = 0;
    if(len == 1.0)
    {
        // l'hopital "0/0":
        norm_factor = 1.0 / num_of_sines;
    }
    else
    { 
        norm_factor = (1.0 - len) / (1.0 - pow(len, num_of_sines));
    }

    return norm_factor; 
}


