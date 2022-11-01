// splitting_example.dsp
//
// Henrik von Coler
// 2020-04-21
 
import("stdfaust.lib");

// a source signal
signal = os.imptrain(5);
 
// split signal to stereo in process function:
process = signal <: _,_,_,_,_,_,_,_;