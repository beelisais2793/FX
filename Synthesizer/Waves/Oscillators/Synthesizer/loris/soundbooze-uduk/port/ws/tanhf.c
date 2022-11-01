/*
   gcc tanhf.c -o tanhf `pkg-config --libs --cflags jack` -lm -Wall -O3
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <jack/jack.h>

jack_port_t *input_port1, *input_port2;
jack_port_t *output_port1, *output_port2;
jack_client_t *client;

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *input1_in, *input2_in, *output1_out, *output2_out;

  input1_in = jack_port_get_buffer (input_port1, nframes);
  output1_out = jack_port_get_buffer (output_port1, nframes);

  input2_in = jack_port_get_buffer (input_port2, nframes);
  output2_out = jack_port_get_buffer (output_port2, nframes);

  int len = sizeof (jack_default_audio_sample_t) * nframes;
  jack_default_audio_sample_t tanhout_1[len];
  jack_default_audio_sample_t tanhout_2[len];

  int i = 0;
  for (i = 0; i < len; i++) {
    tanhout_1[i] = tanhf(input1_in[i]);
    tanhout_2[i] = tanhf(input2_in[i]);
  }

  memcpy (output1_out, tanhout_1, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (output2_out, tanhout_2, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "tanhf";

  jack_options_t options = JackNullOption;
  jack_status_t status;

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
