/*
   gcc stat.c -o stat `pkg-config --libs --cflags jack` -lm -Wall -O3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <jack/jack.h>

int
main (int argc, char *argv[])
{

  jack_client_t *client;
  const char *client_name = "stat";

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

  float cpu_load = jack_cpu_load(client);
  jack_nframes_t sr  = jack_get_sample_rate(client);
  jack_nframes_t bs  = jack_get_buffer_size(client);
  int r = jack_is_realtime(client);
  int pid = jack_get_client_pid(client_name);

  jack_client_close (client);

  exit (0);
}
