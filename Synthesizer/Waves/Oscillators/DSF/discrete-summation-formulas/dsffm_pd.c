/**
  \file dsffm_pd.c
  \author Johannes Zmoelnig, edited by Sebastian Zimmermann
  \date 2001-2006, 2020
  \brief This file creates the interface for the pure data dsffm object

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
#include "dsffm.h"

/**
 * define a new "class" 
 */
static t_class *dsffm_tilde_class;


/**
 * this is the dataspace of our new object
 * the first element is the mandatory "t_object"
 * f_dsf denotes the mixing-factor
 * "f" is a dummy and is used to be able to send floats AS signals.
 */
typedef struct _dsffm_tilde {
  t_object  x_obj;
  t_sample f_dsf;
  t_sample f;

  dsf *dsf;

  t_inlet *x_in2;
  t_inlet *x_in3;
  t_outlet *x_out1;
  t_outlet *x_out2;
} t_dsffm_tilde;


/**
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
t_int *dsffm_tilde_perform(t_int *w)
{
  /* the first element is a pointer to the dataspace of this object */
  t_dsffm_tilde *x = (t_dsffm_tilde *)(w[1]);

  /* TODO: */
  /* in x->dsf is our actual data structure! */ 

  /* here is a pointer to the t_sample arrays that hold the 2 input signals */
  t_sample  *in1 =    (t_sample *)(w[2]);
  t_sample  *in2 =    (t_sample *)(w[3]);
  t_sample  *in3 =    (t_sample *)(w[4]);
  /* here comes the signalblock that will hold the output signal */
  t_sample  *out1 =    (t_sample *)(w[5]);
  t_sample  *out2 =    (t_sample *)(w[6]);
  /* all signalblocks are of the same length */
  int          n =           (int)(w[7]);

  dsffm_run(x->dsf, in1, in2, in3, out1, out2, n);


  return (w+8);
}


/**
 * register a special perform-routine at the dsp-engine
 * this function gets called whenever the DSP is turned ON
 * the name of this function is registered in dsffm_tilde_setup()
 */
void dsffm_tilde_dsp(t_dsffm_tilde *x, t_signal **sp)
{
  /* add dsffm_tilde_perform() to the DSP-tree;
   * the dsffm_tilde_perform() will expect "5" arguments (packed into an
   * t_int-array), which are:
   * the objects data-space, 3 signal vectors (which happen to be
   * 2 input signals and 1 output signal) and the length of the
   * signal vectors (all vectors are of the same length)
   */

  x->dsf->sr = sys_getsr();
  x->dsf->sr_inv = ((INPRECISION) 1.0) / sys_getsr();
  dsp_add(dsffm_tilde_perform, 7, x,
          sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
}

/**
 * this is the "destructor" of the class;
 * it allows us to free dynamically allocated ressources
 */
void dsffm_tilde_free(t_dsffm_tilde *x)
{
  dsf_free(x->dsf);
  /* free any ressources associated with the given inlet */
  inlet_free(x->x_in2);
  inlet_free(x->x_in3);

  /* free any ressources associated with the given outlet */
  outlet_free(x->x_out1);
  outlet_free(x->x_out2);
}

/**
 * this is the "constructor" of the class
 * the argument is the initial mixing-factor
 */
void *dsffm_tilde_new(t_floatarg f)
{
  t_dsffm_tilde *x = (t_dsffm_tilde *)pd_new(dsffm_tilde_class);

  /* save the mixing factor in our dataspace */
  x->f_dsf = f;


  x->dsf = dsf_new();
  
  /* create a new signal-inlet */
  x->x_in2 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

  /* create a new inlet*/
  x->x_in3 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

  /* create a new signal-outlet */
  x->x_out1 = outlet_new(&x->x_obj, &s_signal);
  x->x_out2 = outlet_new(&x->x_obj, &s_signal);

  return (void *)x;
}


void dsffm_tilde_set_fundamental(t_dsffm_tilde *x, float frequency) {
    dsffm_set_fundamental(x->dsf, frequency);
}

void dsffm_tilde_set_weight(t_dsffm_tilde *x, float weight) {
    dsf_set_weight(x->dsf, weight); 
}

void dsffm_tilde_set_num_of_sines(t_dsffm_tilde *x, float num_of_sines) {
    dsf_set_num_of_sines(x->dsf, (int)num_of_sines); 
}


/**
 * define the function-space of the class
 * within a single-object external the name of this function is very special
 */
void dsffm_tilde_setup(void) {
  dsffm_tilde_class = class_new(gensym("dsffm~"),
        (t_newmethod)dsffm_tilde_new,
        (t_method)dsffm_tilde_free,
	sizeof(t_dsffm_tilde),
        CLASS_DEFAULT, 
        A_DEFFLOAT, 0);

  /* whenever the audio-engine is turned on, the "dsffm_tilde_dsp()" 
   * function will get called
   */
  class_addmethod(dsffm_tilde_class,
        (t_method)dsffm_tilde_dsp, gensym("dsp"), 0); 

  class_addmethod(dsffm_tilde_class,
          (t_method)dsffm_tilde_set_fundamental, gensym("fundamental"), A_DEFFLOAT, 0);

  class_addmethod(dsffm_tilde_class,
          (t_method)dsffm_tilde_set_num_of_sines, gensym("partials"), A_DEFFLOAT, 0);

  class_addmethod(dsffm_tilde_class,
          (t_method)dsffm_tilde_set_weight, gensym("weight"), A_DEFFLOAT, 0);

  /* if no signal is connected to the first inlet, we can as well 
   * connect a number box to it and use it as "signal"
   */
  CLASS_MAINSIGNALIN(dsffm_tilde_class, t_dsffm_tilde, f);
}
