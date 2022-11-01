// fourier_series.dsp
//
// Generate a square wave through Fourier series.
// - without control
//
// Henrik von Coler
// 2020-05-06

import("stdfaust.lib");

// define a fundamental frequency
f0 		  = 100;

// define the number of partials
n_partial = 50;

// partial function with one argument ()
partial(partIDX) = (4/ma.PI) * os.oscrs(f)*volume
	// arguments
	with {
		   f = f0 * (2*partIDX+1);			
		   volume = 1/(2*partIDX+1);
	 	  };

// the processing function, 
// running 50 partials parallel
// mono output
process = par(i, n_partial, partial(i)) :> +;