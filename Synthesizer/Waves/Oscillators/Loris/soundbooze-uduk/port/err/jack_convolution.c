/*
   gcc jack_convolution.c -o jack_convolution `pkg-config --libs --cflags jack` -lm -Wall -O3 -lsndfile
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sndfile.h>

#include <jack/jack.h>

jack_port_t *input_port1, *input_port2;
jack_port_t *output_port1, *output_port2;
jack_client_t *client;

long impulseLen; 
float *impulseSignal;

float *
readWav (char *filename, long *len) {

  SF_INFO sndInfo_r;
  float *buffer_r;
  long numFrames;

  SNDFILE *sndFile_r = sf_open(filename, SFM_READ, &sndInfo_r);
  if (sndFile_r == NULL) {
    fprintf(stderr, "sf_open: '%s': %s\n", filename, sf_strerror(sndFile_r));
    exit(EXIT_FAILURE);
  }

  buffer_r = malloc((sndInfo_r.frames * sndInfo_r.channels) * sizeof(float));
  if (buffer_r == NULL) {
    fprintf(stderr, "malloc error\n");
    sf_close(sndFile_r);
    exit(EXIT_FAILURE);
  }

  numFrames = sf_readf_float(sndFile_r, buffer_r, sndInfo_r.frames * sndInfo_r.channels);
  if (numFrames != sndInfo_r.frames) {
    fprintf(stderr, "sf_readf problem\n");
    sf_close(sndFile_r);
    free(buffer_r);
    exit(EXIT_FAILURE);
  }

  if (sndInfo_r.channels == 2) {
    float *buffer_m = malloc((sndInfo_r.frames) * sizeof(float));

    for (unsigned int i = 0; i < sndInfo_r.frames; i++) {
      buffer_m[i] = 0;

      for(unsigned int j = 0; j < sndInfo_r.channels; j++) {
        buffer_m[i] += buffer_r[i * sndInfo_r.channels + j];
      }

      buffer_m[i] /= sndInfo_r.channels;
    }

    sf_close(sndFile_r);
    *len = numFrames;
    return buffer_m;
  }

  sf_close(sndFile_r);
  *len = numFrames;

  return buffer_r;
}

float *
convolve (float *originalSignal, long originalLen, float *impulseSignal, long impulseLen)
{
  long convLen = (originalLen + impulseLen) - 1;
  long iLen = impulseLen - 1;
  long oLen = originalLen - 1;

  float *convSignal = (float *) calloc (convLen, sizeof (float));
 
  for (long n = 0; n < convLen; n++) {
    long kmin, kmax, k;
    kmin = (n >= iLen) ? n - iLen : 0;
    kmax = (n < oLen) ? n : oLen;
    for (k = kmin; k <= kmax; k++) {
      convSignal[n] += originalSignal[k] * impulseSignal[n - k];
    }
  }

  return convSignal;
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input1_in, *input2_in, *output1_out, *output2_out;
  jack_default_audio_sample_t *conv_1, *conv_2;

  input1_in = jack_port_get_buffer (input_port1, nframes);
  output1_out = jack_port_get_buffer (output_port1, nframes);

  input2_in = jack_port_get_buffer (input_port2, nframes);
  output2_out = jack_port_get_buffer (output_port2, nframes);

  conv_1 = convolve(input1_in, nframes, impulseSignal, impulseLen);
  conv_2 = convolve(input2_in, nframes, impulseSignal, impulseLen);

  jack_default_audio_sample_t out1_out[nframes];
  jack_default_audio_sample_t out2_out[nframes];

  // resample

  memcpy (output1_out, out1_out, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (output2_out, out2_out, sizeof (jack_default_audio_sample_t) * nframes);

  free(conv_1); free(conv_2);

  return 0;      
}

void
jack_shutdown (void *arg)
{
  exit (1);
}

int
main (int argc, char *argv[])
{
  const char *client_name = "convolution";

  jack_options_t options = JackNullOption;
  jack_status_t status;

  if (argc < 2) {
    printf("%s ir.wav\n", argv[0]);
    exit(-1);
  }

  impulseSignal = readWav(argv[1], &impulseLen);

  /* free(impulseSignal); */

  client = jack_client_open (client_name, options, &status, NULL);
  if (client == NULL) {
    fprintf (stderr, "jack_client_open() failed, " "status = 0x%2.0x\n", status);
    if (status & JackServerFailed) {
      fprintf (stderr, "Unable to connect to JACK server\n");
    }
    exit (1);
  }

  if (status & JackServerStarted) {
    fprintf (stderr, "JACK server started\n");
  }

  if (status & JackNameNotUnique) {
    client_name = jack_get_client_name(client);
    fprintf (stderr, "unique name `%s' assigned\n", client_name);
  }

  jack_set_process_callback (client, process, 0);

  jack_on_shutdown (client, jack_shutdown, 0);

  input_port1 = jack_port_register (client, "input1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  input_port2 = jack_port_register (client, "input2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);


  output_port1 = jack_port_register (client, "output1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  output_port2 = jack_port_register (client, "output2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  if ((input_port1 == NULL) || (output_port1 == NULL) || (input_port2 == NULL) || (output_port2 == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  sleep (-1);

  jack_client_close (client);

  exit (0);
}
