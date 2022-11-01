import("stdfaust.lib");


speakers = (-45, 45, 135, -135);

n = 4;

angle 	    = hslider("angle",    0.0, -180, 180, 0.01);
distance  = hslider("distance", 0.5, 0, 10, 0.01);


process = _ <: par(i, n, *( scaler(i, n, angle, distance) : si.smooth(0.9999) ))
with {

	scaler(i, n, angle, distance) = (distance/2.0+0.5)
		* sqrt( max(0.0, 1.0 - abs(fmod(angle+0.5 + float(n-i) /n, 1.0) - 0.5) * n * distance));
};