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

// gcc jack_trigger.c -o jack_trigger `pkg-config --libs --cflags jack` -lm -lasound  -Wall -O3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <jack/jack.h>
#include <alsa/asoundlib.h>

jack_port_t *input_port;
jack_client_t *client;

snd_seq_t *seq;
snd_seq_event_t ev;

int port;
int dest_client = 129, dest_port = 0;

#define DEFAULT_THRESHOLD   0.20
#define DEFAULT_BUFFER_SIZE 1024
#define DEFAULT_OVERLAP     746

double *yinBuffer;

int
absoluteThreshold(int N) 
{
  int tau;

  for (tau = 2; tau < N/2; tau++) {
    if (yinBuffer[tau] < DEFAULT_THRESHOLD) {
      while (tau + 1 < N/2 && yinBuffer[tau + 1] < yinBuffer[tau]) {
        tau++;
      }
      break;
    }
  }

  if (tau == N/2 || yinBuffer[tau] >= DEFAULT_THRESHOLD) {
    tau = -1;
  } 

  return tau;
}

double 
parabolicInterpolation(int tauEstimate, int N) 
{
  int betterTau;
  int x0;
  int x2;

  if (tauEstimate < 1) {
    x0 = tauEstimate;
  } else {
    x0 = tauEstimate - 1;
  }
  
  if (tauEstimate + 1 < N/2) {
    x2 = tauEstimate + 1;
  } else {
    x2 = tauEstimate;
  }
  
  if (x0 == tauEstimate) {
    if (yinBuffer[tauEstimate] <= yinBuffer[x2]) {
      betterTau = tauEstimate;
    } else {
      betterTau = x2;
    }
  } else if (x2 == tauEstimate) {
    if (yinBuffer[tauEstimate] <= yinBuffer[x0]) {
      betterTau = tauEstimate;
    } else {
      betterTau = x0;
    }
  } else {
    double s0, s1, s2;
    s0 = yinBuffer[x0];
    s1 = yinBuffer[tauEstimate];
    s2 = yinBuffer[x2];
    betterTau = tauEstimate + (s2 - s0) / (2.0 * (2.0 * s1 - s2 - s0));
  }
  
  return betterTau;
}

void
cumulativeMeanNormalizedDifference(int N) {
  int tau;
  double runningSum = 0.0;
  yinBuffer[0] = 1.0;
  
  for (tau = 1; tau < N/2; tau++) {
    runningSum += yinBuffer[tau];
    yinBuffer[tau] *= tau / runningSum;
  }
}

void
difference(jack_default_audio_sample_t *sample, int N) 
{
  int index, tau;
  float delta;
  
  for (tau = 0; tau < N/2; tau++) {
    yinBuffer[tau] = 0;
  }
  
  for (tau = 1; tau < N/2; tau++) {
    for (index = 0; index < N/2; index++) {
      delta = sample[index] - sample[index + tau];
      yinBuffer[tau] += delta * delta;
    }
  }
}

double
TarsosDSP_getPitch(double sr, int N, jack_default_audio_sample_t *sample) 
{
  yinBuffer = (double *) malloc (sizeof(double) * N/2);

  int tauEstimate = 0;
  double pitchInHertz = 0.0;

  difference(sample, N);
  cumulativeMeanNormalizedDifference(N);
  tauEstimate = absoluteThreshold(N);

  if (tauEstimate != -1) {
    double betterTau = parabolicInterpolation(tauEstimate, N);
    pitchInHertz = sr / betterTau;
  } 

  free(yinBuffer);

  return pitchInHertz;
}

int
freq2note (double frequency)
{
   double noteNum = 12.0 * (log(frequency / 440.0) / log(2.0));
   return (int) round(noteNum) + 69;
}
      
void 
send_ev (void) {
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port);
	snd_seq_ev_set_dest(&ev, dest_client, dest_port);
	snd_seq_event_output(seq, &ev);
}

void 
note_on (int note) {
	snd_seq_ev_set_noteon(&ev, 0, note, 120);
	send_ev();
}

void 
note_off (int note) {
	snd_seq_ev_set_noteoff(&ev, 0, note, 120);
	send_ev();
}

void
trigger (int note)
{
	note_on(note);
	snd_seq_drain_output(seq);
  usleep(3121);

	note_off(note);
	snd_seq_drain_output(seq);
	usleep(3121);
}

int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *in;

  in = jack_port_get_buffer (input_port, nframes);

  int len = sizeof (jack_default_audio_sample_t) * nframes;
  double sr = (double) jack_get_sample_rate(client); 

  int note = freq2note(TarsosDSP_getPitch(sr, len, in));

  if (note >= 0 && note <= 128) {
    trigger(note);
  }

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
  const char *client_name = "trigger";
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
alsa_init (void)
{
	if (snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
		fprintf(stderr, "Error: snd_seq_open\n");
		return -1;
	}

	port = snd_seq_create_simple_port(seq, "trigger", SND_SEQ_PORT_CAP_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC | 
                                                  SND_SEQ_PORT_TYPE_APPLICATION);
	if (port < 0) {
		fprintf(stderr, "Error: snd_seq_create_simple_port\n");
		snd_seq_close(seq);
		return -1;
	}

	if (snd_seq_connect_to(seq, port, dest_client, dest_port) < 0) {
		fprintf(stderr, "Error: snd_seq_connect_to\n");
		snd_seq_close(seq);
		return -1;
	}

  return 0;
}

int
main (int argc, char *argv[])
{

  if (argc > 1)
    dest_client = atoi(argv[1]);

  jack_init();
  alsa_init();

  sleep (-1);

  jack_client_close (client);
	snd_seq_close(seq);

  return EXIT_SUCCESS;
}
