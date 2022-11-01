/*
 * Non-Copyright (c) 2019 SoundBooze <soundbooze@gmail.com>
 *                            _ _                        
 *  ___  ___  _   _ _ __   __| | |__   ___   ___ _______ 
 * / __|/ _ \| | | | '_ \ / _` | '_ \ / _ \ / _ \_  / _ \
 * \__ \ (_) | |_| | | | | (_| | |_) | (_) | (_) / /  __/
 * |___/\___/ \__,_|_| |_|\__,_|_.__/ \___/ \___/___\___|
 *                                                     
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <loris.h>
#include <jack/jack.h>

jack_port_t *input1_port, *input2_port;
jack_port_t *output1_port, *output2_port;
jack_client_t *client;

int xtract_f0 (const double *, const int, const void *, double *);
PartialList *synthesizestream (double, double *, unsigned int, double);
double *morphstream (PartialList *, PartialList *, unsigned int, double);

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *in1, *out1, *in2, *out2;

  in1 = jack_port_get_buffer (input1_port, nframes);
  out1 = jack_port_get_buffer (output1_port, nframes);

  in2 = jack_port_get_buffer (input2_port, nframes);
  out2 = jack_port_get_buffer (output2_port, nframes);

  int len = sizeof (jack_default_audio_sample_t) * nframes;
  double f0 = 0.0;
  double sr = (double) jack_get_sample_rate(client); 

  xtract_f0((double *) in1, len, &sr, &f0);

  PartialList *src1 = synthesizestream (f0, (double *) in1, len, sr);
  PartialList *src2 = synthesizestream (f0, (double *) in2, len, sr);

  jack_default_audio_sample_t *morphed = (jack_default_audio_sample_t *) morphstream (src1, src2, len, sr);

  memcpy (out1, morphed, sizeof (jack_default_audio_sample_t) * nframes);
  memcpy (out2, morphed, sizeof (jack_default_audio_sample_t) * nframes);

  free(morphed);

  return 0;      
}

void
jack_shutdown (void *arg)
{
  exit (EXIT_FAILURE);
}

int
jack_init (void)
{
  const char **ports;
  const char *client_name = "morphin";
  const char *server_name = NULL;

  jack_options_t options = JackNullOption;
  jack_status_t status;

  client = jack_client_open (client_name, options, &status, server_name);
  if (client == NULL) {
    fprintf (stderr, "jack_client_open() failed, " "status = 0x%2.0x\n", status);
    if (status & JackServerFailed) {
      fprintf (stderr, "Unable to connect to JACK server\n");
    }
    return -1;
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

  input1_port = jack_port_register (client, "input1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  output1_port = jack_port_register (client, "output1",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  input2_port = jack_port_register (client, "input2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  output2_port = jack_port_register (client, "output2",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput, 0);

  if ((input1_port == NULL) || (output1_port == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    return -1;
  }

  if ((input1_port == NULL) || (output2_port == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    return -1;
  }

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    return -1;
  }

  ports = jack_get_ports (client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
  if (ports == NULL) {
    fprintf(stderr, "no physical capture ports\n");
    return -1;
  }

  if (jack_connect (client, ports[0], jack_port_name (input1_port))) {
    fprintf (stderr, "cannot connect input ports\n");
  }

  if (jack_connect (client, ports[0], jack_port_name (input2_port))) {
    fprintf (stderr, "cannot connect input ports\n");
  }

  free (ports);

  ports = jack_get_ports (client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL) {
    fprintf(stderr, "no physical playback ports\n");
    return -1;
  }

  if (jack_connect (client, jack_port_name (output1_port), ports[0])) {
    fprintf (stderr, "cannot connect output ports\n");
  }

  if (jack_connect (client, jack_port_name (output2_port), ports[0])) {
    fprintf (stderr, "cannot connect output ports\n");
  }

  free (ports);

  return 0;
}

int
main (int argc, char *argv[])
{
  jack_init();

  sleep (-1);

  jack_client_close (client);

  return EXIT_SUCCESS;
}
