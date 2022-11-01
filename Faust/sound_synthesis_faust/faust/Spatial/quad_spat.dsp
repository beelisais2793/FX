declare name 	"quad_spat";
declare author 	"HvC"; 

import("stdfaust.lib");

angle 			= hslider("angle",    0.0, 0, 1, 0.01);
distance 		= hslider("distance", 0.5, 0, 1, 0.01);

process 		= vgroup("quad_spat", sp.spat(4, angle, distance));