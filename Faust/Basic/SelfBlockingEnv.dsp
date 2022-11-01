import("stdfaust.lib");
//SELF BLOCKING ENVELOPE USING RECURSION
//once triggered, envelope won't retrigger until done

but = button("trig");

env = (1,_>0:- : _*but:en.ar(0,3)) ~ _;

process = env;  