#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "pdf.h"
#define rand_double() ((double)rand() / (double)RAND_MAX)

double rand_gaussian ()
{
  static int precomputed_next = 0;
  static double next = 0.0;
  double multiplier;
  if (precomputed_next)
    {
      precomputed_next = 0;
      return next;
    }
  else
    {
      double v1, v2, s;
      do
	{
	  v1 = 2 * rand_double () - 1;
	  v2 = 2 * rand_double () - 1;
	  s = v1 * v1 + v2 * v2;
	}
      while (s >= 1 || s == 0);

      multiplier = sqrt (-2 * log (s) / s);
      next = v2 * multiplier;
      precomputed_next = 1;
      return v1 * multiplier;
    }
}

pdf *create_pdf(int max_mixtures)
{
  pdf *pdf_data;
  pdf_data = calloc(sizeof(*pdf_data), 1);
  assert(pdf_data!=NULL);
  pdf_data->max_mixtures = max_mixtures;
  pdf_data->widths = calloc(sizeof(*(pdf_data->widths)), max_mixtures);
  pdf_data->centres = calloc(sizeof(*(pdf_data->centres)), max_mixtures);
  pdf_data->types = calloc(sizeof(*(pdf_data->types)), max_mixtures);
  assert(pdf_data->widths!=NULL);
  assert(pdf_data->centres!=NULL);
  assert(pdf_data->types!=NULL);  
  pdf_data->n_mixtures = 0;
  return pdf_data;
}

double draw_sample(pdf *pdf_data)
{
  double rand_var;
  if(pdf_data->n_mixtures==1)
    {
      switch (pdf_data->types[0])
	{
	case D_UNIFORM_FORWARD:
	  rand_var = fabs ((rand_double ()) - 0.5);
	  break;
	case D_UNIFORM_BACKWARD:
	  rand_var = -(fabs ((rand_double ()) - 0.5));
	  break;
	case D_GAUSSIAN:
	  rand_var = rand_gaussian ();
	  break;
	case D_GAUSSIAN_FORWARD:
	  rand_var = fabs (rand_gaussian ());
	  break;
	case D_GAUSSIAN_BACKWARD:
	  rand_var = -(fabs (rand_gaussian ()));
	  break;
	case D_SPIKE:
	  rand_var = 0.0;
	  break;
	default:
	case D_UNIFORM:
	  rand_var = (rand_double ()) - 0.5;
	  break;
	  break;
	}
      rand_var *= pdf_data->widths[0];
      rand_var += pdf_data->centres[0];
      return rand_var;
    }
  else
    return  0.0; //NYI
}

void set_pdf_single(pdf *pdf_data, distribution type, double centre, double width)
{
  pdf_data->n_mixtures = 1;
  pdf_data->centres[0] = centre;
  pdf_data->widths[0] = width;
  pdf_data->types[0] = type;
}

void set_pdf(pdf *pdf_data, int n_mixtures, distribution *types, double *centres, double *widths)
{
  pdf_data->n_mixtures = n_mixtures;
  pdf_data->types = types;
  pdf_data->centres = centres;
  pdf_data->widths = widths;
}
