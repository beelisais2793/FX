#ifndef GRAIN_ENGINE_H
#define GRAIN_ENGINE_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "math.h"
#include "grain_source.h"

#define WAV_FILE_RATE 44100
#define DEFAULT_GRAIN_LENGTH 8000
#define DEFAULT_MAX_GRAINS 400
#define DEFAULT_GRAIN_DENSITY 4000
#define DEFAULT_ENVELOPE_TYPE ENV_LINEAR
#define DEFAULT_ATTACK 0.2
#define DEFAULT_NORMALIZE 1
#define DEFAULT_MODULATION 1
#define DEFAULT_MAX_SOURCES 128
#define DEFAULT_BUFFER_LENGTH 4096

void set_format (int sample_rate, int n_bits, int n_channels);
void register_distribution_function (double (*distribution_func)
				     (grain_source * source));
void register_time_function (void (*time_func)
				     (int));
int initialize_engine ();
int fill_buffer (unsigned char *buffer, int sub_buffer_length);
void set_grain_length (int grain_length);
void set_max_grains (int max_grains);
void set_grain_density (int grain_density);
void set_envelope (envelope type, double attack);
void set_normalize (int normalize);
void set_amplitude_modulation (int modulation);
void set_buffer_length (int length);
void add_source (grain_source * source);
#endif
