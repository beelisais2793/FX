#include "grain_engine.h"
#define LINEAR_INTERPOLATION
#define vol_red 0x14
static int n_channels = 0, n_bits = 0, sample_rate = 0;
static int n_sources = 0, n_grains = 0;
static int initialized = 0;
static grain_source **sources = NULL;
static grain *grains = NULL;
static int *grain_envelope = NULL;
static int grain_density = DEFAULT_GRAIN_DENSITY;
static int max_grains = DEFAULT_MAX_GRAINS;
static int grain_length = DEFAULT_GRAIN_LENGTH;
static double attack = DEFAULT_ATTACK;
static envelope envelope_type = DEFAULT_ENVELOPE_TYPE;
static int normalize = DEFAULT_NORMALIZE;
static int modulation = DEFAULT_MODULATION;
static int max_sources = DEFAULT_MAX_SOURCES;
static int sample_skip;
static double new_grain_probability;
static int buffer_length = DEFAULT_BUFFER_LENGTH;
static int *sum_buffer;

static double (*distribution_function) (grain_source *) = NULL;
static void (*time_function) (int) = NULL;

void set_format (int rate, int bits, int channels)
{
  sample_rate = rate;
  n_bits = bits;
  n_channels = channels;
  sample_skip = WAV_FILE_RATE / rate;
  new_grain_probability = (float) grain_density / (sample_rate);
}


void create_sources ()
{
  sources = calloc (sizeof (*sources), max_sources);
  assert (sources != NULL);
  n_sources = 0;
}

void create_grains ()
{
  grains = calloc (sizeof (*grains), max_grains);
  assert (grains != NULL);
  n_grains = 0;
}

void create_envelope ()
{
  int i = 0;
  double val;
  int fade_in_end = grain_length * attack;
  int fade_out_start = grain_length - fade_in_end;
  grain_envelope = calloc (sizeof (*grain_envelope), grain_length * 2);
  assert (grain_envelope != NULL);
  for (i = 0; i < grain_length; i++)
    {
      switch (envelope_type)
	{
	case ENV_EXP:
	  val = ((i - grain_length / 2)*(i-grain_length/2)) / (double) (grain_length*grain_length/20);
	  grain_envelope[i] = (int)(256.0 * exp (-val));
	  
	  break;
	case ENV_LINEAR:
	  grain_envelope[i] = 256;
	  break;
	}
      if(envelope_type == ENV_LINEAR)
	{
	  if (i < fade_in_end)
	    grain_envelope[i] *= (double) i / (double) fade_in_end;
	  else if (i > fade_out_start)
	    grain_envelope[i] *=
	      1.0 - ((double) (i - fade_out_start) / (double) fade_in_end);
	}
    }
}

void register_distribution_function (double (*distribution_func)
				     (grain_source * source))
{
  distribution_function = distribution_func;
}

void register_time_function(void (*time_func)(int))
{
  time_function = time_func;
}

void create_buffer ()
{
  sum_buffer = calloc (sizeof (*sum_buffer), buffer_length >> 1);
  assert (sum_buffer != NULL);
}

void set_buffer_length (int length)
{
  buffer_length = length;
  create_buffer ();
}

int initialize_engine ()
{
  if (n_channels < 1 || (n_bits != 8 && n_bits != 16) || sample_rate <= 0)
    return 0;

  if (distribution_function == NULL)
    return 0;

  create_envelope ();
  create_grains ();
  create_sources ();
  create_buffer ();
  initialized = 1;
  return 1;
}

void set_grain_length (int length)
{
  grain_length = length;
  create_envelope ();
}

void set_max_grains (int m_grains)
{
  max_grains = m_grains;
  if (initialized)
    create_grains ();
}

void set_grain_density (int gr_density)
{
  grain_density = gr_density;
  new_grain_probability = (float) grain_density / (sample_rate);
}

void set_envelope (envelope env_type, double attck)
{
  attack = attck;
  envelope_type = env_type;
  create_envelope ();
}

void set_normalize (int norm)
{
  normalize = norm;
}

void set_amplitude_modulation (int mod)
{
  modulation = mod;
}

void reallocate_sources (int new_sources)
{
  void *ret_ptr;
  if (new_sources <= max_sources)
    return;
  ret_ptr = realloc (sources, sizeof (*sources) * new_sources);
  assert (ret_ptr != NULL);
  max_sources = new_sources;
}


void add_source (grain_source * source)
{
  if (!initialized)
    return;
  if (n_sources + 1 == max_sources)
    reallocate_sources (max_sources * 2);
  sources[n_sources] = source;
  n_sources++;
}

void update_pdf ()
{
  int i;
  double sum = 0.0;
  if(time_function!=NULL)
    time_function(buffer_length);
  for (i = 0; i < n_sources; i++)
    {
      distribution_function (sources[i]);
      sum += get_probability(sources[i]);
    }
  if (normalize)
    for (i = 0; i < n_sources; i++)
      set_probability (sources[i], get_probability (sources[i]) / sum);
}

grain_source *new_grain_source ()
{
  int i;
  
  double sum = 0.0, rand_val;
  rand_val = rand_double();
  for (i = 0; i < n_sources; i++)
    {
      sum += get_probability(sources[i]);
      if (rand_val < sum)
	{
	  return sources[i];
	}
    }
  return NULL;
}

void new_grain ()
{
  grain_source *new_source;
  int grain_index = rand () % max_grains;

  if (grains[grain_index].offset == 0)
    {
      new_source = new_grain_source ();
      if (new_source != NULL)
	{
	  draw_grain (new_source, &(grains[grain_index]), grain_length);
	  if(!modulation)
	    grains[grain_index].amplitude = 0x100;
	}
    }
}

int sphase = 0;
void fill_buffer_8_mono (unsigned char *buffer)
{
  int i, j, offset, start_offset;
  int ampl;
  int pitch;
  int position;
  unsigned int uval;
  unsigned int *uwave;
  int max_buffer_length;
  max_buffer_length = buffer_length;
  memset (sum_buffer, 0x80, sizeof (*sum_buffer) * (max_buffer_length));
  
  for (j = 0; j < max_grains; j++)
    {
      if (grains[j].offset != 0)
      {
	  position = grains[j].position;
	  start_offset = (position + grains[j].offset) >> 1; 
	  uwave = (unsigned int *)(grains[j].source->wave_data);	  
	  offset = 0;
	  pitch = grains[j].pitch;
	  ampl = grains[j].amplitude;
	  for (i = 0; i < max_buffer_length;) 
	    {
	      uval = uwave[start_offset + (offset>>0x10)];
	      sum_buffer[i++] += (int)(((((((int)((uval & 0xff))) - 0x80)) * 
				   grain_envelope[position] * ampl) >> vol_red)); 
	      position++;
	      sum_buffer[i++] += (int)(((((((int)(((uval >> 8) & 0xff))) - 0x80)) * 
				   grain_envelope[position] * ampl) >> vol_red)); 
	      position ++;
	      sum_buffer[i++] += (int)(((((((int)(((uval >> 16) & 0xff))) - 0x80)) * 
				   grain_envelope[position] * ampl) >> vol_red)); 
	      position ++;
	      sum_buffer[i++] += (int)(((((((int)(((uval >> 24) & 0xff))) - 0x80)) * 
				   grain_envelope[position] * ampl) >> vol_red));
	      position ++;
	      offset += pitch;
	    }
	  if (position > grain_length-4)
	    grains[j].offset = 0;
	  else
	    grains[j].offset += (offset >> 0x0f) - (max_buffer_length);
	     
	  grains[j].position = position;
	}

      if (rand_double () < 0.5)
	new_grain ();
    }

  max_buffer_length = buffer_length;
  for (i = 0, j = 0 ; i < max_buffer_length;)
    {
      buffer[i++] = sum_buffer[j++] + 0x80;
    }
}


void fill_buffer_16_stereo (unsigned char *buffer)
{
  int i, j, offset, start_offset;
  int ampl;
  int pitch;
  int position;
  int val;
  int pan, apan;
  unsigned int uval;
  unsigned int *uwave;
  unsigned int *i_buffer;
  int max_buffer_length;

  max_buffer_length = buffer_length >> 1;
  i_buffer = (unsigned int *)buffer;
  memset (sum_buffer, 0, sizeof (*sum_buffer) * (max_buffer_length*4));
  for (j = 0; j < max_grains; j++)
    {
      if (grains[j].offset != 0)
      {
	  position = grains[j].position;
	  start_offset = (position + grains[j].offset) >> 1; 
	  uwave = (unsigned int *)(grains[j].source->wave_data);	  
	  offset = 0;
	  pitch = grains[j].pitch;
	  ampl = grains[j].amplitude;
	  pan = grains[j].pan;
	  apan = 0x100-pan;

	  for (i = 0; i < max_buffer_length;) 
	    {
	      uval = uwave[start_offset + (offset>>0x10)];
	      val = (((((int)((uval & 0xffff))) - 0x8000) * grain_envelope[position] * ampl) >> vol_red); 
	      
	      sum_buffer[i++] += ((val * pan) >> 7); 
	      sum_buffer[i++] += ((val * apan) >> 7);

	      position ++;
	      val = ((((int)(uval >> 0x10) - 0x8000) * grain_envelope[position] * ampl) >> vol_red); 

	      sum_buffer[i++] += ((val * pan) >> 7); 
	      sum_buffer[i++] += ((val * apan) >> 7);

	      position++;
	      offset += pitch;
	      
	      uval = uwave[start_offset + (offset>>0x10)];
	      val = (((((int)((uval & 0xffff))) - 0x8000) * grain_envelope[position] * ampl) >> vol_red); 
	      
	      sum_buffer[i++] += ((val * pan) >> 7); 
	      sum_buffer[i++] += ((val * apan) >> 7);

	      position ++;
	      val = ((((int)(uval >> 0x10) - 0x8000) * grain_envelope[position] * ampl) >> vol_red); 

	      sum_buffer[i++] += ((val * pan) >> 7); 
	      sum_buffer[i++] += ((val * apan) >> 7);

	      position++;
	      offset += pitch;
	      
	    }

	  if (position > grain_length-4)
	    grains[j].offset = 0;
	  else
	    grains[j].offset += (offset >> 0x0f) - (max_buffer_length >> 1);
	     
	  grains[j].position = position;
	}

      if (rand_double () < (new_grain_probability))
	new_grain ();
    }

  max_buffer_length = buffer_length >> 1;
  for (i = 0, j = 0 ; i < max_buffer_length;)
    {
      i_buffer[i] = sum_buffer[j++];
      i_buffer[i++] += (sum_buffer[j++] << 0x10) + 0x80008000;
    }
}



void fill_buffer_16_mono_interpolated (unsigned char *buffer)
{
  int i, j, offset, start_offset;
  int ampl;
  int pitch;
  int position;
  unsigned int uval;
  unsigned int *uwave;
  unsigned int *i_buffer;
  unsigned int interp, vala, valb;
  int max_buffer_length;
  int wave_len;
  max_buffer_length = buffer_length >> 1;
  i_buffer = (unsigned int *)buffer;

  memset (sum_buffer, 0, sizeof (*sum_buffer) * (max_buffer_length));
  for (j = 0; j < max_grains; j++)
    {
      if (grains[j].offset != 0)
      {
	  position = grains[j].position;
	  start_offset = (position + grains[j].offset) >> 1; 
	  uwave = (unsigned int *)(grains[j].source->wave_data);	  
	  offset = 0;
	  pitch = grains[j].pitch >> 1;
	  ampl = grains[j].amplitude;
	  wave_len = grains[j].source->n_samples;
	  for (i = 0; i < max_buffer_length;) 
	    {
	      uval = uwave[(start_offset + (offset>>0x10)) % wave_len];
	 
	      vala = (((((int)((uval & 0xffff))) - 0x8000) * 
				   grain_envelope[position] * ampl) >> vol_red); 
	      valb = ((((int)(uval >> 0x10) - 0x8000) * grain_envelope[position] * ampl) >> vol_red); 
	      interp = (offset & 0xffff);

	      vala *= (0xffff - interp);
	      valb *= interp;
	      
	      sum_buffer[i++] += ((vala + valb) >> 0x10);
	      position++;
	      offset += pitch;
	    }

	  if (position > grain_length-4)
	    grains[j].offset = 0;
	  else
	    grains[j].offset += (offset >> 0x0f) - (max_buffer_length);
	     
	  grains[j].position = position;
	}

      if (rand_double () <  0.10)
	new_grain ();
    }

  max_buffer_length = buffer_length >> 2;

  
  for (i = 0, j = 0 ; i < max_buffer_length;)
    {
      i_buffer[i] = sum_buffer[j++];
      i_buffer[i++] += (sum_buffer[j++] << 0x10) + 0x80008000;
    }
  
}

void fill_buffer_16_mono (unsigned char *buffer)
{
  static int total_i = 0;
  int i, j, offset, start_offset;
  int ampl;
  int pitch;
  int position;
  unsigned int uval;
  unsigned int *uwave;
  unsigned int *i_buffer;
  int max_buffer_length;
  max_buffer_length = buffer_length >> 1;
  i_buffer = (unsigned int *)buffer;
  memset (sum_buffer, 0, sizeof (*sum_buffer) * (max_buffer_length));

  for (j = 0; j < max_grains; j++)
    {
      if (grains[j].offset != 0)
      {
	  position = grains[j].position;
	  start_offset = (position + grains[j].offset) >> 1; 
	  uwave = (unsigned int *)(grains[j].source->wave_data);	  
	  offset = 0;
	  pitch = grains[j].pitch;
	  ampl = grains[j].amplitude;
	  for (i = 0; i < max_buffer_length;) 
	    {
	      uval = uwave[start_offset + (offset>>0x10)];
	 
	      sum_buffer[i++] += (((((int)((uval & 0xffff))) - 0x8000) * 
				   grain_envelope[position] * ampl) >> vol_red); 

	      position ++;
	      sum_buffer[i++] += ((((int)(uval >> 0x10) - 0x8000) * 
				   grain_envelope[position] * ampl) >> vol_red); 
	      position++;
	      offset += pitch;

	      uval = uwave[start_offset + (offset>>0x10)];

	      sum_buffer[i++] += ((((int)((uval & 0xffff))) - 0x8000) * 
					grain_envelope[position] * ampl) >> vol_red; 
	      
	      position ++;
	      
	      sum_buffer[i++] +=  ((((int)(uval >> 0x10) - 0x8000) * 
				    grain_envelope[position] * ampl) >> vol_red); 

	      position++;
	      offset += pitch;

	    }

	  if (position > grain_length-4)
	    grains[j].offset = 0;
	  else
	    grains[j].offset += (offset >> 0x0f) - (max_buffer_length);
	     
	  grains[j].position = position;
	}

      if (rand_double () < (new_grain_probability))
	new_grain ();
    }

  max_buffer_length = buffer_length >> 2;

  for (i = 0, j = 0 ; i < max_buffer_length;)
    {
      i_buffer[i] = sum_buffer[j++];
      i_buffer[i++] += (sum_buffer[j++] << 0x10) + 0x80008000;
    }
  
}




int fill_buffer (unsigned char *buffer, int total_buffer_length)
{
  int i;
  if (!initialized)
    return 0;

  if(n_bits==16 && n_channels == 1)
    {
      
      for(i=0;i<total_buffer_length;i+=buffer_length)
	{
	  update_pdf ();
#ifdef LINEAR_INTERPOLATION
	  fill_buffer_16_mono_interpolated(buffer + i);
#else
	  fill_buffer_16_mono(buffer + i);
#endif
	}
    }
  else if(n_bits == 16 && n_channels == 2)
    {
      
      for(i=0;i<total_buffer_length; i += buffer_length)
	{
	  update_pdf ();
	  fill_buffer_16_stereo(buffer + i);
	}
    }
  else if(n_bits == 8) /* 8 bit mono only */
    {
      printf("Buffer 8");
      for(i=0;i<total_buffer_length;i+=buffer_length)
	{
	  update_pdf ();
	  fill_buffer_8_mono(buffer + i);
	}
    }

  return 1;
}
