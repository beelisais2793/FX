
declare name "Echolot";
declare author "Bjornmossa";
declare version "1.0";
declare license "GNU GPL 3"; 

import("filters.lib");
import("delays.lib");
import("math.lib");

feedback = vslider("[4]feedback[style:knob]", 0.01, 0.01, 0.999, 0.01);

filter = resonlp(fc,Q,1)
with {
    fc = hslider("[0]freq[style:knob]", 1000, 60, 8000, 0.01);
    Q = hslider("[1]Q[style:knob]", 0.25, 0.01, 1, 0.01);
};

delayed = delay(SR, d)
with {
    d = vslider("[3]delay[style:knob]", 0.01, 0, 0.2, 0.001) * SR;
};

process = +~(hgroup("Echolot", (delayed : filter) * feedback));