/**
  \file dsf_pd.c
  \author Johannes Zmoelnig, edited by Sebastian Zimmermann
  \date 2001-2006, 2020
  \brief This file creates the interface for the pure data dsf object

  Interface functionality is implemented in this
  file, while actual logic is found in files
  dsf.c, dsffm.c and dsflib.c.


  \see https://github.com/pure-data/externals-howto/tree/master/example4


 */


/**
 * include the interface to Pd 
 */
#include "m_pd.h"
#include "dsf.h"

/**
 * define a new "class" 
 */
static t_class *dsf_tilde_class;


/**
   \brief dataspace of new pd object 

   t_dsf_tilde holds pointers to the dsf variables,
   and two outlets for the calculated signal
   and its quadrature signal.
 */
typedef struct _dsf_tilde {
  t_object  x_obj;

  dsf *dsf;

  t_outlet *x_out1;
  t_outlet *x_out2;
} t_dsf_tilde;


/**
   \brief core performance routine

 * this is the core of the object
 * this perform-routine is called for each signal block
 * the name of this function is arbitrary and is registered to Pd in the 
 * dsf_tilde_dsp() function, each time the DSP is turned on
 *
 * the argument to this function is just a pointer within an array
 * we have to know for ourselves how many elements inthis array are
 * reserved for us (hint: we declare the number of used elements in the
 * dsf_tilde_dsp() at registration
 *
 * since all elements are of type "t_int" we have to cast them to whatever
 * we think is apropriate; "apropriate" is how we registered this function
 * in dsf_tilde_dsp()
 */
t_int *dsf_tilde_perform(t_int *w)
{
  /* the first element is a pointer to the dataspace of this object */
  t_dsf_tilde *x = (t_dsf_tilde *)(w[1]);

  /* in x->dsf is our actual data structure! */ 
  /* here come the signalblocks that will hold the output signal */
  t_sample  *out1 =    (t_sample *)(w[2]);
  t_sample  *out2 =    (t_sample *)(w[3]);
  /* all signalblocks are of the same length */
  int          n =           (int)(w[4]);


  dsf_run(x->dsf, out1, out2, n);

  /* return a pointer to the dataspace for the next dsp-object */ 
  return (w+5);
}


/**
   \brief initialise object on dsp-engine start-up

 * register a special perform-routine at the dsp-engine
 * this function gets called whenever the DSP is turned ON
 * the name of this function is registered in dsf_tilde_setup()
 */
void dsf_tilde_dsp(t_dsf_tilde *x, t_signal **sp)
{
  /* add dsf_tilde_perform() to the DSP-tree;
   * the dsf_tilde_perform() will expect "4" arguments (packed into an
   * t_int-array), which are:
   * the objects data-space, 2 signal vectors (which happen to be
   * 2 output signals) and the length of the
   * signal vectors (all vectors are of the same length)
   */

  x->dsf->sr = sys_getsr();
  x->dsf->sr_inv = ((INPRECISION) 1.0) / sys_getsr();

  dsf_set_frequency(x->dsf, x->dsf->frequency); 

  dsp_add(dsf_tilde_perform, 4, x,
          sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

/**
   \brief destructor of dataspace object

 * this is the "destructor" of the class;
 * it allows us to free dynamically allocated ressources
 */
void dsf_tilde_free(t_dsf_tilde *x)
{
  dsf_free(x->dsf);

  /* free any ressources associated with the given outlet */
  outlet_free(x->x_out1);
  outlet_free(x->x_out2);
}

/**
   \brief constructor of dataspace object

 * this is the "constructor" of the class.

    \param f Initial frequency of dsp module
    \param r Initial ratio of overtones to frequency
    \param n Initial number of partials from which to construct the sound
    \param w Initial weight of overtones
 * 
 */
void *dsf_tilde_new(t_floatarg f, t_floatarg r, t_floatarg n, t_floatarg w)
{
  t_dsf_tilde *x = (t_dsf_tilde *)pd_new(dsf_tilde_class); 

  x->dsf = dsf_new();

  f = f ? f : 220;
  r = r ? r : 1;
  n = n ? n : 100;
  w = w ? w : 0.8;

  post("init dsf with arguments\nfreq: %f\nratio: %f\npartials: %f\nweight: %f\n", f, r, n, w); 

  dsf_set_weight(x->dsf, w);
  dsf_set_num_of_sines(x->dsf, (int)n);
  dsf_set_fundamental(x->dsf, f);
  dsf_set_ratio(x->dsf, r);
  dsf_set_frequency(x->dsf, f);

  /* create a new signal-outlet */
  x->x_out1 = outlet_new(&x->x_obj, &s_signal);
  x->x_out2 = outlet_new(&x->x_obj, &s_signal);

  return (void *)x;
}


/**
  \brief setter method for increment phasor angle

  This is for testing only and sets the increment
  phasor to an angle given by the argument parameter.

  \param x pd object dataspace
  \param argument Angle to set the increment phasor to
  */
void dsf_tilde_set_argument(t_dsf_tilde *x, float argument) {
    set_phasor_to_argument(x->dsf->increment_a, (INPRECISION) argument);
}

/**
  \brief setter method for fundamental frequency

  Set the fundamental frequency of dsp module
  without altering the overtone spacing.

  \param x pd object dataspace
  \param frequency Frequency to set the fundamental to.
  */
void dsf_tilde_set_fundamental(t_dsf_tilde *x, float frequency) {
    dsf_set_fundamental(x->dsf, frequency);
}

/**
  \brief setter method for spacing of overtones

  Set the spacing of overtones of dsp module
  without altering the fundamental frequency.

  \param x pd object dataspace
  \param distance New spacing for overtones.
  */
void dsf_tilde_set_distance(t_dsf_tilde *x, float distance) {
    dsf_set_distance(x->dsf, distance);
}

/**
  \brief setter method for weight of overtones

  Set the weight x of overtones of dsp module.
  The nth overtone is weighted by \f$ x^n \f$.

  \param x pd object dataspace
  \param distance New weight for overtones.
  */
void dsf_tilde_set_weight(t_dsf_tilde *x, float weight) {
    dsf_set_weight(x->dsf, weight); 
}

/**
  \brief setter method for number of partials

  Set number of partials to create the sound
  with. The number is limited with regard to
  the sample rate and the current frequency
  to avoid aliasing.

  \param x pd object dataspace
  \param num_of_sines Number of partials to create
  sound from 
  */
void dsf_tilde_set_num_of_sines(t_dsf_tilde *x, float num_of_sines) {
    dsf_set_num_of_sines(x->dsf, (int)num_of_sines); 
}

/**
  \brief setter method for frequency

  Set new frequency and change overtone spacing
  accordingly to keep same sonic characteristics.

  \param x pd object dataspace
  \param frequency New frequency to set the dsp module to
  */
void dsf_tilde_set_frequency(t_dsf_tilde *x, float frequency) {
    dsf_set_frequency(x->dsf, frequency); 
}

/**
  \brief setter method for overtone ratio

  Set overtone spacing relative to fundamental
  frequency. New spacing is set to \f$ x \cdot f \f$,
  where x is ratio parameter and f is current 
  fundamental frequency.

  \param x pd object dataspace
  \param ratio New ratio for overtone spacing
  */
void dsf_tilde_set_ratio(t_dsf_tilde *x, float ratio) {
    dsf_set_ratio(x->dsf, ratio); 
}

/**
 * \brief define the function-space of the class
 */
void dsf_tilde_setup(void) {
  dsf_tilde_class = class_new(gensym("dsf~"),
        (t_newmethod)dsf_tilde_new,
        (t_method)dsf_tilde_free,
	sizeof(t_dsf_tilde),
        CLASS_DEFAULT, 
        A_DEFFLOAT, A_DEFFLOAT,
        A_DEFFLOAT, A_DEFFLOAT, 0);

  /* whenever the audio-engine is turned on, the "dsf_tilde_dsp()" 
   * function will get called
   */
  class_addmethod(dsf_tilde_class,
        (t_method)dsf_tilde_dsp, gensym("dsp"), 0);

  class_addmethod(dsf_tilde_class,
          (t_method)dsf_tilde_set_argument, gensym("argument"), A_DEFFLOAT, 0);

  class_addmethod(dsf_tilde_class,
          (t_method)dsf_tilde_set_fundamental, gensym("fundamental"), A_DEFFLOAT, 0);

  class_addmethod(dsf_tilde_class,
          (t_method)dsf_tilde_set_distance, gensym("distance"), A_DEFFLOAT, 0);

  class_addmethod(dsf_tilde_class,
          (t_method)dsf_tilde_set_weight, gensym("weight"), A_DEFFLOAT, 0);

  class_addmethod(dsf_tilde_class,
          (t_method)dsf_tilde_set_num_of_sines, gensym("partials"), A_DEFFLOAT, 0);

  class_addmethod(dsf_tilde_class,
          (t_method)dsf_tilde_set_frequency, gensym("frequency"), A_DEFFLOAT, 0);

  class_addmethod(dsf_tilde_class,
          (t_method)dsf_tilde_set_ratio, gensym("ratio"), A_DEFFLOAT, 0);
}
