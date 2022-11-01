// they're not necessarily the best f/x ever but they got character


#include "hammerzhang.hpp"
#include <cstdio>
#include <algorithm>


using namespace HammerFX;

//DspFloatType sampleRate = 44100.0f;
//DspFloatType invSampleRate = 44100.0f;

///////////////////////////////////////////////////////////////////////////////
// RC Filter
///////////////////////////////////////////////////////////////////////////////



void LC_filter(size_t numSamples, DspFloatType * buffer, int filter_no, DspFloatType freq, struct filter_data *pp)
{
    DspFloatType          R,
                    L,
                    C,
                    dt_div_L,
                    dt_div_C;
    DspFloatType          du,
                    d2i;
    int             t,
                    currchannel = 0;
    DspFloatType           *sound = buffer;
    int channels=1;

    L = 50e-3;			/* 
				 * like original crybaby wahwah, hehehe 
				 */
    C = 1.0 / (4.0 * pow(M_PI * freq, 2.0) * L);
    R = 300.0;

    dt_div_C = 1.0 / (C * sampleRate);
    dt_div_L = 1.0 / (L * sampleRate);

    for (t = 0; t < numSamples; t++) {
	if(isnan(*sound))
	    *sound=0;
	du = (DspFloatType) *sound - pp->last_sample[filter_no][0][currchannel];
	pp->last_sample[filter_no][0][currchannel] = (DspFloatType) *sound;

	d2i =
	    dt_div_L * (du - pp->i[filter_no][0][currchannel] * dt_div_C -
			R * pp->di[filter_no][0][currchannel]);
	pp->di[filter_no][0][currchannel] += d2i;
	pp->i[filter_no][0][currchannel] +=
	    pp->di[filter_no][0][currchannel];

	*sound = (int) (pp->i[filter_no][0][currchannel] * 500.0);
	if(isnan(*sound))
	    *sound=0;
	currchannel = (currchannel + 1) % channels;

	sound++;
    }
}

static DspFloatType other(DspFloatType f, DspFloatType x)
{
    return 1.0 / (2 * M_PI * f * x);
}

void RC_setup(int max, DspFloatType amplify, struct filter_data *pp)
{
    pp->max = max;
    pp->amplify = amplify;

    memset(pp->i, 0, sizeof(pp->i));
    memset(pp->di, 0, sizeof(pp->di));
    memset(pp->last_sample, 0, sizeof(pp->last_sample));
}

void RC_set_freq(DspFloatType f, struct filter_data *pp)
{
    pp->R = 1000.0;
    pp->C = other(f, pp->R);
    pp->invR = 1.0 / pp->R;
    pp->dt_div_C = invSampleRate * pp->C;
}

static void RC_filter(size_t numSamples, DspFloatType * buffer, int mode, int filter_no,
	  struct filter_data *pp)
{
    DspFloatType          du,
                    di;
    int             t,
                    currchannel = 0;
    DspFloatType     *sound = buffer;
    int channels = 1;

    for (t = 0; t < numSamples; t++) {
	if(isnan(*sound))
	    *sound=0;
	du = (DspFloatType) *sound -
	    pp->last_sample[filter_no][mode][currchannel];
	pp->last_sample[filter_no][mode][currchannel] = (DspFloatType) *sound;

	di = pp->invR * (du -
			 pp->i[filter_no][mode][currchannel] *
			 pp->dt_div_C);
	pp->i[filter_no][mode][currchannel] += di;

	if (mode == HIGHPASS)
	    *sound =
		((pp->i[filter_no][mode][currchannel] * pp->R) *
		       pp->amplify);
	else
	    *sound =
		(((DspFloatType) *sound -
			pp->i[filter_no][mode][currchannel] * pp->R) *
		       pp->amplify);
	currchannel = (currchannel + 1) % channels;
	if(isnan(*sound))
	    *sound=0;

	sound++;
    }
}

void RC_bandpass(size_t n, DspFloatType * buffer, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++) {
	RC_filter(n, buffer, HIGHPASS, a, pp);
	RC_filter(n, buffer, LOWPASS, a, pp);
    }
}

void RC_highpass(size_t n, DspFloatType * buffer, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	RC_filter(n, buffer, HIGHPASS, a, pp);
}

void RC_lowpass(size_t n, DspFloatType * buffer, struct filter_data *pp)
{
    int             a;

    for (a = 0; a < pp->max; a++)
	    RC_filter(n, buffer, LOWPASS, a, pp);
}

///////////////////////////////////////////////////////////////////////////////
// Autowah
///////////////////////////////////////////////////////////////////////////////
#define AUTOWAH_HISTORY_LENGTH  100  /* ms */
#define AUTOWAH_DISCANT_TRIGGER 0.60 /* dB */
#define AUTOWAH_BASS_TRIGGER    0.65 /* dB */

Backbuf_t * new_Backbuf(const unsigned int size);
void del_Backbuf(Backbuf_t *b);

static void backbuf_add(Backbuf_t *b, const BUF_TYPE d)
{
    b->curpos += 1;
    b->storage[b->curpos & b->mask] = d;
}

static BUF_TYPE
backbuf_get(Backbuf_t *b, const unsigned int delay)
{
    assert(delay < b->nstor);
    return b->storage[(b->curpos - delay) & b->mask];
}

/* XXX optimize this a bit */
static BUF_TYPE
backbuf_get_interpolated(Backbuf_t *b, DspFloatType delay)
{
    unsigned int delay_int = delay;
    unsigned int getpos;
    
    delay -= delay_int;
    getpos = b->curpos - delay_int;
    return b->storage[getpos & b->mask] * (1 - delay) + b->storage[(getpos - 1) & b->mask] * delay;
}

static void
backbuf_clear(Backbuf_t *b)
{
    memset(b->storage, 0, (b->mask + 1) * sizeof(b->storage[0]));
}

Backbuf_t *
new_Backbuf(const unsigned int max_delay)
{
    unsigned int size;
    
    Backbuf_t *b = (Backbuf_t*)calloc(1, sizeof(Backbuf_t));

    b->nstor = max_delay + 1;
    size = 1;
    while (size < b->nstor)
        size <<= 1;
    b->storage = (DspFloatType*)calloc(size, sizeof(BUF_TYPE));
    b->mask = size - 1;
    b->curpos = 0;
    b->add = backbuf_add;
    b->get = backbuf_get;
    b->get_interpolated = backbuf_get_interpolated;
    b->clear = backbuf_clear;
    return b;
}

void
del_Backbuf(Backbuf_t *b)
{
    free(b->storage);
    free(b);
}


static DspFloatType power2db(DspFloatType power)
{
    return log(power) / log(10) * 10;
}

#define MAX_sampleRate 192000
#define MAX_SAMPLE (32767 << 8)
//#define MAX_CHANNELS 8

/*
static const char *methods[] = {
    "Lowpass",
    "Bandpass",
    "Moog ladder",
    "TB-303 style",
    NULL
};

static const char *syncs[] = {
    "Envelope",
    "Continuous",
    "MIDI continous control",
    NULL
};

static void
update_wah_speed(GtkAdjustment *adj, struct autowah_params *params)
{
    params->sweep_time = adj->value;
}

static void
update_wah_freqlow(GtkAdjustment *adj, struct autowah_params *params)
{
    params->freq_low = adj->value;
}

static void
update_wah_freqhi(GtkAdjustment *adj, struct autowah_params *params)
{
    params->freq_high = adj->value;
}

static void
update_wah_drywet(GtkAdjustment *adj, struct autowah_params *params)
{
    params->drywet = adj->value;
}

static void
update_wah_res(GtkAdjustment *adj, struct autowah_params *params)
{
    params->res = adj->value;
}

static void
update_wah_sync(GtkWidget *w, struct autowah_params *params)
{
    int i;
    const char *tmp;
    
    tmp = gtk_entry_get_text(GTK_ENTRY(w));
    if (tmp == NULL)
        return;

    for (i = 0; syncs[i] != NULL; i += 1) {
	if (strcmp(tmp, syncs[i]) == 0) {
	    params->sync = i;
	    break;
	}
    }

    
    gtk_widget_set_sensitive(GTK_WIDGET(params->w_sweep), params->sync != 2);
}

static void
update_method(GtkWidget *w, struct autowah_params *params)
{
    int i;
    const char *tmp;
    
    tmp = gtk_entry_get_text(GTK_ENTRY(w));
    if (tmp == NULL)
        return;

    for (i = 0; methods[i] != NULL; i += 1) {
	if (strcmp(tmp, methods[i]) == 0) {
	    params->method = i;
	    break;
	}
    }
    
    gtk_widget_set_sensitive(GTK_WIDGET(params->w_resonance), params->method != 1);
}

static void toggle_effect_custom(GtkWidget *w, effect_t *p) {
    struct autowah_params *params = p->params;
    toggle_effect(w, p);
    gtk_widget_set_sensitive(GTK_WIDGET(params->w_control), p->toggle);
}

static void tblattach(GtkWidget *table, GtkWidget *widget, int x, int y) {
    int attachopts = GTK_EXPAND;
    if ((x == 0 || x == 2)
        && !(y == 0 && x == 2)) { 
        attachopts = 0;
        gtk_misc_set_alignment(GTK_MISC(widget), 0, 0.5);
    }

    gtk_table_attach(GTK_TABLE(table), (widget), (x), (x)+1, (y), (y)+1,
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK | attachopts),
                     __GTKATTACHOPTIONS(GTK_FILL | GTK_SHRINK), 3, 3);
}

autowah_init(struct effect *p)
{
    int i;
    struct autowah_params *params = p->params;

    GtkWidget      *label;
    GtkWidget      *widget;
    GtkObject      *adj;

    GtkWidget      *button;
    GtkWidget      *table;

    GList          *glist_methods = NULL;
    
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_signal_connect(GTK_OBJECT(p->control), "delete_event",
		       GTK_SIGNAL_FUNC(delete_event), p);
    table = gtk_table_new(3, 6, FALSE);
    
    label = gtk_label_new("Wah control:");
    tblattach(table, label, 0, 0);
    for (i = 0; syncs[i] != NULL; i += 1)
        glist_methods = g_list_append(glist_methods, (gchar *) syncs[i]);
    widget = gtk_combo_new();
    gtk_combo_set_popdown_strings(GTK_COMBO(widget), glist_methods);
    g_list_free(glist_methods);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(widget)->entry), FALSE);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(widget)->entry), syncs[params->sync]);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(widget)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_wah_sync), params);
    tblattach(table, widget, 1, 0);

    params->w_control = widget;
    
    button = gtk_check_button_new_with_label("On");
    if (p->toggle)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_effect_custom), p);
    tblattach(table, button, 2, 0);
    gtk_widget_set_sensitive(GTK_WIDGET(params->w_control), p->toggle);

    label = gtk_label_new("Period:");
    tblattach(table, label, 0, 1);
    adj = gtk_adjustment_new(params->sweep_time, 100.0,
                             10000.0, 1.0, 10.0, 0.0);
    gtk_signal_connect(GTK_OBJECT(adj), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_speed), params);
    widget = gtk_hscale_new(GTK_ADJUSTMENT(adj));
    gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
    tblattach(table, widget, 1, 1);
    label = gtk_label_new("ms");
    tblattach(table, label, 2, 1);
    
    params->w_sweep = widget; 
    gtk_widget_set_sensitive(GTK_WIDGET(params->w_sweep), params->sync != 2);

    label = gtk_label_new("First frequency:");
    tblattach(table, label, 0, 3);
    adj = gtk_adjustment_new(params->freq_high,
			     80.0, 5000.0, 1.0, 1.0, 0.0);
    gtk_signal_connect(GTK_OBJECT(adj), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_freqhi), params);
    widget = gtk_hscale_new(GTK_ADJUSTMENT(adj));
    gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
    tblattach(table, widget, 1, 3);
    label = gtk_label_new("Hz");
    tblattach(table, label, 2, 3);

    label = gtk_label_new("Last frequency:");
    tblattach(table, label, 0, 2);
    adj = gtk_adjustment_new(params->freq_low,
			     80.0, 5000.0, 1.0, 1.0, 0.0);
    gtk_signal_connect(GTK_OBJECT(adj), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_freqlow), params);
    widget = gtk_hscale_new(GTK_ADJUSTMENT(adj));
    gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
    tblattach(table, widget, 1, 2);
    label = gtk_label_new("Hz");
    tblattach(table, label, 2, 2);

    label = gtk_label_new("Wah type:");
    tblattach(table, label, 0, 4);
    glist_methods = NULL;
    for (i = 0; methods[i] != NULL; i += 1)
        glist_methods = g_list_append(glist_methods, (gchar *) methods[i]);
    widget = gtk_combo_new();
    gtk_combo_set_popdown_strings(GTK_COMBO(widget), glist_methods);
    g_list_free(glist_methods);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(widget)->entry), FALSE);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(widget)->entry), methods[params->method]);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(widget)->entry),
		       "changed", GTK_SIGNAL_FUNC(update_method), params);
    tblattach(table, widget, 1, 4);

    label = gtk_label_new("Filter resonance:");
    tblattach(table, label, 0, 5);
    adj = gtk_adjustment_new(params->res,
			     30.0, 100.0, 10, 30, 0.0);
    widget = gtk_hscale_new(GTK_ADJUSTMENT(adj));
    gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
    gtk_signal_connect(GTK_OBJECT(adj), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_res), params);
    tblattach(table, widget, 1, 5);
    label = gtk_label_new("%");
    tblattach(table, label, 2, 5);
    
    params->w_resonance = widget;
    gtk_widget_set_sensitive(GTK_WIDGET(params->w_resonance), params->method != 1);
    
    label = gtk_label_new("Dry / Wet:");
    tblattach(table, label, 0, 6);
    adj = gtk_adjustment_new(params->drywet,
		             0.0, 100.0, 1.0, 5.0, 0.0);
    widget = gtk_hscale_new(GTK_ADJUSTMENT(adj));
    gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
    gtk_signal_connect(GTK_OBJECT(widget), "value_changed",
		       GTK_SIGNAL_FUNC(update_wah_drywet), params);
    tblattach(table, widget, 1, 6);
    label = gtk_label_new("%");
    tblattach(table, label, 2, 6);
    
    gtk_window_set_title(GTK_WINDOW(p->control), "Autowah");
    gtk_container_add(GTK_CONTAINER(p->control), table);

    gtk_widget_show_all(p->control);
}
*/


void autowah_init(autowah_params * ap) {        
    ap->history = new_Backbuf(MAX_sampleRate * AUTOWAH_HISTORY_LENGTH / 1000);
    ap->method = 0; /* low-pass resonant filter */
    ap->freq_low = 280;
    ap->freq_high = 900;
    ap->sweep_time = 500;
    ap->drywet = 100;
    ap->sync = 0;
    ap->res = 85;        
    ap->num_channels=1;
}
void autowah_destroy(autowah_params * ap) {
    del_Backbuf(ap->history);    
    //gtk_widget_destroy(p->control);    
}
#define SIN_LOOKUP_SIZE         65536
DspFloatType sin_lookup_table[SIN_LOOKUP_SIZE+1];

static void init_sin_lookup_table(void) {
    int i = 0;
    for (i = 0; i < SIN_LOOKUP_SIZE + 1; i += 1)
        sin_lookup_table[i] = sin(2 * M_PI * i / SIN_LOOKUP_SIZE);
}

static inline DspFloatType sin_lookup(DspFloatType pos) {
    return sin_lookup_table[(int) (pos * (DspFloatType) SIN_LOOKUP_SIZE)];
}


static inline DspFloatType cos_lookup(DspFloatType pos) {
    if (pos >= 0.75f)
        return sin_lookup(pos - 0.75f);
    else
        return sin_lookup(pos + 0.25f);
}


void autowah_filter(autowah_params *ap, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{
    
    int             i, curr_channel = 0, delay_time;
    DspFloatType           freq, g, g2;
    

    //memcpy(db->data_swap, db->data, db->len * sizeof(DspFloatType));

    if (ap->sync == 1) { /* continuous sweep */
        /* recover from noncontinuous sweep */
        if (ap->dir == 0)
            ap->dir = 1.0;
        
        if (ap->f > 1.0f && ap->dir > 0)
            ap->dir = -1;
        if (ap->f < 0.0f && ap->dir < 0)
            ap->dir = 1;
    }
    if (ap->sync == 0) { /* envelope detect */
        /* Firstly, quiesce wah if we have reached the end of sweep */
        if (ap->f < 0.0f) {
            ap->f = 0.0;
            ap->dir = 0;
        }
        delay_time = sampleRate * AUTOWAH_HISTORY_LENGTH / 1000;
        
        /* Estimate signal higher frequency content's power. When user picks
         * the string strongly it's the high frequency content that increases
         * most. */

        /* XXX we should probably treat all channels separately.
         * We just skip all channels but the first presently. */
        
        

        for (i = 0; i < numSamples; i++) { 
            outputs[i] = inputs[i];
            if (curr_channel == 0) {
            ap->delayed_accum_power += pow(ap->history->get(ap->history, delay_time), 2);
            
            ap->fresh_accum_power += pow(outputs[i], 2);
            
            ap->delayed_accum_delta +=
                pow(ap->history->get(ap->history, delay_time) -
                    ap->history->get(ap->history, delay_time - 1), 2);
            
            ap->fresh_accum_delta +=
                    pow(outputs[i] - ap->history->get(ap->history, 0), 2);

            ap->history->add(ap->history, outputs[i]);
            
            ap->accum_n += 1;
            if (ap->accum_n > 8192) {
                ap->fresh_accum_power   /= 2; 
                ap->fresh_accum_delta   /= 2; 
                ap->delayed_accum_power /= 2; 
                ap->delayed_accum_delta /= 2;
                ap->accum_n             /= 2;
            }
        } curr_channel = (curr_channel + 1) % ap->num_channels; }

        /* I skip some scale factors here that would cancel out */
        if ((power2db(ap->fresh_accum_delta) > power2db(ap->delayed_accum_delta)
                                              + AUTOWAH_DISCANT_TRIGGER) ||
            (power2db(ap->fresh_accum_power) > power2db(ap->delayed_accum_power)
                                              + AUTOWAH_BASS_TRIGGER)) {
            ap->f = 1.0f;
            ap->dir = -1.0;
        }
    }
    /*
    if (ap->sync == 2) { // midi control, from Roland FC-200 or somesuch 
        ap->f = midi_get_continuous_control(-1);
        ap->dir = 0;
        ap->freq_vibrato = 0;
    }
    */

    /* in order to have audibly linear sweep, we must map
     * [0..1] -> [freq_low, freq_high] linearly in log2, which requires
     * f(x) = a * 2 ^ (b * x)
     *
     * we know that f(0) = freq_low, and f(1) = freq_high. It follows that:
     * a = freq_low, and b = log2(freq_high / freq_low)
     */

    ap->smoothed_f = (ap->f + ap->smoothed_f) / 2.f;
    freq = ap->freq_low * pow(2, log(ap->freq_high / ap->freq_low)/log(2) * ap->smoothed_f + 0.2 * sin_lookup(ap->freq_vibrato));
    ap->f += ap->dir * 1000.0f / ap->sweep_time * numSamples / (sampleRate * ap->num_channels) * 2;

    ap->freq_vibrato += 1000.0f / ap->sweep_time * numSamples / (sampleRate * ap->num_channels) / 2.0f;
    if (ap->freq_vibrato >= 1.0f)
        ap->freq_vibrato -= 1.0f;
    
    switch (ap->method) {
      case 0:
        /* lowpass resonant filter -- we must avoid setting value 0 to
         * resonance. We also drop level by 6 dB to leave some room for it. */
        set_lpf_biquad(sampleRate, freq, 1.1 + -ap->res / 100.0, &ap->lpf);
        for (i = 0; i < numSamples; i += 1) {
            outputs[i] = do_biquad(outputs[i], &ap->lpf, curr_channel) / 2;
            curr_channel = (curr_channel + 1) % ap->num_channels;
        }
        break;
        
      case 1: 
        set_bpf_biquad(sampleRate, freq, 1.1 + -ap->res / 100.0, &ap->bpf);
        for (i = 0; i < numSamples; i += 1) {
            outputs[i] = do_biquad(outputs[i], &ap->bpf, curr_channel);
            curr_channel = (curr_channel + 1) % ap->num_channels;
        }
        break;

      case 2:
      case 3:
        /* Moog ladder filter according to Antti Huovilainen. */

/* I, C, V = electrical parameters
 * f = center frequency
 * r = resonance amount 0 .. 1
 *
 * ya(n) = ya(n-1) + I / (C * f) * (tanh( (x(n) - 4 * r * yd(n-1)) / (2 * V) ) - Wa(n-1))
 * yb(n) = yb(n-1) + I / (C * f) * (Wa(n) - Wb(n-1))
 * yc(n) = yc(n-1) + I / (C * f) * (Wb(n) - Wc(n-1))
 * yd(n) = yd(n-1) + I / (C * f) * (Wc(n) - Wd(n-1))
 *
 * Wx = tanh(Yx(n) / (2 * Vt)) */

        g = 1.f - expf((DspFloatType) (-2.0 * M_PI) * freq / sampleRate);
        g2 = g;
        /* TB-303 style: the first phase is one octave higher than rest */
        if (ap->method == 3)
            g2 = 1.f - expf((DspFloatType) (-2.0 * M_PI) * 2 * freq / sampleRate);
        for (i = 0; i < numSamples; i += 1) {
#define PARAM_V (MAX_SAMPLE * 1.0) 
            ap->ya[curr_channel] += (DspFloatType) PARAM_V * g2 *
                (tanhf( (outputs[i] - 4.f * ap->res/100.0f * ap->yd[curr_channel]) / (DspFloatType) PARAM_V )
                 - tanhf( ap->ya[curr_channel] / (DspFloatType) PARAM_V));
            ap->yb[curr_channel] += (DspFloatType) PARAM_V * g *
                (tanhf( ap->ya[curr_channel] / (DspFloatType) PARAM_V )
                 - tanhf( ap->yb[curr_channel] / (DspFloatType) PARAM_V ));
            ap->yc[curr_channel] += (DspFloatType) PARAM_V * g *
                (tanhf( ap->yb[curr_channel] / (DspFloatType) PARAM_V )
                 - tanhf( ap->yc[curr_channel] / (DspFloatType) PARAM_V ));
            ap->yd[curr_channel] += (DspFloatType) PARAM_V * g *
                (tanhf( ap->yc[curr_channel] / (DspFloatType) PARAM_V )
                 - tanhf( ap->yd[curr_channel] / (DspFloatType) PARAM_V ));

            /* the wah causes a gain loss of 12 dB which, but due to
             * resonance we may clip; to compromise I'll adjust 6 dB back. */
            outputs[i] = ap->yd[curr_channel] * 2.f;
            curr_channel = (curr_channel + 1) % ap->num_channels;
        }
        break;

      default:
        break;
    }
    
    /* mix with dry sound */
    for (i = 0; i < numSamples; i++)
        outputs[i] = (outputs[i]*ap->drywet + inputs[i]*(100.f-ap->drywet))/100.0f;
}


/////////////////////////////////////////////////////////////////
// Hammerzhang Chorus
/////////////////////////////////////////////////////////////////


#define MAX_DEPTH       20
#define MAX_BASEDELAY   50


void chorus_filter_mono(chorus_params * cp, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{    
    int             count, i, curr_channel = 0;
    DspFloatType          dly, Speed, tmp_ang, Depth, BaseDelay, Dry, Wet, Rgn;
    DspFloatType     *s;
    DspFloatType      tmp, rgn;    
    s = inputs;
    count = numSamples;

    Dry = 1 - cp->drywet / 100.0;
    Wet =     cp->drywet / 100.0;
    Rgn = cp->regen / 100.0;
    Speed = 1000.0 / cp->speed / sampleRate;
    Depth = cp->depth / 1000.0 * sampleRate;
    BaseDelay = cp->basedelay / 1000.0 * sampleRate;

    while (count) {
        tmp = 0.0;
        tmp_ang = cp->ang;
        for (i = 0; i < cp->voices; i += 1) {
            dly = BaseDelay * i / cp->voices + Depth * (1 + sin_lookup(tmp_ang)) / 2.0;
            tmp += cp->history[curr_channel]->get_interpolated(cp->history[curr_channel], dly) / sqrt(cp->voices);
            tmp_ang += 1.0 / cp->voices;
            if (tmp_ang >= 1.0)
                tmp_ang -= 1.0;
        }
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(tmp)
#endif
        /* XXX regen sounds generally bad, maybe we should take it away? */
        dly = BaseDelay + Depth * (1 + sin_lookup(cp->ang)) / 2.0;
        rgn = cp->history[curr_channel]->get_interpolated(cp->history[curr_channel], dly) * Rgn + *s;
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(rgn)
#endif
        cp->history[curr_channel]->add(cp->history[curr_channel], rgn);
	    outputs[count] = *s * Dry / sqrt(cp->voices) + tmp * Wet;

	    curr_channel = (curr_channel + 1) % 1;
        if (curr_channel == 0) {
            cp->ang += Speed;
            if (cp->ang >= 1.0)
                cp->ang -= 1.0;
        }

	    s++;
	    count--;
    }
}

void chorus_create(chorus_params * cp)
{
    for (size_t i = 0; i < MAX_CHANNELS; i++)
	    cp->history[i] = new_Backbuf((MAX_DEPTH + MAX_BASEDELAY) / 1000.0 * MAX_sampleRate);

    cp->ang = 0.0;
    cp->depth = 2.5;
    cp->basedelay = 3.5;
    cp->voices = 3;
    cp->speed = 1000;
    cp->drywet = 50;
    cp->regen = 0;    
}

void chorus_destroy(struct chorus_params * cp)
{
    int             i;
    for (i = 0; i < MAX_CHANNELS; i++)
	    del_Backbuf(cp->history[i]);    
}



///////////////////////////////////////////////////////////////////////////////
// Hammerzhang Delay
///////////////////////////////////////////////////////////////////////////////
#define MAX_COUNT 10
#define MAX_SECONDS 10
#define MAX_STEP (MAX_sampleRate * MAX_SECONDS)
#define MAX_SIZE (MAX_STEP * (MAX_COUNT + 1))




void delay_create(delay_params * pdelay)
{
    int                 i;
    pdelay->multichannel = 0;
    pdelay->delay_decay = 55;
    pdelay->delay_time = 1000;
    pdelay->delay_count = 8;
    pdelay->numChannels = 1;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        pdelay->history[i] = new_Backbuf(MAX_SIZE);    
}

void delay_destroy(struct delay_params * dp)
{    
    int i;    
    for (i = 0; i < MAX_CHANNELS; i += 1)
        del_Backbuf(dp->history[i]);    
}

void delay_filter_mono(delay_params * dp, size_t n, DspFloatType * inputs, DspFloatType * outputs)
{    
    int             i,
                    count,
                    current_delay = 0;
    DspFloatType          current_decay, delay_inc, decay_fac;
    DspFloatType           *s, *t, newval;
    int             curr_channel=0;
    count = n;

    delay_inc = dp->delay_time / 1000.0 * sampleRate;
    decay_fac = dp->delay_decay / 100.0;
    
    s = inputs;
    t = outputs;
    /* this is a simple mono version that treats all channels separately */
    while (count) {
        newval = 0;
        current_delay = 0;
        current_decay = 1.0;
        *t = *s;
        for (i = 0; i < dp->delay_count; i += 1) {
            current_delay += delay_inc;
            current_decay *= decay_fac;
            newval += dp->history[curr_channel]->get(dp->history[curr_channel], current_delay) * current_decay;
    	}
        
        /* write to history, add decay to current sample */
        dp->history[curr_channel]->add(dp->history[curr_channel], *s);
        *t += newval;

        curr_channel = (curr_channel + 1) % dp->numChannels;
        s++;
        t++;
        count--;
    }
}




///////////////////////////////////////////////////////////////////////////////
// Hammerzhang Distortion
///////////////////////////////////////////////////////////////////////////////


void distort_filter(struct distort_params * dp, size_t numSamples, DspFloatType * buffer)
{
    int             count,
                    currchannel = 0;
    DspFloatType      *s;    
    DspFloatType	    t;
    int channels = 1;
    
    /*
     * sat clips derivative by limiting difference between samples. Use lastval 
     * member to store last sample for seamlessness between chunks. 
     */
    count = numSamples;
    s = buffer;

    RC_highpass(numSamples, s, &(dp->fd));

    while (count) {
	/*
	 * apply drive  
	 */
	t=*s;
	t *= dp->drive;
	t /= 16;

	/*
	 * apply sat 
	 */
	if ((t - dp->lastval[currchannel]) > (dp->sat << 8))
	    t = dp->lastval[currchannel] + (dp->sat << 8);
	else if ((dp->lastval[currchannel] - t) > (dp->sat << 8))
	    t = dp->lastval[currchannel] - (dp->sat << 8);

	dp->lastval[currchannel] = t;
        currchannel = (currchannel + 1) % channels;
        
	t *= dp->level;
	t /= 256;
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(t)
#endif
	if(isnan(t))
	    t=0;
	*s=t;

	s++;
	count--;
    }

    LC_filter(numSamples, s, 0, dp->lowpass, &(dp->noise));
}

void distort_destroy(struct delay_params * p)
{    
    
}


void distort_create(struct distort_params *ap)
{
    ap->sat = 10000;
    ap->level = 20;
    ap->drive = 555;
    ap->lowpass = 350;
    ap->noisegate = 3000;

    RC_setup(10, 1.5, &(ap->fd));
    RC_set_freq(ap->lowpass, &(ap->fd));

    RC_setup(10, 1, &(ap->noise));
    RC_set_freq(ap->noisegate, &(ap->noise)); 
}


/////////////////////////////////////////////////////
// distort2 tubescream
/////////////////////////////////////////////////////
#define RC_FEEDBACK_R          4.7e3
#define RC_FEEDBACK_C          47e-9
#define RC_DRIVE_C             51e-12   /* farads */
#define DRIVE_STATIC           50e3     /* ohms */
#define DRIVE_LOG              500e3    /* ohms */
 
#define MAX_NEWTON_ITERATIONS   20      /* limits the time looking for convergence */
#define UPSAMPLING_RATE         4       /* you can't change this, see upsample[] */
 
/* the effect is defined in -1.0 .. 1.0 range */
#define DIST2_DOWNSCALE		(1.0 / MAX_SAMPLE)
#define DIST2_UPSCALE		(MAX_SAMPLE / 1.0)
/* when we have converged within one 16-bit sample, accept value */
#define EFFECT_PRECISION	(1.0 / 32768)

/* Check if the compiler is Visual C or GCC */
#if defined(_MSC_VER)
#   pragma intrinsic (exp)
#   define expf(x) exp(x)
#endif

static void
distort2_filter(struct distort2_params *dp, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{

    int32_t            i,count,bailout;
    int8_t             curr_channel = 0;
    DspFloatType    	       *s = inputs;    
    DspFloatType * o = outputs;
    DspFloatType x,y,x1,x2,f,df,dx,e1,e2;
    DspFloatType upsample[UPSAMPLING_RATE];
    DspFloatType DRIVE = (DspFloatType) DRIVE_STATIC + dp->drive / 100.0f * (DspFloatType) DRIVE_LOG;
    DspFloatType mUt = (20.0f + 100.f - 70.f) * 1e-3f;
    /* correct Is with mUt to approximately keep drive the
     * same. Original parameters said Is is 10e-12 and mUt 30e-3.
     * If mUt grows, Is must shrink. 0.39 is experimental */
    DspFloatType Is = 10e-12f * expf(0.39f/30e-3f - 0.39f/mUt);
    size_t channels =1;
    count = numSamples;
    

    /* no input, no output :-) to avoid extra calc. Optimized for noise gate,
     * when all input is zero.
     * This is the heuristics - since there is no the standard function
     * in the ANSI C library that reliably compares the memory region
     * with the given byte, we compare just a few excerpts from an array.
     * If everything is zero, we have a large chances that all array is zero. */
    if(s[0]==0 && s[1]==0 && s[16]==0 && s[17]==0 &&
          s[24]==0 && s[25]==0 && s[32]==0 && s[33]==0 && s[numSamples-1]==0) {
        for (i = 0; i < MAX_CHANNELS; i += 1) {
            dp->last[i] = 0;
        }
        return;
    }

    set_rc_lowpass_biquad(sampleRate * UPSAMPLING_RATE, 720, &dp->rolloff);
    /*
     * process signal; x - input, in the range -1, 1
     */
    while (count) {
        /* "properly" interpolate previous input at positions 0 and 2 */
        fir_interpolate_2x(dp->interpolate_firmem[curr_channel],
                           *s, &upsample[2], &upsample[0]);
        /* estimate the rest, this should be good enough for our purposes. */
        upsample[1] = (upsample[0] + upsample[2]) / (DspFloatType) 2;
        /* looking into firmem is a gross violation of interface. This will
         * go away once I design fir_interpolate_4x. */
        upsample[3] = ((DspFloatType) 3 * upsample[2] + dp->interpolate_firmem[curr_channel][2]) / (DspFloatType) 4;

        /* Now the actual upsampled processing */
        for (i = 0; i < UPSAMPLING_RATE; i++)
        {
            /* scale down to -1 .. 1 range */
            x = upsample[i] * (DspFloatType) DIST2_DOWNSCALE;
            
            /* first compute the linear rc filter current output */
            x2 = do_biquad(x, &dp->feedback_minus_loop, curr_channel);
                
            x1 = (x - x2) / (DspFloatType) RC_FEEDBACK_R;
                
            /* start searching from time previous point , improves speed */
            y = dp->last[curr_channel];
                /* limit iterations if the algorithm fails to converge */
                bailout = MAX_NEWTON_ITERATIONS;
            do {
            /* f(y) = 0 , y= ? */
                    /* e^3 ~ 20 */
                    e1 = expf((x - y) / mUt);
                    e2 = 1.0f / e1;
            /* f=x1+(x-y)/DRIVE+Is*(exp((x-y)/mUt)-exp((y-x)/mUt));  optimized makes : */
            f = x1 + (x - y) / DRIVE + Is * (e1 - e2);
        
            /* df/dy */
            /*df=-1.0/DRIVE-Is/mUt*(exp((x-y)/mUt)+exp((y-x)/mUt)); optimized makes : */
            df = -1.0f / DRIVE - Is / mUt * (e1 + e2);
        
            /* This is the newton's algo, it searches a root of a function,
            * f here, which must equal 0, using it's derivate. */
            dx = f/df;
            y -= dx;
            }
            while (fabs(dx) > (DspFloatType) EFFECT_PRECISION && --bailout);
            /* when dx gets very small, we found a solution. */
            
            /* Ensure that the value gets reset if something goes wrong */
            if (isnan(y) || ! (y >= -2.0f && y <= 2.0f))
                y = 0.f;
            
	        dp->last[curr_channel] = y;
            /* static lowpass filtering -- this floats as our decimation filter
             * the rolloff is at 720 Hz, but is only 6 dB/oct. */
            y = do_biquad(y, &dp->rolloff, curr_channel);
	    }
        /* treble control + other final output filtering */
        y += (y - do_biquad(y, &dp->treble_highpass, curr_channel)) * dp->treble / 3.0f;
        y = do_biquad(y, &dp->output_bass_cut, curr_channel);
	
        /* scale up from -1..1 range */
	    *o = y * (DspFloatType) DIST2_UPSCALE * (dp->clip / 100.0f);

        /*if(*s > MAX_SAMPLE)
            *s=MAX_SAMPLE;
        else if(*s < -MAX_SAMPLE)
            *s=-MAX_SAMPLE;*/

        s++;
        o++;
        count--;

        curr_channel = (curr_channel + 1) % channels;
    }
}

void distort2_destroy(struct distort2_params *ap)
{    

}

void distort2_create(struct distort2_params *ap)
{        
    ap->drive = 0.0;
    ap->clip = 100.0;
    ap->treble = 6.0;

    /* static shapers */
    set_rc_lowpass_biquad(sampleRate, 3200, &ap->treble_highpass);
    set_rc_lowpass_biquad(sampleRate * UPSAMPLING_RATE, 
            1 / (2 * M_PI * RC_FEEDBACK_R * RC_FEEDBACK_C),
            &ap->feedback_minus_loop);
    set_rc_highpass_biquad(sampleRate, 160, &ap->output_bass_cut);
    
}


/////////////////////////////////////////////////////
// echo
/////////////////////////////////////////////////////

#define ECHO_FIRST_PRIME                    20
#define ECHO_NEXT_PRIME_DISTANCE_FACTOR	    1.6
#define ECHO_CROSSMIX_ATTN                  10.0

static int
is_prime(int n)
{
    int             i;

    for (i = 2; i < n; i++)
	if (n % i == 0)
	    return 0;

    return 1;
}


void echo_filter(struct echo_params  *params, size_t numSamples, DspFloatType * input, DspFloatType * output)
{
    unsigned int i, count, curr_channel = 0;
    DspFloatType *s, tmp;
    DspFloatType in, out, echo_samples, echo_decay;
    
    int delay_lookup[MAX_ECHO_COUNT];
    DspFloatType decay_lookup[MAX_ECHO_COUNT];
    DspFloatType *outs = output;
    int channels=1;

    s = input;    
    count = numSamples;

    if (params->echoes > MAX_ECHO_COUNT) {
        printf("Warning: max echo count exceeded: %u\n", params->echoes);
        params->echoes = MAX_ECHO_COUNT;
    }

    echo_samples = params->echo_size / 1000.0 * sampleRate;
    echo_decay   = params->echo_decay / 100.0;

    for (i = 0; i < params->echoes; i += 1) {
        delay_lookup[i] = echo_samples * params->size_factor[i];
        decay_lookup[i] = pow(echo_decay, params->decay_factor[i]);
    }

    while (count) {
        /* mix current input into the various echo buffers at their
         * expected delays */
        in = *s;
        out = in;
        for (i = 0; i < params->echoes; i += 1) {
            tmp = params->history[curr_channel][i]->get(params->history[curr_channel][i], delay_lookup[i]) * decay_lookup[i];
            out += tmp;
            if (params->echoes > 1) {
                if (i > 1)
                    tmp += params->history[0][i-1]->get(params->history[0][i-1], delay_lookup[i-1]) * decay_lookup[i-1] / ECHO_CROSSMIX_ATTN;
                else
                    tmp += params->history[0][params->echoes-1]->get(params->history[0][params->echoes-1], delay_lookup[params->echoes-1]) * decay_lookup[params->echoes-1] / ECHO_CROSSMIX_ATTN;
            }
            params->history[curr_channel][i]->add(params->history[curr_channel][i], in + tmp);
        }
        *outs = out;
        
        curr_channel = (curr_channel + 1) % channels; 
        s++;
        outs++;
        count--;
    }
}


void echo_destroy(struct echo_params *params )
{    
    int             i, j;

    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
        for (j = 0; j < MAX_CHANNELS; j += 1) {
            del_Backbuf(params->history[j][i]);
        }
    }    
}

void echo_create(struct echo_params *params)
{    
    int             i, j, k;
    
    params->multichannel = 1;
    params->echo_size = 200;
    params->echo_decay = 30.0;
    params->echoes = MAX_ECHO_COUNT;

    /* find some primes to base echo times on */
    k = ECHO_FIRST_PRIME;
    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
	while (! is_prime(k))
	    k += 1;
        params->primes[i] = k;
        k *= ECHO_NEXT_PRIME_DISTANCE_FACTOR;
    }
    /* scale primes such that largest value is 1.0 in both */
    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
        params->size_factor[i] = params->primes[i] / params->primes[0];
        params->decay_factor[i] = params->primes[i] / params->primes[0];
    }
    /* build history buffers, one per channel per echo */
    /* with 20 voices, 0.5 s max buffer, 48 kHz sample rate
     * and 4 bytes per sample we need approx. 1 MB */
    for (i = 0; i < MAX_ECHO_COUNT; i += 1) {
        for (j = 0; j < MAX_CHANNELS; j += 1) {
            params->history[j][i] = new_Backbuf(MAX_ECHO_LENGTH / 1000.0 * MAX_sampleRate * params->size_factor[i]);
        }
    }
    
}


/////////////////////////////////////////////////////
// eq
/////////////////////////////////////////////////////

/* Number of filters in bank */
#define FB_NB 14
/* Minimal value in Decibels ( for UI purpose only) */
#define FB_MIN -9.9
/* Maximal Value in Decibels */
#define FB_MAX 9.9

static const int fb_cf[FB_NB] =
    {40,100,200,320,640,1000,1600,2200,3000,4000,6000,8000,12000,16000};
/* Array with the bandwidths of each filter in Hertz */
static const int fb_bw[FB_NB] =
    {30,120,160,320,640,800,1200,1200,1400,1800,2600,4000,6000,8000};


void eqbank_filter(struct eqbank_params *params, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{
    int                 count;
    DspFloatType         *s;    
    DspFloatType		  ocoeff;
    int			  cchannel=0;
	DspFloatType * o = outputs;
    int channels=1;

    count = numSamples;
    s = inputs;

    ocoeff = pow(10, params->volume / 20.0);
    while (count) {
        int i;
        DspFloatType out1, out2;
        /* using 2x upsampling */
        fir_interpolate_2x(params->history_in[cchannel], *s, &out1, &out2);
	    for (i = 0; i < FB_NB; i++) {
            /* out1 is the sample before out2 */
	        out1 = do_biquad(out1, &params->filters[i], cchannel);
            out2 = do_biquad(out2, &params->filters[i], cchannel);
        }
        /* downsample back to 1x -- observe reversed ordering */
        *o = fir_decimate_2x(params->history_out[cchannel], out1, out2) * ocoeff;

	cchannel = (cchannel + 1) % channels;
	s++;
    o++;
	count--;
    }
}

static void
eqbank_done(struct eqbank_params *p)
{    
    
}


/////////////////////////////////////////////////////
// Phaser
/////////////////////////////////////////////////////

#define PHASOR_UPDATE_INTERVAL 8
#define PHASOR_SHAPE 0.7


void phasor_filter_mono(struct phasor_params *params, size_t numSamples, DspFloatType * inputs,DspFloatType * outputs)
{    
    DspFloatType     *s, tmp;
    DspFloatType * out = outputs;
    int16_t    count, curr_channel = 0, i;
    DspFloatType           delay, Dry, Wet, f;
    int channels=1;

    count = numSamples;
    s = inputs;

    Wet = params->drywet / 100.0f;
    Dry = 1.f - Wet;
    f = params->f;

    while (count) {
        if (curr_channel == 0 && count % PHASOR_UPDATE_INTERVAL == 0) { 
            f += 1000.0f / params->sweep_time / sampleRate * PHASOR_UPDATE_INTERVAL;
            if (f >= 1.0f)
                f -= 1.0f;
            delay = (sin_lookup(f) + 1.f) / 2.f;
            delay *= params->depth / 100.0f;
            delay = 1.0f - delay;
            delay = ((exp(PHASOR_SHAPE * delay) - 1.f) / (exp(PHASOR_SHAPE) - 1.f));

            for (i = 0; i < MAX_PHASOR_FILTERS; i += 1)
                set_phaser_biquad(delay, &params->allpass[i]);
        }
        
        tmp = *s;
        for (i = 0; i < MAX_PHASOR_FILTERS; i += 1)
            tmp = do_biquad(tmp, &params->allpass[i], curr_channel);
        *out = *s * Dry + tmp * Wet;
        
        curr_channel = (curr_channel + 1) % channels;
        s++;
        out++;
        count--;
    }
    params->f += 1000.0 / params->sweep_time / sampleRate * (numSamples/ channels);
    if (params->f >= 1.0)
        params->f -= 1.0;
}

void phasor_create(struct phasor_params *params)
{    
    params->sweep_time = 200.0;
    params->depth = 100.0;
    params->drywet = 50.0;
    params->f = 0;
    params->stereo = 0;

    hilbert_init(&params->hilb); 
}



/////////////////////////////////////////////////////
// Phaser
/////////////////////////////////////////////////////
void sustain_filter(struct sustain_params *ds, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{

    int             count;
    DspFloatType     *s = inputs;
    DspFloatType     *o = outputs;
    DspFloatType      volAccum,
                    tmp;
    DspFloatType           CompW1;
    DspFloatType           CompW2;
    DspFloatType           gateFac;
    DspFloatType           compFac;

    

    count = numSamples;
    

    volAccum = ds->volaccum;
    CompW1 = ds->sust / 100.0f;
    CompW2 = 1.0f - CompW1;

    while (count) {
	tmp = *s;
	/*
	 * update volAccum 
	 */
	tmp = (tmp < 0) ? -tmp : tmp;
	volAccum = (256 - ds->noise) * volAccum + ds->noise * tmp;
	volAccum /= 256;

	/*
	 * handle compression 
	 */
	compFac = 30000.0f / (DspFloatType) volAccum;
	compFac = CompW1 * compFac + CompW2;
	/*
	 * handle gate 
	 */
	if (ds->threshold <= 1.0f)
	    gateFac = 1.0f;
	else
	    gateFac = (volAccum > (ds->threshold * 100)) ? 1.0f :
		((DspFloatType) (volAccum) / (DspFloatType) (ds->threshold * 100));
	/*
	 * process signal... 
	 */
	tmp = ((DspFloatType) (*s) * compFac * gateFac);
#ifdef CLIP_EVERYWHERE
        CLIP_SAMPLE(tmp)
#endif
	*o = tmp;
	s++;
    o++;
	count--;
    }
    ds->volaccum = volAccum;

}


void sustain_create(struct sustain_params *psustain)
{
    psustain->noise = 40;
    psustain->sust = 256;
    psustain->threshold = 256;
}


/////////////////////////////////////////////////////
// Tremolo
/////////////////////////////////////////////////////

void tremolo_filter(struct tremolo_params *tp, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{    
    DspFloatType vol;
    DspFloatType speed;
    int count;
    int currchannel = 0;
    DspFloatType *s;
    DspFloatType *o = outputs;
    int channels=1;

    s = inputs;
    count = numSamples;
    
    speed = tp->tremolo_speed / 1000.0 * sampleRate;;
    
    while (count) {

        *o = *s;
        if (tp->tremolo_phase >= speed)
            tp->tremolo_phase = 0;

        vol = 1.0 - tp->tremolo_amplitude / 100.0 *
            (1.0 + sin(tp->tremolo_phase / speed)) / 2.0;

        *o *= vol;

        currchannel = (currchannel + 1) % channels;
        if (currchannel == 0)
            tp->tremolo_phase++;

        s++;
        o++;
        count--;
    }
}

void tremolo_create(struct tremolo_params * p)
{
    p->tremolo_amplitude = 0.9;
    p->tremolo_speed     = 1.0;
    p->tremolo_phase     = 0.25;        
}


/////////////////////////////////////////////////////
// Tube Amp
/////////////////////////////////////////////////////

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define UPSAMPLE_RATIO 6
#define IMPULSE_SIZE   512

#ifdef __GNUC__
#define ALIGNED(n) __attribute__((aligned(n)))
#else
#define ALIGNED(n)
#endif

/* Marshall G15R */
static const DspFloatType ALIGNED(16) impulse_g15r[IMPULSE_SIZE] = {
  4405,  17364,  30475,  33517,  28810,  20846,   9309,  -4045, -13421, -13737,  -6939,
  -644,   2381,   4726,   4890,   -577,  -8708, -13224, -11835,  -6840,   -805,   5847,
 11158,  10895,   3963,  -5524, -11923, -13616, -12717,  -9577,  -4247,   -180,    568,
  -647,  -3434,  -7154,  -9508,  -9045,  -5524,   -436,   3582,   4809,   2857,   -600,
 -3441,  -5228,  -5474,  -3608,   -936,    568,    785,    961,   1268,    763,   -657,
 -1941,  -2769,  -3712,  -4174,  -3405,  -2362,  -2122,  -2521,  -2793,  -2499,  -1808,
 -1174,  -1051,  -1339,  -1579,  -1982,  -2796,  -3538,  -3834,  -3492,  -2315,   -813,
   461,   1832,   3235,   3535,   1981,   -688,  -2823,  -3401,  -2683,  -1290,    290,
  1276,    924,   -613,  -2299,  -3054,  -2582,  -1428,   -275,    411,    317,   -452,
 -1440,  -2337,  -2950,  -3028,  -2519,  -1646,   -584,    580,   1663,   2431,   2623,
  2135,   1272,    563,    325,    470,    861,   1568,   2341,   2490,   1838,    954,
   227,   -215,   -167,    182,    305,    140,    -70,   -334,   -565,   -404,    123,
   592,    852,   1030,   1158,   1222,   1264,   1225,    932,    389,   -103,   -322,
  -347,   -283,    -81,    280,    683,   1030,   1335,   1549,   1539,   1323,   1070,
   928,   1095,   1687,   2403,   2755,   2615,   2172,   1513,    736,    167,    -33,
   -77,    -72,     70,    261,    311,    206,     41,   -127,   -177,     79,    635,
  1166,   1339,   1139,    812,    641,    707,    913,   1170,   1414,   1597,   1697,
  1667,   1492,   1250,    993,    687,    328,      7,   -170,   -125,    118,    346,
   425,    483,    579,    577,    465,    405,    358,    138,   -141,   -242,   -189,
  -124,    -41,    136,    373,    533,    603,    652,    682,    710,    728,    673,
   560,    468,    378,    222,     39,    -20,    117,    391,    740,   1171,   1582,
  1737,   1543,   1113,    601,    108,   -273,   -479,   -516,   -482,   -554,   -802,
 -1038,  -1068,   -905,   -618,   -236,    137,    338,    388,    463,    582,    567,
   325,     52,    -29,     22,    141,    347,    481,    382,    206,    121,     38,
  -141,   -394,   -684,   -914,   -923,   -729,   -536,   -457,   -459,   -499,   -510,
  -412,   -254,   -135,   -115,   -243,   -509,   -723,   -695,   -588,   -668,   -874,
 -1000,   -994,   -927,   -852,   -743,   -587,   -456,   -436,   -472,   -439,   -281,
   -41,    140,    152,     17,   -204,   -454,   -649,   -740,   -707,   -573,   -403,
  -268,   -229,   -307,   -460,   -685,  -1015,  -1384,  -1657,  -1755,  -1664,  -1434,
 -1150,   -859,   -555,   -255,    -60,    -17,    -32,    -22,     -2,    -20,    -83,
  -169,   -288,   -388,   -351,   -173,     47,    273,    483,    573,    444,     86,
  -348,   -621,   -701,   -706,   -631,   -489,   -438,   -519,   -577,   -562,   -513,
  -373,   -123,    113,    269,    380,    438,    421,    343,    202,     -1,   -229,
  -401,   -450,   -374,   -220,    -65,     23,     47,     17,    -60,   -135,   -151,
   -83,     72,    264,    398,    415,    295,     73,   -134,   -237,   -268,   -307,
  -370,   -414,   -411,   -308,    -64,    233,    424,    422,    267,     72,    -23,
    63,    282,    545,    824,   1069,   1165,   1098,   1012,    931,    752,    577,
   612,    765,    831,    816,    783,    684,    537,    484,    564,    671,    759,
   843,    860,    742,    569,    459,    432,    459,    541,    680,    798,    812,
   741,    611,    453,    352,    382,    527,    728,    923,   1016,    928,    707,
   491,    393,    467,    670,    900,   1073,   1115,   1000,    778,    577,    520,
   562,    574,    536,    494,    407,    222,     15,    -98,   -111,    -97,    -68,
     6,     73,    109,    224,    442,    592,    535,    306,     37,   -127,   -114,
    23,    183,    268,    193,     30,    -61,    -74,    -46,     45,    156,    201,
   217,    275,    317,    268,    146,    -16,   -186,   -257,   -148,     74,    275,
   372,    338,    207,     53,    -63,   -131,   -187,   -265,   -372,   -482,   -551,
  -587,   -642,   -689,   -662,   -577,   -485
};

/* Princeton II */
static const DspFloatType ALIGNED(16) impulse_princeton2[IMPULSE_SIZE] = {
  2799,  11631,  23881,  32811,  34786,  30693,  22401,  12097,   3608,    333,   1986,
  5050,   5906,   3149,  -2263,  -7957, -11151,  -9808,  -4421,   1179,   2345,  -1974,
 -8064, -11426, -10826,  -7845,  -4476,  -2085,  -1307,  -1743,  -2306,  -2291,  -1539,
  -317,    118,  -1496,  -4272,  -5880,  -5257,  -2844,    410,   2743,   1949,  -2161,
 -6821,  -9053,  -8010,  -4596,   -871,    603,  -1322,  -5235,  -8392,  -8852,  -6539,
 -2851,    514,   2273,   1827,   -491,  -3243,  -4689,  -4452,  -3242,  -1617,   -302,
  -339,  -1949,  -3846,  -4448,  -3687,  -3003,  -3510,  -4938,  -6335,  -7114,  -7239,
 -6769,  -5656,  -4182,  -2937,  -2249,  -1904,  -1544,  -1072,   -366,    566,   1098,
   532,  -1043,  -2570,  -2751,  -1216,   1249,   3294,   3731,   2278,   -206,  -2371,
 -3195,  -2402,   -639,    959,   1533,    852,   -597,  -1887,  -2102,   -820,   1549,
  3846,   4837,   4143,   2398,    624,   -298,    -81,    793,   1552,   1605,    889,
   -26,   -445,   -138,    631,   1455,   1937,   1740,    745,   -735,  -2002,  -2435,
 -1966,  -1092,   -495,   -539,  -1109,  -1708,  -1762,  -1079,     21,    935,   1103,
   489,   -400,   -960,   -718,    427,   2035,   3310,   3636,   3115,   2310,   1690,
  1475,   1623,   1770,   1452,    548,   -577,  -1348,  -1424,   -922,   -242,    234,
   357,    244,     66,      9,    208,    450,    376,    -69,   -688,  -1261,  -1605,
 -1569,  -1160,   -563,    -59,    110,    -19,   -277,   -570,   -774,   -798,   -733,
  -693,   -715,   -705,   -525,   -217,     69,    263,    338,    298,    199,     98,
    28,     83,    344,    774,   1293,   1772,   2010,   1960,   1771,   1555,   1321,
  1015,    609,    246,     59,    -23,    -17,    164,    447,    666,    749,    681,
   472,    145,   -206,   -444,   -476,   -389,   -386,   -535,   -683,   -692,   -618,
  -550,   -459,   -335,   -222,   -121,    -20,     42,     19,   -109,   -284,   -432,
  -588,   -803,   -925,   -743,   -296,    158,    419,    486,    393,    140,   -150,
  -305,   -249,    -36,    209,    377,    353,    143,    -27,     72,    414,    789,
  1050,   1150,   1066,    861,    697,    716,    850,    914,    803,    592,    463,
   502,    652,    831,    933,    826,    398,   -277,   -895,  -1136,   -937,   -460,
    54,    374,    374,    147,    -70,   -119,    -51,     13,     48,     69,     99,
   171,    305,    515,    759,    919,    904,    753,    606,    589,    731,    945,
  1125,   1182,   1069,    824,    600,    546,    682,    890,   1042,   1070,    917,
   588,    228,     58,    181,    439,    561,    488,    369,    277,    186,    150,
   218,    306,    309,    228,    161,    210,    355,    474,    526,    538,    479,
   352,    292,    356,    429,    432,    369,    220,     33,     -9,    149,    329,
   374,    347,    353,    356,    312,    369,    588,    735,    650,    497,    459,
   465,    419,    401,    447,    408,    218,     75,    122,    298,    550,    802,
   914,    793,    468,    134,     48,    265,    538,    603,    401,    -11,   -499,
  -805,   -752,   -451,   -167,   -106,   -314,   -666,   -922,   -896,   -621,   -313,
  -169,   -249,   -522,   -850,   -981,   -774,   -374,    -45,     49,    -87,   -320,
  -443,   -281,    207,    831,   1269,   1309,    999,    586,    307,    276,    451,
   608,    527,    265,     86,    145,    403,    710,    869,    762,    456,    139,
   -46,    -62,     53,    189,    214,     73,   -149,   -301,   -285,   -111,    104,
   202,    101,    -96,   -195,   -102,    106,    263,    252,     62,   -223,   -442,
  -446,   -214,    166,    549,    811,    893,    791,    564,    360,    344,    506,
   676,    744,    719,    635,    511,    382,    289,    245,    207,    124,     35,
     6,     -1,    -50,    -83,    -24,    115,    247,    331,    411,    479,    436,
   235,    -15,   -148,   -130,    -84,    -98,   -135,   -147,   -160,   -220,   -304,
  -396,   -515,   -664,   -772,   -733,   -528,   -267,    -96,   -105,   -307,   -610,
  -870,   -939,   -708,   -263,    109,    199
};

typedef struct {
    const char       *name;
    const DspFloatType *impulse;
} ampmodels_t;

static const ampmodels_t ampmodels[] = {
    { "Marshall G15R", impulse_g15r,      },
    { "Princeton II",  impulse_princeton2 },
    { NULL,            NULL               }
};

typedef struct {
    const char       *name;
    const int        quality;
} ampqualities_t;

static const ampqualities_t ampqualities[] = {
    { "Lowest",  64  },
    { "Normal",  128 },
    { "High",    256 },
    { "Extreme", 512 },
    { NULL,      0   }
};

#define NONLINEARITY_SIZE 16384      /* the bigger the table, the louder before hardclip */
#define NONLINEARITY_PRECISION (1/16.0)   /* the bigger the value, the lower the noise */

#define NONLINEARITY_SCALE 1024     /* this variable works like gain */
static DspFloatType nonlinearity[NONLINEARITY_SIZE];
static void tubeamp_init(struct effect *p) {
  (void) p;
}

/* waveshaper based on generic lookup table */
static DspFloatType
F_tube(DspFloatType in, DspFloatType r_i)
{
    DspFloatType pos;
    int32_t idx;
    
    pos = (in / r_i) * (DspFloatType) (NONLINEARITY_SCALE * NONLINEARITY_PRECISION) + (DspFloatType) (NONLINEARITY_SIZE / 2);
    
    /* This safety catch should be made unnecessary.
     * But it may require us to extend the nonlinearity table to ridiculously far.
     * Besides, hard blocking distortion is fairly ok as effect when you go too loud. */
    if (pos < 0.f) {
        //printf("pos < 0!");
        pos = 0.f;
    }
    if (pos > (DspFloatType) (NONLINEARITY_SIZE - 2)) {
        //printf("pos > size!");
        pos = (DspFloatType) (NONLINEARITY_SIZE - 2);
    }

    idx = pos;
    pos -= idx;
    return (nonlinearity[idx] * (1.0f-pos) + nonlinearity[idx+1] * pos) * r_i;
}

 
static DspFloatType convolve(const DspFloatType *a, const DspFloatType *b, int len)
{
    int i;
    /* a long int type would be needed to hold the value in integer dsp */
    DspFloatType dot = 0;
    for (i = 0; i < len; i += 1)
            dot += (DspFloatType) a[i] * (DspFloatType) b[i];
    return dot;
}



void tubeamp_filter(struct tubeamp_params *params, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{
    unsigned int i, j, k, curr_channel = 0;
    DspFloatType *ptr1;    
    DspFloatType gain;
    DspFloatType in;
    DspFloatType out;
    
    DspFloatType *o = outputs;
    int channels=1;

    /* update bq states from tone controls */
    set_lsh_biquad(sampleRate * UPSAMPLE_RATIO, 500, params->tone_bass, &params->bq_bass);
    set_peq_biquad(sampleRate * UPSAMPLE_RATIO, 650, 500.0, params->tone_middle, &params->bq_middle);
    set_hsh_biquad(sampleRate * UPSAMPLE_RATIO, 800, params->tone_treble, &params->bq_treble);

    gain = pow(10.f, params->gain / 20.f);
    
    /* highpass -> low shelf eq -> lowpass -> waveshaper */
    for (i = 0; i < numSamples; i += 1) {
        DspFloatType result;
        for (k = 0; k < UPSAMPLE_RATIO; k += 1) {
            /* IIR interpolation */
            in = inputs[i] * MAX_SAMPLE;
            params->in[curr_channel] = (in + params->in[curr_channel] * (DspFloatType) (UPSAMPLE_RATIO-1)) / (DspFloatType) UPSAMPLE_RATIO;
            result = params->in[curr_channel] / (DspFloatType) MAX_SAMPLE;
            for (j = 0; j < params->stages; j += 1) {
                /* gain of the block */
                result *= gain;
                /* low-pass filter that mimicks input capacitance */
                result = do_biquad(result, &params->lowpass[j], curr_channel);
                /* add feedback bias current for "punch" simulation for waveshaper */
                result = F_tube(params->bias[j] - result, params->r_i[j]);
                /* feedback bias */
                params->bias[j] = do_biquad((params->asymmetry - params->biasfactor * result) * params->r_k_p[j], &params->biaslowpass[j], curr_channel);
                /* high pass filter to remove bias from the current stage */
                result = do_biquad(result, &params->highpass[j], curr_channel);
                
                /* run tone controls after second stage */
                if (j == 1) {
                    result = do_biquad(result, &params->bq_bass, curr_channel);
                    result = do_biquad(result, &params->bq_middle, curr_channel);
                    result = do_biquad(result, &params->bq_treble, curr_channel);
                }
            }
            result = do_biquad(result, &params->decimation_filter, curr_channel);
        }
        ptr1 = params->buf[curr_channel] + params->bufidx[curr_channel];
        
        /* convolve the output. We put two buffers side-by-side to avoid & in loop. */
        ptr1[IMPULSE_SIZE] = ptr1[0] = result / 500.f * (DspFloatType) (MAX_SAMPLE >> 13);
        out = convolve(ampmodels[params->impulse_model].impulse, ptr1, ampqualities[params->impulse_quality].quality) / 32.f;
        out /= MAX_SAMPLE;
        //db->data_swap[i] = out;
        *o++ = out;
        params->bufidx[curr_channel] -= 1;
        if (params->bufidx[curr_channel] < 0)
            params->bufidx[curr_channel] += IMPULSE_SIZE;
        
        curr_channel = (curr_channel + 1) % channels;
    }
}


void tubeamp_destroy(struct tubeamp_params *params)
{
    int i;
    for (i = 0; i < MAX_CHANNELS; i += 1)
        free(params->buf[i]); 
}
void tubeamp_create(struct tubeamp_params *params)
{
    int i;
    DspFloatType tmp;
       
    params->stages = 4;
    params->gain = 35.0; /* dB */
    params->biasfactor = -7;
    params->asymmetry = -3500;
    params->impulse_model = 0;
    params->impulse_quality = 1;

    params->tone_bass = +3; /* dB */
    params->tone_middle = -10; /* dB */
    params->tone_treble = 0;

    /* configure the various stages */
    params->r_i[0] = 68e3 / 3000;
    params->r_k_p[0] = 2700 / 100000.0;
    set_chebyshev1_biquad(sampleRate * UPSAMPLE_RATIO, 22570, 0.0, TRUE, &params->lowpass[0]);
    set_rc_lowpass_biquad(sampleRate * UPSAMPLE_RATIO, 86, &params->biaslowpass[0]);
    set_rc_highpass_biquad(sampleRate * UPSAMPLE_RATIO, 37, &params->highpass[0]);
    
    params->r_i[1] = 250e3 / 3000;
    params->r_k_p[1] = 1500 / 100000.0;
    set_chebyshev1_biquad(sampleRate * UPSAMPLE_RATIO, 6531, 0.0, TRUE, &params->lowpass[1]);
    set_rc_lowpass_biquad(sampleRate * UPSAMPLE_RATIO, 132, &params->biaslowpass[1]);
    set_rc_highpass_biquad(sampleRate * UPSAMPLE_RATIO, 37, &params->highpass[1]);
    
    params->r_i[2] = 250e3 / 3000;
    params->r_k_p[2] = 820 / 1000000.0;
    set_chebyshev1_biquad(sampleRate * UPSAMPLE_RATIO, 6531, 0.0, TRUE, &params->lowpass[2]);
    set_rc_lowpass_biquad(sampleRate * UPSAMPLE_RATIO, 194, &params->biaslowpass[2]);
    set_rc_highpass_biquad(sampleRate * UPSAMPLE_RATIO, 37, &params->highpass[2]);
    
    params->r_i[3] = 250e3 / 3000;
    params->r_k_p[3] = 820 / 100000.0;
    set_chebyshev1_biquad(sampleRate * UPSAMPLE_RATIO, 6531, 0.0, TRUE, &params->lowpass[3]);
    set_rc_lowpass_biquad(sampleRate * UPSAMPLE_RATIO, 250, &params->biaslowpass[3]);
    set_rc_highpass_biquad(sampleRate * UPSAMPLE_RATIO, 37, &params->highpass[3]);

    set_chebyshev1_biquad(sampleRate * UPSAMPLE_RATIO, 12000, 10.0, TRUE, &params->decimation_filter);

#define STEEPNESS   3e-3
#define SCALE       1e2
#define STEEPNESS2  1e-2
#define SCALE2      5e-1
#define NONLINEARITY_MAX 1024           /* normalize table between -1 .. 1 */

    DspFloatType y = 0.0;
    for (i = 0; i < NONLINEARITY_SIZE; i += 1) {
        int iter = 10000;
        /* Solve implicit equation
         * x - y = e^(-y / 10) / 10
         * for x values from -250 to 250. */
        DspFloatType x = (i - NONLINEARITY_SIZE / 2) / (DspFloatType) NONLINEARITY_PRECISION;
        while (--iter) {
            DspFloatType value = x - y - SCALE * exp(STEEPNESS * y) + SCALE2 * exp(STEEPNESS2 * -y);
            DspFloatType dvalue_y = -1 - (SCALE * STEEPNESS) * exp(STEEPNESS * y) - (SCALE2 * STEEPNESS2) * exp(STEEPNESS2 * -y);
            DspFloatType dy = value / dvalue_y;
            y = (y + (y - dy)) / 2; /* average damp */

            if (fabs(value) < 1e-4)
                break;
        }
        if (iter == 0) {
            printf( "Failed to solve the nonlinearity equation for %f!\n", x);
        }
        nonlinearity[i] = y / NONLINEARITY_MAX;
        // printf("%d %f\n", i, nonlinearity[i]);
    }
    /* balance median to 0 */
    tmp = nonlinearity[NONLINEARITY_SIZE / 2];
    for (i = 0; i < NONLINEARITY_SIZE; i += 1)
        nonlinearity[i] -= tmp;

    for (i = 0; i < MAX_CHANNELS; i += 1)
        params->buf[i] = (DspFloatType*)calloc(IMPULSE_SIZE * 2, sizeof(DspFloatType));


}


/////////////////////////////////////////////////////
// Vibrato
/////////////////////////////////////////////////////



void vibrato_filter(struct vibrato_params *vp, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{
    DspFloatType     *s = inputs;
    DspFloatType *o = outputs;
    int             count;
    int		    curr_channel = 0;
    int channels =1;

    
    count = numSamples;

    /* the vibrato effect is based on hilbert transform that allows us to reconstruct
     * shifted frequency spectra. However, the shifting effect does not preserve
     * frequency relationships because it implements f' = f - f_mod. On the other
     * hand, neither does the genuine guitar tremolo bar, so there is some emulation
     * accuracy.
     *
     * The f_mod is the speed of modulation, stored in variables speed. The speed
     * itself is modulated by the period and depth parameters. */
    
    while (count) {
        DspFloatType x0, x1;
        DspFloatType sinval, cosval;
        hilbert_transform(*s, &x0, &x1, &vp->hilbert, curr_channel);
        
        sinval = sin_lookup(vp->phase);
        cosval = cos_lookup(vp->phase);
        if (vp->vibrato_base > 0)
            *o = cosval * x0 + sinval * x1;
        else
            *o = cosval * x0 - sinval * x1;
        
        curr_channel = (curr_channel + 1) % channels;
	    if (curr_channel == 0) {
	    vp->vibrato_phase += 1000.0 / vp->vibrato_speed / sampleRate;
            if (vp->vibrato_phase >= 1.0)
                vp->vibrato_phase -= 1.0;
	    vp->phase += (fabs(vp->vibrato_base) + (vp->vibrato_amplitude + sin_lookup(vp->vibrato_phase) * vp->vibrato_amplitude) / 2) / sampleRate;
            if (vp->phase >= 1.0)
                vp->phase -= 1.0;
        }

	    s++;
        o++;
	    count--;
    }
}


void vibrato_create(struct vibrato_params *pvibrato)
{
    hilbert_init(&pvibrato->hilbert);
    pvibrato->vibrato_base = 0;
    pvibrato->vibrato_amplitude = 10;
    pvibrato->vibrato_speed = 200;
    pvibrato->vibrato_phase = 0;
}


/////////////////////////////////////////////////////
// Pitch
/////////////////////////////////////////////////////


/* this function scans only about 1/4th of the full range of possible
 * offsets, allowing us to work realtime on somewhat worse hardware.
 * GNUitar is however becoming very SSE dependant. */
static int
estimate_best_correlation(DspFloatType *data, const int frames, const int alignoff, DspFloatType *ref, const int looplen)
{
    int16_t i = alignoff, best = 0, apprx;
    DspFloatType goodness = 0;
    /* i is chosen so that data + i is aligned by 16, which allows the
     * use of optimum assembly instructions in the faster scan. */

    /* estimate the position of a local maximum */
    while (i < frames - looplen) {
        /* compute correlation term. The aim is to maximise this value. */
        DspFloatType goodness_try = convolve(ref, data + i, looplen);
        /* HACK: skip forward faster if currently anticorrelated. This is purely
         * for performance: we want as long convolution buffers as possible without
         * paying so much for them. Since the max guitar frequency is roughly
         * 1.2 kHz we can assume that the output oscillates at roughly 40 sample long
         * patterns. If we are negative, it is almost certain that about 10 samples
         * one quarter of that can be skipped forward without bypassing any peak. */
        if (goodness_try >= goodness) {
            goodness = goodness_try;
            best = i;
        }
        /* increment by 4 keeps data+i aligned by 16 */
        i += 4;
    }
    apprx = best;

    /* now look around the estimated maximum for the best match */
    for (i = std::max(best - 3, 0); i < std::min(best + 4, frames - looplen); i += 1) {
        /* don't recompute the convolution we already know */
        if (i == apprx)
            continue;
        DspFloatType goodness_try = convolve(ref, data + i, looplen);
        if (goodness_try >= goodness) {
            goodness = goodness_try;
            best = i;
        }
    }

    return best;
}

#ifdef __SSE__
static void
copy_to_output_buffer(DspFloatType *in, DspFloatType *out, DspFloatType *wp, const int length)
{
    const __m128 ones = { 1.f, 1.f, 1.f, 1.f };
    __m128 * __restrict__ out4 = (__m128 * __restrict__) out;
    const __m128 * __restrict__ wp4 = (const __m128 * __restrict__) wp;
    int16_t i;

    /* sum the first half with the tail of previous buffer, but overwrite
     * with the second half because the data on that side is old. */
    for (i = 0; i < length / 2 / 4; i += 1) {
        __m128 w = wp4[i];
        /* unfortunately we must take the performance hit of unaligned load */
        out4[i] = w * _mm_loadu_ps(in + i * 4) + (ones - w) * out4[i + length / 2 / 4];
    }
    memcpy(out + length / 2, in + length / 2, sizeof(DspFloatType) * length / 2);
    /* output buffer can now be read from 0 to length / 2 */
}
#else
static void
copy_to_output_buffer(DspFloatType *in, DspFloatType *out, DspFloatType *wp, const int length)
{
    int16_t i;

    /* sum the first half with the tail of previous buffer, but overwrite
     * with the second half because the data on that side is old. */
    for (i = 0; i < length / 2; i += 1) {
        DspFloatType w = wp[i];
        out[i] = w * in[i] + (1.f - w) * out[i + length/2];
    }
    memcpy(out + length / 2, in + length / 2, sizeof(DspFloatType) * length / 2);
    /* output buffer can now be read from 0 to length / 2 */
}
#endif

#define LOOP_LENGTH 1000
#define MEMORY_LENGTH (1800 + LOOP_LENGTH * 3 / 2) /* 1800 ~ 26.7 Hz at 48 kHz */
#define MAX_RESAMPLING_FACTOR 2.0
#define MAX_OUTPUT_BUFFER (MAX_RESAMPLING_FACTOR * (MEMORY_LENGTH + LOOP_LENGTH))


static void
resample_to_output(Backbuf_t *history, const int deststart, const int destend, DspFloatType *input, const int sourcelength)
{
    int16_t i;
    const int16_t destlength = destend - deststart;
    DspFloatType factor = (DspFloatType) sourcelength / destlength, pos = 0;

    /* very primitive resampler but it should be good enough for now */
    for (i = 0; i < destlength; i += 1) {
        int idx;
        pos += factor;
        idx = pos;
        DspFloatType mid = pos - idx;
        history->add(history, (1.f - mid) * input[idx] + mid * input[idx + 1]);
    }
}

static DspFloatType *window_memory = NULL;

void pitch_filter(struct pitch_params * params, size_t n, DspFloatType * inputs, DspFloatType * outputs)
{    
    DspFloatType *s = inputs;
    DspFloatType *o = outputs;
    int count = n;
    int channels=1;
    int i;
    DspFloatType depth, Wet, Dry, output_inc;

    depth = powf(2.f, (params->halfnote + params->finetune) / 12.f);
    if (depth < 0.25f)
       depth = 0.25f;
    if (depth > (DspFloatType) MAX_RESAMPLING_FACTOR)
       depth = (DspFloatType) MAX_RESAMPLING_FACTOR;

    Wet = params->drywet / 100.0f;
    Dry = 1.f - Wet;

    output_inc = LOOP_LENGTH / 2 / depth; 
    while (count --) {
        /* uninterleave channel data */
        for (i = 0; i < channels; i += 1) {
            params->channel_memory[i][params->memory_index] =
            params->channel_memory[i][params->memory_index + MEMORY_LENGTH] =
                *s++;
        }
        params->memory_index += 1;
        if (params->memory_index == MEMORY_LENGTH)
            params->memory_index = 0;
       
        /* test whether we have advanced enough to do chunk of output */ 
        params->output_buffer_trigger -= 1;
        if (params->output_buffer_trigger >= 0.f)
            continue;

        /* the factor 2.0 comes from doing 2x overlap with Hann window. */
        params->output_buffer_trigger += LOOP_LENGTH / depth / 2.0f;

        /* at this point, all data from memory-index forward in each buffer
         * is the oldest data recalled. There is exactly MEMORY_LENGTH
         * frames of valid data forward from that point as continuous chunk. */

        /* the aim now is to produce LOOP_LENGTH of data in the output buffer
         * and sum the first half using some window with the old data,
         * and overwrite the other half. After this, data from
         * position 0 to LOOP_LEN/2 can be resampled to output. */

        /* We start by searching for data in the input that looks like the
         * last chunk we put into output. Output is split into two parts:
         * the data mixed with the "tail" of the previous buffer and
         * pristine input data. At each iteration, we use only half of the
         * output buffer for actual output, and copy the latter half as the
         * first half for next iteration. */
        for (i = 0; i < channels; i += 1) {
            int bestpos = estimate_best_correlation(
                params->channel_memory[i] + params->memory_index,
                MEMORY_LENGTH - LOOP_LENGTH / 2, /* look at next stmt */
                (4 - (params->memory_index % 4)) % 4, /* alignment 0..3 */
                params->output_memory[i],
                LOOP_LENGTH);
            
            /* copy the data after the best match into the output buffer. The
             * 0..length/2 part is windowed with the previous part, and the
             * length/2 .. length part is new. So by advanging forwards by
             * LOOP_LENGTH / 2 we start mixing over the part that the previous
             * memcpy() left us (see end of this loop). */
            bestpos += LOOP_LENGTH / 2;

            copy_to_output_buffer(params->channel_memory[i] + params->memory_index + bestpos, params->output_memory[i], window_memory, LOOP_LENGTH);

            /* write to output from memory -- this algorithm is rubbish.
             * I should probably produce full length resampled stream and
             * then do the resampling in one step. */
            resample_to_output(params->history[i], params->output_pos, params->output_pos + output_inc, params->output_memory[i], LOOP_LENGTH / 2);
        }
        
        params->output_pos += output_inc;
    }
    /* now output_memory holds the resampled output. The trouble is, there
     * might not be enough bytes to fill the final output buffer. At least
     * one LOOP_LENGTH / RESAMPLE_RATIO bytes should be kept extra... */

    s = inputs; 
    count = n;
    while (count --) {
        /* don't consume until we have enough for full fill. This should
         * not trigger after the first few startup frames. It might be
         * best to do this initially as a small hack during initializing. */
        if (params->output_pos < count) {
            for (i = 0; i < n; i += 1)
                *o++ = 0;
            continue;
        }
        /* consume history now. */
        for (i = 0; i < n; i += 1) {
            *o = Dry * *s + Wet * params->history[i]->get(params->history[i], (int) params->output_pos);
            s++;
        }
        params->output_pos -= 1.f;
    }
}



void pitch_create(    struct pitch_params *params)
{
    int                 i;
       
    for (i = 0; i < MAX_CHANNELS; i += 1) {
        params->channel_memory[i] = (DspFloatType*)calloc(MEMORY_LENGTH * 2, sizeof(DspFloatType));
        params->output_memory[i] = (DspFloatType*)calloc(LOOP_LENGTH, sizeof(DspFloatType));
        params->history[i] = new_Backbuf(MAX_OUTPUT_BUFFER);
    }
    if (window_memory == NULL) {
        /* I will never free this memory -- some effects should have
         * a global init and destroy funcs and I don't have them. */
        window_memory =(DspFloatType*)calloc(LOOP_LENGTH, sizeof(DspFloatType));
        for (i = 0; i < LOOP_LENGTH; i += 1)
            window_memory[i] = 0.5f-0.5f * cos_lookup((DspFloatType) i / LOOP_LENGTH);
    }
    
    params->drywet = 100;
    
}

void pitch_destroy(struct pitch_params *params)
{
    
    int i;
    
    for (i = 0; i < MAX_CHANNELS; i += 1) {        
        del_Backbuf(params->history[i]);
    }
    free(window_memory);
    
}



/* this is a single-channel to two-channel effect */
void rotary_filter(struct rotary_params *params, size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{
    
    int i;
    DspFloatType pha, sinval = 0, cosval = 0;
    DspFloatType *tmp;
    DspFloatType *o = outputs;
    int channels = 1;

    
    /* The rotary speaker simulation is based on modulating input with subsonic
     * sinuswave and then separating the upwards and downwards shifted frequencies
     * with hilbert transform. The upwards shifted component can be thought to be
     * the horn, and the downwards shifted component the bass speaker. (After
     * a half-turn, the "bass" will shift up and the "horn" will shift down.)
     *
     * After the separation step, a rough approximation of HRTF is used to mix the
     * horn with the bass speaker for both channels. */
    

    pha = params->phase;
    for (i = 0; i < numSamples; i += 1) {
        DspFloatType x0, x1, y0, y1;

        /* update the approximation of sin and cos values to avoid
         * discontinuities between audio blocks */
        if (i % 8 == 0) {
            sinval = sin_lookup(pha);
            cosval = cos_lookup(pha);
            pha += (DspFloatType) 16 / sampleRate * 1000.0 / params->speed;
            // params->speed;
            if (pha >= 1.0)
                pha -= 1.0;
        }

        /* compute separate f + fc and f - fc outputs */
        hilbert_transform(inputs[i], &x0, &x1, &params->hilb, 0);
        y0 = cosval * x0 + sinval * x1;
        y1 = cosval * x0 - sinval * x1;

        /* factor and biquad estimate hrtf */
        *o++ = 0.60 * y0 + 0.40 * do_biquad(y1, &params->ld, 0);
        *o   = 0.5*(*o +  0.60 * y1 + 0.40 * do_biquad(y0, &params->rd, 0));
    }

    params->phase += (DspFloatType) numSamples / sampleRate * 1000.0 / params->speed;
    if (params->phase >= 1.0)
        params->phase -= 1.0;

}

void rotary_create(struct rotary_params *params)
{
    params->speed = 1000;
    params->unread_output = 0;

    set_rc_lowpass_biquad(sampleRate, 2000, &params->ld);
    set_rc_lowpass_biquad(sampleRate, 2000, &params->rd);

    hilbert_init(&params->hilb);
}

