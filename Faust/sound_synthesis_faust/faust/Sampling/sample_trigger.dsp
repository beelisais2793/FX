// sample_trigger.dsp
//
// Read files and make them playable with a trigger.
//
// - makes use of the 
//
// Henrik von Coler
// 2020-05-28

import("stdfaust.lib");

// read a set of wav files
s = soundfile("label[url:{'../WAV/kick.wav'; '../WAV/cowbell.wav'; '../WAV/my_model.wav'}]", 1);

// a slider for controlling the level of all samples:
level = hslider("level",1,0,2,0.01);
 
// sample objects
kick = so.sound(s, 0);
bell = so.sound(s, 1);

process = kick.play( level, button("kick") ),  bell.play( level, button("bell")) :>  _   <: _,_ ;
