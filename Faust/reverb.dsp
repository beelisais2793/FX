import("stdfaust.lib");

metaverb(maxdel,
        sz, dffs, fb_gain, lfo_freq, lfo_amount, hi_level, hi_freq, lo_level, lo_freq) =
        (route_in : +,+ : core) ~ fb with {
    route_in = route(4, 4, (1, 3), (2, 2), (3, 1), (4, 4));

    sz_scale(0) = 0.506392;
    sz_scale(1) = 0.803821;
    sz_scale(2) = 1;
    sz_scale(3) = 1.49834;

    filter_hs = fi.highshelf(1, hi_level, hi_freq);
    filter_ls = fi.lowshelf(1, lo_level, lo_freq);
    filter = filter_hs : filter_ls;

    lfo_phase(i) = ma.PI/4 * i;
    lfo(i) = lfo_amount * os.oscp(lfo_freq, lfo_phase(i));

    dtime(ch, i) = sz_scale(i) * sz; // + lfo(ch*i);

    allpass(ch, i) = fi.allpass_fcomb(maxdel, dtime(ch, i), dffs);

    diffusor(ch) = filter : seq(i, 3, allpass(ch, i));
    delay(ch) = de.fdelay(maxdel, dtime(ch, 3));

    fb = par(i, 2, delay(i) * fb_gain);
    core = par(i, 2, diffusor(i));
};

hi_level = 0;
hi_freq = 4000;

lo_level = 0;
lo_freq = 100;

//sz = hslider("Size", 476.8, 476.8, 2400, 0.01);

sz = 2000;

maxdel = 5000;
dffs = 0.54;

lfo_freq = 0.3;
lfo_amount = 0;

fb = 0.74;

reverb = metaverb(
     maxdel, sz, dffs, fb,
     lfo_freq, lfo_amount, hi_level, hi_freq, lo_level, lo_freq);

freq = vslider("Freq", 200, 200, 1000, 1);

process = reverb;