#pragma once

#include <cassert>
#include <cstring>
#include "effects_biquad.h"

#define BUF_TYPE DspFloatType

extern DspFloatType sampleRate;
extern DspFloatType invSampleRate;

#define HIGHPASS 0
#define LOWPASS 1

#define MAX_FILTERS 10

namespace HammerFX
{
    struct filter_data {
        DspFloatType          i[MAX_FILTERS][2][MAX_CHANNELS],
                        last_sample[MAX_FILTERS][2][MAX_CHANNELS];
        DspFloatType          max,
                        amplify,
                        R,
                        C,
                        invR,
                        dt_div_C;
        DspFloatType          di[MAX_FILTERS][2][MAX_CHANNELS];
    };



    typedef struct Backbuf {
        BUF_TYPE *storage;
        unsigned int mask, nstor, curpos;
        /* method slots */
        void (*add)(struct Backbuf *b, const BUF_TYPE value);
        BUF_TYPE (*get)(struct Backbuf *b, const unsigned int pos);
        BUF_TYPE (*get_interpolated)(struct Backbuf *b, DspFloatType pos);
        void (*clear)(struct Backbuf *b);
    } Backbuf_t;

    struct autowah_params {
        Backbuf_t      *history;
        DspFloatType           sweep_time;
        DspFloatType           freq_low;
        DspFloatType           freq_high;
        DspFloatType           res;
        DspFloatType 	        drywet;
        int             sync;
        int             method;

        DspFloatType           fresh_accum_delta;
        DspFloatType           fresh_accum_power;
        DspFloatType           delayed_accum_delta;
        DspFloatType           delayed_accum_power;
        int             accum_n;
        
        DspFloatType           f, smoothed_f, freq_vibrato;
        int             dir;
        DspFloatType           ya[MAX_CHANNELS];
        DspFloatType           yb[MAX_CHANNELS];
        DspFloatType           yc[MAX_CHANNELS];
        DspFloatType           yd[MAX_CHANNELS];    
        unsigned        num_channels;
        Biquad_t        lpf;
        Biquad_t        bpf;
    };



    void autowah_init(autowah_params * ap);
    void autowah_destroy(autowah_params * ap);
    void autowah_filter(autowah_params *ap, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs);


    struct chorus_params {
        Backbuf_t      *history[MAX_CHANNELS];
        DspFloatType           drywet,
                        basedelay,
                        depth,
                        speed,
                        regen,
                        ang;
        int             voices;
        short           multichannel;    
    };



    void chorus_filter_mono(chorus_params * cp, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs);
    void chorus_create(chorus_params * cp);
    void chorus_destroy(struct chorus_params * cp);



    struct delay_params {
        Backbuf_t       *history[MAX_CHANNELS];  /* history of samples */
        short            multichannel;
        DspFloatType            delay_decay,   /* % of decay each time */
                        delay_time;    /* time (ms) between repeats */
        int              delay_count;    /* number of repeats */
        size_t           numChannels;
    };

    void delay_create(delay_params * pdelay);
    void delay_destroy(struct delay_params * dp);
    void delay_filter_mono(delay_params * dp, size_t n, DspFloatType * inputs, DspFloatType * outputs);



    struct distort_params {
        int             sat,
                        level,
                        drive,
                        lastval[MAX_CHANNELS],
                        noisegate;
        int             lowpass;
        struct filter_data fd;
        struct filter_data noise;
    };


    void distort_create(struct distort_params *ap);
    void distort_filter(struct distort_params * dp, size_t numSamples, DspFloatType * buffer);

    struct distort2_params {
        DspFloatType       drive, clip, treble;
        DspFloatType 	last[MAX_CHANNELS];

        DspFloatType       interpolate_firmem[MAX_CHANNELS][8];
        Biquad_t    feedback_minus_loop, output_bass_cut,
                    rolloff, treble_highpass;
    };

    void distort2_create(struct distort2_params *ap);
    void distort2_filter(struct distort2_params * dp, size_t numSamples, DspFloatType * buffer);



    #define MAX_ECHO_COUNT 4
    #define MAX_ECHO_LENGTH 500 /* ms */

    struct echo_params {
        Backbuf_t *history[MAX_CHANNELS][MAX_ECHO_COUNT];
        DspFloatType primes[MAX_ECHO_COUNT];
        DspFloatType size_factor[MAX_ECHO_COUNT];
        DspFloatType decay_factor[MAX_ECHO_COUNT];
        /** @brief Length of the echo, in milliseconds. */
        DspFloatType echo_size;
        /** @brief Each echo reduces in volume. The amount
         * of volume reduced is determined by the decay. This
         * value should be between zero and one hundred. */
        DspFloatType echo_decay;
        /** @brief The number of echoes that occur. */
        unsigned int echoes;
        /** @deprecated */
        short multichannel;
    };


    void echo_create(struct echo_params *params);
    void echo_destroy(struct echo_params *params );
    void echo_filter(struct echo_params  *params, size_t numSamples, DspFloatType * input, DspFloatType * output);


    struct eqbank_params {
        DspFloatType          *boosts;
        DspFloatType           volume;
        Biquad_t        *filters;
        DspFloatType      history_in[MAX_CHANNELS][8];
        DspFloatType      history_out[MAX_CHANNELS][8];
    };


    #define MAX_PHASOR_FILTERS  24

    struct phasor_params {
        DspFloatType          depth, sweep_time, drywet, f;
        int             stereo;
        Biquad_t        allpass[MAX_PHASOR_FILTERS];
        Hilbert_t       hilb;
    };

    void phasor_filter_mono(struct phasor_params *params, size_t numSamples, DspFloatType * inputs,DspFloatType * outputs);
    void phasor_create(struct phasor_params *params);


    struct sustain_params {
        int             sust,
                        noise,
                        threshold,
                        volaccum;
    };

    void sustain_create(struct sustain_params *psustain);
    void sustain_filter(struct sustain_params *ds, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs);



    struct tremolo_params {
        DspFloatType tremolo_amplitude;
        DspFloatType tremolo_speed;
        DspFloatType tremolo_phase;        
    };

    void tremolo_create(struct tremolo_params * p);
    void tremolo_filter(struct tremolo_params *tp, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs);


    #define MAX_STAGES 4

    struct tubeamp_params {
        /* internal state variables */
        Biquad_t highpass[MAX_STAGES];
        Biquad_t lowpass[MAX_STAGES];
        Biquad_t biaslowpass[MAX_STAGES];
        Biquad_t bq_bass, bq_middle, bq_treble;
        Biquad_t decimation_filter;

        /* user parameters */
        unsigned int stages;
        unsigned int impulse_model;
        unsigned int impulse_quality;
        DspFloatType gain;
        DspFloatType asymmetry;
        DspFloatType biasfactor;
        DspFloatType tone_bass;
        DspFloatType tone_middle;
        DspFloatType tone_treble;
        
        /* internal stuff */
        DspFloatType bias[MAX_STAGES];
        DspFloatType r_i[MAX_STAGES], r_k_p[MAX_STAGES];
        
        /* effect state stuff */
        DspFloatType in[MAX_CHANNELS];
        /* convolution buffer */
        DspFloatType *buf[MAX_CHANNELS];
        int16_t bufidx[MAX_CHANNELS];
    };

    void tubeamp_create(struct tubeamp_params *params);
    void tubeamp_destroy(struct tubeamp_params *params);
    void tubeamp_filter(struct tubeamp_params *params, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs);



    #define MAX_VIBRATO_BUFSIZE 16384

  

    struct vibrato_params {
        Hilbert_t       hilbert;
        DspFloatType           vibrato_amplitude, vibrato_base,
                    vibrato_speed,
                        vibrato_phase,
                        phase;
    };

    void vibrato_create(struct vibrato_params *pvibrato);
    void vibrato_filter(struct vibrato_params *vp, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs);



    struct rotary_params {
        int         speed;
        int         time_to_next_fft, unread_output;
        DspFloatType       phase;
        Biquad_t    ld, rd;
        Hilbert_t   hilb;
    };

    void rotary_filter(struct rotary_params *params, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs);
    void rotary_create(struct rotary_params *params);

    struct pitch_params {
        Backbuf_t      *history[MAX_CHANNELS];
        DspFloatType     *channel_memory[MAX_CHANNELS], *output_memory[MAX_CHANNELS];
        int memory_index;
        DspFloatType output_pos, output_buffer_trigger;
        
        /* user tunables */
        int             halfnote;
        DspFloatType          finetune, drywet;
    };
    void pitch_create(    struct pitch_params *params);
    void pitch_filter(struct pitch_params * params, size_t n, DspFloatType * inputs, DspFloatType * outputs);
    void pitch_destroy(struct pitch_params *params);

};
