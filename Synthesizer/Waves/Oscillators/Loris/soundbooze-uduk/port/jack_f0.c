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

// gcc jack_f0.c -o jack_f0 -lxtract -I/usr/local/include -L/usr/local/lib -lm `pkg-config --libs --cflags jack` -Wall -O3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <xtract/libxtract.h>
#include <jack/jack.h>

jack_port_t *input_port;
jack_client_t *client;

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *in;

  in = jack_port_get_buffer (input_port, nframes);

  int len = sizeof (jack_default_audio_sample_t) * nframes;
  double f0 = 0.0;
  double sr = (double) jack_get_sample_rate(client); 

  xtract_f0((double *) in, len, &sr, &f0);

  printf("%.4f\n", f0);

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
  const char *client_name = "f0";
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

  input_port = jack_port_register (client, "input",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsInput, 0);

  if ((input_port == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    return -1;
  }

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    return -1;
  }

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
