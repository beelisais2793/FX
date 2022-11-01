// switch_example.dsp
//
//
// Henrik von Coler
// 2020-05-28

import("all.lib");

// outputs 0 if x is greater 1
// and 1 if x is below 0
// 'l' is used as an implicit argument
sel(l,x) = select2((x>=0), 0, 1);

process = -0.1 : sel(2);