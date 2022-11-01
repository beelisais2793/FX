#include "grain_source.h"
#include <assert.h>




int get_id (grain_source * source)
{
  return source->id;
}




void set_position_normalized (grain_source * source,
			      double position, double width)
{
  set_distribution (source, position, width, source->distribution_type);
}

void set_distribution_normalized (grain_source * source, double position,
				  double width, distribution type)
{
  source->distribution_type = type;
  source->width = width;
  source->position = position;
  source->scaled_position = (position * source->n_samples);
  source->scaled_position %= source->n_samples;
  source->scaled_width = width * source->n_samples;
  set_pdf_single(source->position_pdf, type, source->scaled_position, source->scaled_width);
}

void set_position (grain_source * source, int position, int width)
{
  set_distribution (source, position, width, source->distribution_type);
}

void set_pitch_distribution (grain_source * source, double position,
			     double width, distribution type)
{
  set_pdf_single(source->pitch_pdf, type, position, width);
}

void set_pan_distribution (grain_source * source, double position,
			     double width, distribution type)
{
  set_pdf_single(source->pan_pdf, type, position, width);
}

void set_distribution (grain_source * source, int position,
		       int width, distribution type)
{
  source->distribution_type = type;
  source->width = ((double) width / (double) (source->n_samples));
  source->position = ((double) position / (double) (source->n_samples));
  source->scaled_position = position % source->n_samples;
  source->scaled_width = width;
  set_pdf_single(source->position_pdf, type, source->scaled_position, source->scaled_width);
}

void set_probability (grain_source * source, double probability)
{
  source->probability = probability;
}

void draw_grain (grain_source * source, grain * data, int grain_length)
{
  data->amplitude = (source->probability * 0x100);
  data->source = source;
  data->position = 0;

  data->pitch = draw_sample(source->pitch_pdf) * 0x10000;
  data->pan = draw_sample(source->pan_pdf) * 0x80;

  if(data->pitch<1024)
    data->pitch = 1024;

  if(data->pan<0)
    data->pan = 0;

  data->offset = draw_sample(source->position_pdf);

  if (data->offset < 0)
    data->offset += source->n_samples;

  data->offset = data->offset % ((source->n_samples >> 1) - grain_length);
 
}

double get_probability (grain_source * source)
{
  

  return source->probability;
}

unsigned char *get_wave_ptr (grain_source * source)
{
  return source->wave_data;
}

void set_extended_properties (grain_source * source,
			      property_type type, void *properties)
{
  source->extended_properties = properties;
  source->extended_type = type;
}

property_type get_extended_type (grain_source * source)
{
  return source->extended_type;
}

void *get_extended_properties (grain_source * source)
{
  return source->extended_properties;
}

int get_source_length (grain_source * source)
{
  return source->n_samples;
}

void resample_wave(grain_source *source, int sample_factor)
{
  int i, j;
  int val;
  unsigned char *old_wave = source->wave_data;
  if(sample_factor<2)
    return;
  source->wave_data = calloc(sizeof(*(source->wave_data)), source->n_samples/sample_factor);
  printf("Resampling %d\n", sample_factor);
  assert(source->wave_data!=NULL);
  
  for(i=0;i<source->n_samples;i+=2*sample_factor)
    {
      val = 0;
      for(j=0;j<sample_factor;j++)
	val += (int)((old_wave[i + (j<<1)]) + (old_wave[i + 1 + (j<<1)] << 8)) - 0x8000;
      val /= sample_factor;
      val += 0x8000;
      source->wave_data[i/sample_factor] = val & 0xff;
      source->wave_data[(i/sample_factor)+1] = (val >> 8) & 0xff;
    }
  source->n_samples /= sample_factor;
  
  //free(old_wave);

}

grain_source *create_uniform_source (int id, unsigned char *wave_form,
				     int n_samples)
{
  return create_source (id, wave_form, n_samples, D_UNIFORM, 1.0, 0.0);
}

static void concatenate(grain_source *source)
{
  char *temp;
  int old_len;
  temp = calloc(sizeof(*(source->wave_data)), 32000);
  memcpy(temp, source->wave_data, source->n_samples);
  old_len = source->n_samples;
  while(source->n_samples<15000)
    {
      temp[source->n_samples] = source->wave_data[(source->n_samples) % old_len];
    source->n_samples++;
    }
  source->n_samples = old_len;
  source->wave_data = temp;
}

grain_source *create_source (int id, unsigned char *wave_form, int n_samples,
			     distribution type, double width, double position)
{
  grain_source *source = calloc (sizeof (grain_source), 1);
  if (source == NULL)
    return NULL;
  source->id = id;
  source->wave_data = wave_form;
  source->n_samples = n_samples;
  source->position_pdf = create_pdf(1);
  source->pitch_pdf = create_pdf(1);
  source->pan_pdf = create_pdf(1);
  
  set_pitch_distribution(source, 1.0, 0.0, D_SPIKE);
  set_pan_distribution(source, 0.5, 0.0, D_SPIKE);

  set_distribution_normalized (source, position, width, type);
  set_probability (source, 0.0);
  set_extended_properties (source, EX_NONE, NULL);
  /*if(source->n_samples<16000)
    concatenate(source);
  */
  return source;
}
