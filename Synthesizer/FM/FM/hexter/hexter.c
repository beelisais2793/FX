/* hexter DSSI software synthesizer plugin
 *
 * Copyright (C) 2004, 2009, 2011, 2012, 2014, 2018 Sean Bolton and others.
 *
 * Portions of this file may have come from Peter Hanappe's
 * Fluidsynth, copyright (C) 2003 Peter Hanappe and others.
 * Portions of this file may have come from Chris Cannam and Steve
 * Harris's public domain DSSI example code.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <ladspa.h>
#include <alsa/seq_event.h>
#include <dssi.h>

#include "hexter_types.h"
#include "hexter.h"
#include "hexter_synth.h"
#include "dx7_voice.h"
#include "dx7_voice_data.h"

static LADSPA_Descriptor *hexter_LADSPA_descriptor = NULL;
static DSSI_Descriptor   *hexter_DSSI_descriptor = NULL;

static void
hexter_cleanup(LADSPA_Handle instance);

static void
hexter_run_synth(LADSPA_Handle instance, unsigned long sample_count,
                 snd_seq_event_t *events, unsigned long event_count);

/* ---- mutual exclusion ---- */

static inline int
dssp_voicelist_mutex_trylock(hexter_instance_t *instance)
{
    int rc;

    /* Attempt the mutex lock */
    rc = pthread_mutex_trylock(&instance->voicelist_mutex);
    if (rc) {
        instance->voicelist_mutex_grab_failed = 1;
        return rc;
    }
    /* Clean up if a previous mutex grab failed */
    if (instance->voicelist_mutex_grab_failed) {
        hexter_instance_all_voices_off(instance);
        instance->voicelist_mutex_grab_failed = 0;
    }
    return 0;
}

inline int
dssp_voicelist_mutex_lock(hexter_instance_t *instance)
{
    return pthread_mutex_lock(&instance->voicelist_mutex);
}

inline int
dssp_voicelist_mutex_unlock(hexter_instance_t *instance)
{
    return pthread_mutex_unlock(&instance->voicelist_mutex);
}

/* ---- LADSPA interface ---- */

/*
 * hexter_instantiate
 *
 * implements LADSPA (*instantiate)()
 */
static LADSPA_Handle
hexter_instantiate(const LADSPA_Descriptor *descriptor,
                   unsigned long sample_rate)
{
    hexter_instance_t *instance;
    int i;

    instance = (hexter_instance_t *)calloc(1, sizeof(hexter_instance_t));
    if (!instance) {
        return NULL;
    }

    /* do any per-instance one-time initialization here */
    for (i = 0; i < HEXTER_MAX_POLYPHONY; i++) {
        instance->voice[i] = dx7_voice_new();
        if (!instance->voice[i]) {
            DEBUG_MESSAGE(-1, " hexter_instantiate: out of memory!\n");
            hexter_cleanup(instance);
            return NULL;
        }
    }
    if (!(instance->patches = (dx7_patch_t *)malloc(128 * DX7_VOICE_SIZE_PACKED))) {
        DEBUG_MESSAGE(-1, " hexter_instantiate: out of memory!\n");
        hexter_cleanup(instance);
        return NULL;
    }

    instance->sample_rate = (float)sample_rate;
    instance->nugget_remains = 0;
    dx7_eg_init_constants(instance);  /* depends on sample rate */

    instance->note_id = 0;
    instance->polyphony = HEXTER_DEFAULT_POLYPHONY;
    instance->monophonic = DSSP_MONO_MODE_OFF;
    instance->max_voices = instance->polyphony;
    instance->current_voices = 0;
    instance->last_key = 0;
    pthread_mutex_init(&instance->voicelist_mutex, NULL);
    instance->voicelist_mutex_grab_failed = 0;
    pthread_mutex_init(&instance->patches_mutex, NULL);
    instance->pending_program_change = -1;
    instance->current_program = 0;
    instance->overlay_program = -1;
    hexter_data_performance_init(instance->performance_buffer);
    hexter_data_patches_init(instance->patches);
    hexter_instance_select_program(instance, 0, 0);
    hexter_instance_init_controls(instance);

    return (LADSPA_Handle)instance;
}

/*
 * hexter_connect_port
 *
 * implements LADSPA (*connect_port)()
 */
static void
hexter_connect_port(LADSPA_Handle handle, unsigned long port, LADSPA_Data *data)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;

    switch (port) {
      case HEXTER_PORT_OUTPUT:  instance->output = data;  break;
      case HEXTER_PORT_TUNING:  instance->tuning = data;  break;
      case HEXTER_PORT_VOLUME:  instance->volume = data;  break;
      default:
        break;
    }
}

/*
 * hexter_activate
 *
 * implements LADSPA (*activate)()
 */
static void
hexter_activate(LADSPA_Handle handle)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;

    hexter_instance_all_voices_off(instance);  /* stop all sounds immediately */
    instance->current_voices = 0;
    dx7_lfo_reset(instance);
}

/*
 * hexter_ladspa_run
 */
static void
hexter_ladspa_run(LADSPA_Handle instance, unsigned long sample_count)
{
    hexter_run_synth(instance, sample_count, NULL, 0);
}

// optional:
//  void (*run_adding)(LADSPA_Handle Instance,
//                     unsigned long SampleCount);
//  void (*set_run_adding_gain)(LADSPA_Handle Instance,
//                              LADSPA_Data   Gain);

/*
 * hexter_deactivate
 *
 * implements LADSPA (*deactivate)()
 */
void
hexter_deactivate(LADSPA_Handle handle)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;

    hexter_instance_all_voices_off(instance);  /* stop all sounds immediately */
}

/*
 * hexter_cleanup
 *
 * implements LADSPA (*cleanup)()
 */
static void
hexter_cleanup(LADSPA_Handle handle)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;
    int i;

    if (instance) {
        hexter_deactivate(instance);

        if (instance->patches) free(instance->patches);
        for (i = 0; i < HEXTER_MAX_POLYPHONY; i++) {
            if (instance->voice[i]) {
                free(instance->voice[i]);
                instance->voice[i] = NULL;
            }
        }
        free(instance);
    }
}

/* ---- DSSI interface ---- */

/*
 * hexter_configure
 *
 * implements DSSI (*configure)()
 */
char *
hexter_configure(LADSPA_Handle handle, const char *key, const char *value)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;

    DEBUG_MESSAGE(DB_DSSI, " hexter_configure called with '%s' and '%s'\n", key, value);

    if (strlen(key) == 8 && !strncmp(key, "patches", 7)) {

        return hexter_instance_handle_patches(instance, key, value);

    } else if (!strcmp(key, "edit_buffer")) {

        return hexter_instance_handle_edit_buffer(instance, value);

    } else if (!strcmp(key, "performance")) {  /* global performance parameters */

        return hexter_instance_handle_performance(instance, value);

    } else if (!strcmp(key, "monophonic")) {

        return hexter_instance_handle_monophonic(instance, value);

    } else if (!strcmp(key, "polyphony")) {

        return hexter_instance_handle_polyphony(instance, value);

#ifdef DSSI_GLOBAL_CONFIGURE_PREFIX
    } else if (!strcmp(key, DSSI_GLOBAL_CONFIGURE_PREFIX "polyphony")) {
#else
    } else if (!strcmp(key, "global_polyphony")) {
#endif

        DEBUG_MESSAGE(DB_DSSI, " -- global polyphony limiting is no longer supported --\n");

#ifdef DSSI_PROJECT_DIRECTORY_KEY
    } else if (!strcmp(key, DSSI_PROJECT_DIRECTORY_KEY)) {

        return NULL; /* plugin has no use for project directory key, ignore it */

#endif
    }
    return strdup("error: unrecognized configure key");
}

/*
 * hexter_get_program
 *
 * implements DSSI (*get_program)()
 */
const DSSI_Program_Descriptor *
hexter_get_program(LADSPA_Handle handle, unsigned long index)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;
    static DSSI_Program_Descriptor pd;

    DEBUG_MESSAGE(DB_DSSI, " hexter_get_program called with %lu\n", index);

    if (index < 128) {
        hexter_instance_set_program_descriptor(instance, &pd, 0, index);
        return &pd;
    }
    return NULL;
}

/*
 * hexter_select_program
 *
 * implements DSSI (*select_program)()
 */
void
hexter_select_program(LADSPA_Handle handle, unsigned long bank,
                      unsigned long program)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;

    DEBUG_MESSAGE(DB_DSSI, " hexter_select_program called with %lu and %lu\n", bank, program);

    /* ignore invalid program requests */
    if (bank || program >= 128)
        return;

    /* Attempt the patch mutex, return if lock fails. */
    if (pthread_mutex_trylock(&instance->patches_mutex)) {
        instance->pending_program_change = program;
        return;
    }

    hexter_instance_select_program((hexter_instance_t *)instance, bank, program);

    pthread_mutex_unlock(&instance->patches_mutex);
}

/*
 * hexter_handle_pending_program_change
 */
static inline void
hexter_handle_pending_program_change(hexter_instance_t *instance)
{
    /* Attempt the patch mutex, return if lock fails. */
    if (pthread_mutex_trylock(&instance->patches_mutex))
        return;

    hexter_instance_select_program((hexter_instance_t *)instance, 0,
                                   instance->pending_program_change);
    instance->pending_program_change = -1;

    pthread_mutex_unlock(&instance->patches_mutex);
}

/*
 * hexter_get_midi_controller
 *
 * implements DSSI (*get_midi_controller_for_port)()
 */
int
hexter_get_midi_controller(LADSPA_Handle handle, unsigned long port)
{
    DEBUG_MESSAGE(DB_DSSI, " hexter_get_midi_controller called for port %lu\n", port);
    switch (port) {
      // case EXAMPLE_PORT_VOLUME:
      //   return DSSI_CC(7);
      default:
        break;
    }

    return DSSI_NONE;
}

/*
 * hexter_handle_event
 */
static inline void
hexter_handle_event(hexter_instance_t *instance, snd_seq_event_t *event)
{
    DEBUG_MESSAGE(DB_DSSI, " hexter_handle_event called with event type %d\n", event->type);

    switch (event->type) {
      case SND_SEQ_EVENT_NOTEOFF:
        hexter_instance_note_off(instance, event->data.note.note, event->data.note.velocity);
        break;
      case SND_SEQ_EVENT_NOTEON:
        if (event->data.note.velocity > 0)
            hexter_instance_note_on(instance, event->data.note.note, event->data.note.velocity);
        else
            hexter_instance_note_off(instance, event->data.note.note, 64); /* shouldn't happen, but... */
        break;
      case SND_SEQ_EVENT_KEYPRESS:
        hexter_instance_key_pressure(instance, event->data.note.note, event->data.note.velocity);
        break;
      case SND_SEQ_EVENT_CONTROLLER:
        hexter_instance_control_change(instance, event->data.control.param, event->data.control.value);
        break;
      case SND_SEQ_EVENT_CHANPRESS:
        hexter_instance_channel_pressure(instance, event->data.control.value);
        break;
      case SND_SEQ_EVENT_PITCHBEND:
        hexter_instance_pitch_bend(instance, event->data.control.value);
        break;
      /* SND_SEQ_EVENT_PGMCHANGE - shouldn't happen */
      /* SND_SEQ_EVENT_SYSEX - shouldn't happen */
      /* SND_SEQ_EVENT_CONTROL14? */
      /* SND_SEQ_EVENT_NONREGPARAM? */
      /* SND_SEQ_EVENT_REGPARAM? */
      default:
        break;
    }
}

static void
hexter_run_synth(LADSPA_Handle handle, unsigned long sample_count,
                 snd_seq_event_t *events, unsigned long event_count)
{
    hexter_instance_t *instance = (hexter_instance_t *)handle;

    unsigned long samples_done = 0;
    unsigned long event_index = 0;
    unsigned long burst_size;

    /* silence the buffer */
    memset(instance->output, 0, sizeof(LADSPA_Data) * sample_count);
#if defined(DSSP_DEBUG) && (DSSP_DEBUG & DB_AUDIO)
*instance->output += 0.10f; /* add a 'buzz' to output so there's something audible even when quiescent */
#endif /* defined(DSSP_DEBUG) && (DSSP_DEBUG & DB_AUDIO) */

    /* attempt the mutex, return only silence if lock fails. */
    if (dssp_voicelist_mutex_trylock(instance))
        return;

    if (instance->pending_program_change > -1)
        hexter_handle_pending_program_change(instance);

    while (samples_done < sample_count) {

        if (!instance->nugget_remains)
            instance->nugget_remains = HEXTER_NUGGET_SIZE;

        /* process any ready events */
        while (event_index < event_count
               && samples_done == events[event_index].time.tick) {
            hexter_handle_event(instance, &events[event_index]);
            event_index++;
        }

        /* calculate the sample count (burst_size) for the next
         * hexter_instance_render_voices() call to be the smallest of:
         * - control calculation quantization size (HEXTER_NUGGET_SIZE,
         *     in samples)
         * - the number of samples remaining in an already-begun nugget
         *     (instance->nugget_remains)
         * - the number of samples until the next event is ready
         * - the number of samples left in this run
         */
        burst_size = HEXTER_NUGGET_SIZE;
        if (instance->nugget_remains < burst_size) {
            /* we're still in the middle of a nugget, so reduce the burst size
             * to end when the nugget ends */
            burst_size = instance->nugget_remains;
        }
        if (event_index < event_count
            && events[event_index].time.tick - samples_done < burst_size) {
            /* reduce burst size to end when next event is ready */
            burst_size = events[event_index].time.tick - samples_done;
        }
        if (sample_count - samples_done < burst_size) {
            /* reduce burst size to end at end of this run */
            burst_size = sample_count - samples_done;
        }

        /* render the burst */
        hexter_instance_render_voices(instance, samples_done, burst_size,
                                      (burst_size == instance->nugget_remains));
        samples_done += burst_size;
        instance->nugget_remains -= burst_size;
    }

    dssp_voicelist_mutex_unlock(instance);
}

// optional:
//    void (*run_synth_adding)(LADSPA_Handle    Instance,
//                             unsigned long    SampleCount,
//                             snd_seq_event_t *Events,
//                             unsigned long    EventCount);
//    void (*run_multiple_synths)(unsigned long     InstanceCount,
//                                LADSPA_Handle   **Instances,
//                                unsigned long     SampleCount,
//                                snd_seq_event_t **Events,
//                                unsigned long    *EventCounts);
//    void (*run_multiple_synths_adding)(unsigned long     InstanceCount,
//                                       LADSPA_Handle   **Instances,
//                                       unsigned long     SampleCount,
//                                       snd_seq_event_t **Events,
//                                       unsigned long    *EventCounts);

/* ---- export ---- */

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
    switch (index) {
      case 0:
        return hexter_LADSPA_descriptor;
      default:
        return NULL;
    }
}

const DSSI_Descriptor *dssi_descriptor(unsigned long index)
{
    switch (index) {
      case 0:
        return hexter_DSSI_descriptor;
      default:
        return NULL;
    }
}

#ifdef __GNUC__
__attribute__((constructor)) void init()
#else
void _init()
#endif
{
    char **port_names;
    LADSPA_PortDescriptor *port_descriptors;
    LADSPA_PortRangeHint *port_range_hints;

    DSSP_DEBUG_INIT("hexter.so");

    dx7_voice_init_tables();

    hexter_LADSPA_descriptor =
        (LADSPA_Descriptor *) malloc(sizeof(LADSPA_Descriptor));
    if (hexter_LADSPA_descriptor) {
        hexter_LADSPA_descriptor->UniqueID = 2183;
        hexter_LADSPA_descriptor->Label = "hexter";
        hexter_LADSPA_descriptor->Properties = 0;
        hexter_LADSPA_descriptor->Name = "hexter DX7 emulation (v" VERSION ")";
        hexter_LADSPA_descriptor->Maker = "Sean Bolton <sean AT smbolton DOT com>";
        hexter_LADSPA_descriptor->Copyright = "GNU General Public License version 2 or later";
        hexter_LADSPA_descriptor->PortCount = HEXTER_PORTS_COUNT;

        port_descriptors = (LADSPA_PortDescriptor *)
                                calloc(hexter_LADSPA_descriptor->PortCount, sizeof
                                                (LADSPA_PortDescriptor));
        hexter_LADSPA_descriptor->PortDescriptors =
            (const LADSPA_PortDescriptor *) port_descriptors;

        port_range_hints = (LADSPA_PortRangeHint *)
                                calloc(hexter_LADSPA_descriptor->PortCount, sizeof
                                                (LADSPA_PortRangeHint));
        hexter_LADSPA_descriptor->PortRangeHints =
            (const LADSPA_PortRangeHint *) port_range_hints;

        port_names = (char **) calloc(hexter_LADSPA_descriptor->PortCount, sizeof(char *));
        hexter_LADSPA_descriptor->PortNames = (const char **) port_names;

        /* Parameters for Output */
        port_descriptors[HEXTER_PORT_OUTPUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
        port_names[HEXTER_PORT_OUTPUT] = "Output";
        port_range_hints[HEXTER_PORT_OUTPUT].HintDescriptor = 0;

        /* Parameters for Tuning */
        port_descriptors[HEXTER_PORT_TUNING] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[HEXTER_PORT_TUNING] = "Tuning";
        port_range_hints[HEXTER_PORT_TUNING].HintDescriptor =
                        LADSPA_HINT_DEFAULT_440 |
                        LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[HEXTER_PORT_TUNING].LowerBound = 415.3f;
        port_range_hints[HEXTER_PORT_TUNING].UpperBound = 466.2f;

        /* Parameters for Volume */
        port_descriptors[HEXTER_PORT_VOLUME] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[HEXTER_PORT_VOLUME] = "Volume";
        port_range_hints[HEXTER_PORT_VOLUME].HintDescriptor =
                        LADSPA_HINT_DEFAULT_0 |
                        LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[HEXTER_PORT_VOLUME].LowerBound = -70.0f;
        port_range_hints[HEXTER_PORT_VOLUME].UpperBound =  20.0f;

        hexter_LADSPA_descriptor->instantiate = hexter_instantiate;
        hexter_LADSPA_descriptor->connect_port = hexter_connect_port;
        hexter_LADSPA_descriptor->activate = hexter_activate;
        hexter_LADSPA_descriptor->run = hexter_ladspa_run;
        hexter_LADSPA_descriptor->run_adding = NULL;
        hexter_LADSPA_descriptor->set_run_adding_gain = NULL;
        hexter_LADSPA_descriptor->deactivate = hexter_deactivate;
        hexter_LADSPA_descriptor->cleanup = hexter_cleanup;
    }

    hexter_DSSI_descriptor = (DSSI_Descriptor *) malloc(sizeof(DSSI_Descriptor));
    if (hexter_DSSI_descriptor) {
        hexter_DSSI_descriptor->DSSI_API_Version = 1;
        hexter_DSSI_descriptor->LADSPA_Plugin = hexter_LADSPA_descriptor;
        hexter_DSSI_descriptor->configure = hexter_configure;
        hexter_DSSI_descriptor->get_program = hexter_get_program;
        hexter_DSSI_descriptor->select_program = hexter_select_program;
        hexter_DSSI_descriptor->get_midi_controller_for_port = hexter_get_midi_controller;
        hexter_DSSI_descriptor->run_synth = hexter_run_synth;
        hexter_DSSI_descriptor->run_synth_adding = NULL;
        hexter_DSSI_descriptor->run_multiple_synths = NULL;
        hexter_DSSI_descriptor->run_multiple_synths_adding = NULL;
    }
}

#ifdef __GNUC__
__attribute__((destructor)) void fini()
#else
void _fini()
#endif
{
    if (hexter_LADSPA_descriptor) {
        free((LADSPA_PortDescriptor *) hexter_LADSPA_descriptor->PortDescriptors);
        free((char **) hexter_LADSPA_descriptor->PortNames);
        free((LADSPA_PortRangeHint *) hexter_LADSPA_descriptor->PortRangeHints);
        free(hexter_LADSPA_descriptor);
    }
    if (hexter_DSSI_descriptor) {
        free(hexter_DSSI_descriptor);
    }
}
