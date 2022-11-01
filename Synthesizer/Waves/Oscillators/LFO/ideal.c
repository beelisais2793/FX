/*******************************************************************************
 *
 * Fundamental ideal oscillators.
 *
 * This header file contains the abstract type definition to instantiate
 * objects to pilot the phase of some of the common ideal oscillators.
 * The header also includes the generating functions for such oscillators.
 *
 * Copyright(C) 2020 Dario Sanfilippo – All rights reserved.
 *
 * Reference: Boulanger and Lazzarini 2010 – The Audio Programming Book.
 *
 ******************************************************************************/

#define TWOPI (M_PI * 2.0)

/* Define an abstract type that contains the fundamental variables
 * of an oscillator. */
typedef struct t_phasor {
    unsigned long SR;
    double phase;
    double freq;
    double incr;
    double current_ph;
} PH;

/* Creation and initialisation of an object of type "PH",
 * as defined above. It returns a pointer to a memory location of
 * appropriate size. */
PH* ph_new(unsigned long samplerate) {
    PH* ph;
    ph = (PH*) malloc(sizeof(PH));
    if (ph == NULL)
        return NULL;
    ph->SR = samplerate;
    ph->phase = 0.0;
    ph->freq = 0.0;
    ph->incr = 0.0;
    ph->current_ph = 0.0;
    return ph;
}

/* Function to free memory after the object is no longer needed. */
void ph_free(void* ph) {
    free(ph);
}

/* Sample-generating function for a phasor oscillator.
 * The function returns a new value representing the oscillation
 * at a required frequency whenever it is called.
 * The function performs operations on a "PH" object set of variables
 * that is passed as a pointer argument*/
double phasor_gen(PH* ph, double set_freq, double set_ph) {
    /* Var to store the state of the oscillator before increment. */
    double out;
    /* Set state to specified phase (in the [0; 1] range) if it varies. */
    if (ph->phase != set_ph) {
        ph->phase = set_ph;
        ph->current_ph = ph->phase;
    }
    /* Set output value to current state. */
    out = ph->current_ph;
    /* Set new frequency if it varies. */
    if (ph->freq != set_freq) {
        ph->freq = set_freq;
        ph->incr = ph->freq / (double) ph->SR;
    }
    /* Increment the phase of the oscillator. */
    ph->current_ph += ph->incr;
    /* Wrap-around if the phase exceeds boundaries. */
    if (ph->current_ph >= 1.0)
        ph->current_ph -= 1.0;
    if (ph->current_ph < 0.0)
        ph->current_ph += 1.0;
    return out;
}

/* Sample-generating function for a sinusoidal oscillator.
 * The function returns a new value representing the oscillation
 * at a required frequency whenever it is called.
 * The function performs operations on a "PH" object set of variables
 * that is passed as a pointer argument*/
double sine_gen(PH* ph, double set_freq, double set_ph) {
    /* Var to store the state of the oscillator before increment. */
    double out;
    /* Set state to specified phase (in the [0; 1] range) if it varies. */
    if (ph->phase != set_ph) {
        ph->phase = set_ph;
        ph->current_ph = ph->phase;
    }
    /* Set output value to current state. */
    out = sin(ph->current_ph * TWOPI);
    /* Set new frequency if it varies. */
    if (ph->freq != set_freq) {
        ph->freq = set_freq;
        ph->incr = ph->freq / (double) ph->SR;
    }
    /* Increment the phase of the oscillator. */
    ph->current_ph += ph->incr;
    /* Wrap-around if the phase exceeds boundaries. */
    if (ph->current_ph >= 1.0)
        ph->current_ph -= 1.0;
    if (ph->current_ph < 0.0)
        ph->current_ph += 1.0;
    return out;
}

/* Sample-generating function for a square oscillator.
 * The function returns a new value representing the oscillation
 * at a required frequency whenever it is called.
 * The function performs operations on a "PH" object set of variables
 * that is passed as a pointer argument*/
double square_gen(PH* ph, double set_freq, double set_ph) {
    /* Var to store the state of the oscillator before increment. */
    double out;
    /* Set state to specified phase (in the [0; 1] range) if it varies. */
    if (ph->phase != set_ph) {
        ph->phase = set_ph;
        ph->current_ph = ph->phase;
    }
    /* Set output value to current state. */
    if (ph->current_ph < 0.5)
        out = 1.0;
    else
        out = -1.0;
    /* Set new frequency if it varies. */
    if (ph->freq != set_freq) {
        ph->freq = set_freq;
        ph->incr = ph->freq / (double) ph->SR;
    }
    /* Increment the phase of the oscillator. */
    ph->current_ph += ph->incr;
    /* Wrap-around if the phase exceeds boundaries. */
    if (ph->current_ph >= 1.0)
        ph->current_ph -= 1.0;
    if (ph->current_ph < 0.0)
        ph->current_ph += 1.0;
    return out;
}

/* Sample-generating function for a sawtooth oscillator.
 * The function returns a new value representing the oscillation
 * at a required frequency whenever it is called.
 * The function performs operations on a "PH" object set of variables
 * that is passed as a pointer argument*/
double sawtooth_gen(PH* ph, double set_freq, double set_ph) {
    /* Var to store the state of the oscillator before increment. */
    double out;
    /* Set state to specified phase (in the [0; 1] range) if it varies. */
    if (ph->phase != set_ph) {
        ph->phase = set_ph;
        ph->current_ph = ph->phase;
    }
    /* Set output value to current state. */
    out = ph->current_ph * 2.0 - 1.0;
    /* Set new frequency if it varies. */
    if (ph->freq != set_freq) {
        ph->freq = set_freq;
        ph->incr = ph->freq / (double) ph->SR;
    }
    /* Increment the phase of the oscillator. */
    ph->current_ph += ph->incr;
    /* Wrap-around if the phase exceeds boundaries. */
    if (ph->current_ph >= 1.0)
        ph->current_ph -= 1.0;
    if (ph->current_ph < 0.0)
        ph->current_ph += 1.0;
    return out;
}

/* Sample-generating function for a triangle oscillator.
 * The function returns a new value representing the oscillation
 * at a required frequency whenever it is called.
 * The function performs operations on a "PH" object set of variables
 * that is passed as a pointer argument*/
double triangle_gen(PH* ph, double set_freq, double set_ph) {
    /* Var to store the state of the oscillator before increment. */
    double out;
    /* Set state to specified phase (in the [0; 1] range) if it varies. */
    if (ph->phase != set_ph) {
        ph->phase = set_ph;
        ph->current_ph = ph->phase;
    }
    /* Set output value to current state. */
    out = ph->current_ph * 2.0 - 1.0;
    if (out < 0.0)
        out = -out;
    out = 2.0 * out - 1.0;
    /* Set new frequency if it varies. */
    if (ph->freq != set_freq) {
        ph->freq = set_freq;
        ph->incr = ph->freq / (double) ph->SR;
    }
    /* Increment the phase of the oscillator. */
    ph->current_ph += ph->incr;
    /* Wrap-around if the phase exceeds boundaries. */
    if (ph->current_ph >= 1.0)
        ph->current_ph -= 1.0;
    if (ph->current_ph < 0.0)
        ph->current_ph += 1.0;
    return out;
}