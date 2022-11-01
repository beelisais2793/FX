#ifndef GRAIN_SOURCE_H
#define GRAIN_SOURCE_H
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pdf.h"

#define rand_double() ((double)rand() / (double)RAND_MAX)



typedef enum property_type_enum
{ EX_NONE,
  EX_POSITION_2D,
  EX_POSITION_3D
}
property_type;

typedef enum envelope_enum
{ ENV_LINEAR, ENV_EXP }
envelope;

typedef struct ex_position_2d_struct
{
  int x, y;
}
ex_position_2d;

typedef struct ex_position_3d_struct
{
  int x, y, z;
}
ex_position_3d;



typedef struct grain_source_struct
{
  int id;
  double probability;
  unsigned char *wave_data;
  int n_samples;
  
  pdf *pitch_pdf, *position_pdf, *pan_pdf;

  double width, position;
  int scaled_position;
  int scaled_width;
  distribution distribution_type; 
  void *extended_properties;
  property_type extended_type;
}
grain_source;


typedef struct grain_struct
{
  int offset;
  grain_source *source;
  int position;
  int pitch;
  int pan;
  int amplitude;
}
grain;


grain_source *create_uniform_source (int id, unsigned char *wave_form,
				     int n_samples);
grain_source *create_source (int id, unsigned char *wave_form, int n_samples,
			     distribution type, double width,
			     double position);

void set_probability (grain_source * source, double probability);
double get_probability (grain_source * source);
int get_id (grain_source * source);

void set_position_normalized (grain_source * source, double position,
			      double width);
void set_distribution_normalized (grain_source * source, double position,
				  double width, distribution type);

void set_position (grain_source * source, int position, int width);
void set_distribution (grain_source * source, int position,
		       int width, distribution type);


void set_pitch_distribution(grain_source *source, double position, double width, distribution type);
void set_pan_distribution(grain_source *source, double position, double width, distribution type);


void set_extended_properties (grain_source * source,
			      property_type type, void *properties);
property_type get_extended_type (grain_source * source);
void *get_extended_properties (grain_source * source);

void draw_grain (grain_source * source, grain * data, int grain_length);
unsigned char *get_wave_ptr (grain_source * source);

int get_source_length (grain_source * source);
void resample_wave(grain_source *source, int sample_factor);
#endif
