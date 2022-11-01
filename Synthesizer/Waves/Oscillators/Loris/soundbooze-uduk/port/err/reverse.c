/*
   gcc reverse.c -o reverse `pkg-config --libs --cflags jack` -lm -Wall -O3
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
  jack_default_audio_sample_t *guitar_in, *synth_in, *guitar_out, *synth_out;

  guitar_in = jack_port_get_buffer (input_port1, nframes);
  guitar_out = jack_port_get_buffer (output_port1, nframes);

  synth_in = jack_port_get_buffer (input_port2, nframes);
  synth_out = jack_port_get_buffer (output_port2, nframes);

  int len = sizeof (jack_default_audio_sample_t) * nframes;

  jack_default_audio_sample_t rev_guitar[len];
  jack_default_audio_sample_t rev_synth[len];

  int i = 0;

  for (i = 0; i < len; i++) {
    rev_guitar[i] = guitar_in[(len-1) - i];
    rev_synth[i] = synth_in[(len-1) - i];
  }

  memcpy (guitar_out, rev_guitar, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (synth_out, rev_synth, sizeof (jack_default_audio_sample_t) * nframes);

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
  const char *client_name = "reverse";

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

  input_port1 = jack_port_register (client, "guitar_in",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  input_port2 = jack_port_register (client, "synth_in",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);


  output_port1 = jack_port_register (client, "guitar_out",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  output_port2 = jack_port_register (client, "synth_out",
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
