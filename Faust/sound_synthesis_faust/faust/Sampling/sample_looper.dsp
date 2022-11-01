// sample_looper.dsp
//
// Read a set of samples from wav files
//
// - loop sample with slider for speed
// - select active sample
//
// Henrik von Coler
// 2020-05-28

import("stdfaust.lib");

// read a set of wav files
s = soundfile("label[url:{'../WAV/kick.wav'; '../WAV/cowbell.wav'; '../WAV/my_model.wav'}]", 1);

// a slider for selecting a sound file:
file_idx = hslider("file_idx",0,0,2,1);

// a slider for controlling the playback speed:
speed = hslider("speed",1,-100,100,0.01);
 
// a logic for reverse loops (wrap to positive indices)
wrap(l,x) = select2((x>=0),l-abs(x),x);


// the loop function
loop(s, idx) = (idx, reader(s)) : outs(s)
    with {

		// get recent file's properties
        length(s) = idx,0 : s : _,si.block(outputs(s)-1);
        srate(s)  = idx,0 : s : !,_,si.block(outputs(s)-2);
	
	// the playback position (a recursive counter)
	reader(s) = (speed * float(srate(s)))/ma.SR : (+,length(s):fmod)~  _ : wrap(length(s)) : int;

        // read from sample
        outs(s)   = s : si.block(2), si.bus(outputs(s)-2);

    };

process = loop(s,file_idx) <: _,_ ;