declare author "Bart Brouns";
declare license "GPLv3";
declare name "osc-example";
import("stdfaust.lib");

process(extFMSignal) = sineSquarePulse(extFMSignal)~_<:(_,_);

sineSquarePulse(extFMSignal,FB) =
  it.interpolate_linear(sinSquare,sine,square):clip
with {
  sine = os.osc(f);
  square = os.pulsetrain(f,width);
  clip = _*drive:max(-1):min(1);
  f = freq+(FM*FB)+(extFMSignal * extFMDep);
};

freq      = hslider ("[0]freq", 440, 27.5, 3520, 0.1) :si.smoo;
extFMDep  = hslider ("[1]fmdepth", 0, 0.0, 1.0, 0.1 ) :si.smoo;
sinSquare = hslider ("[2]sinSquare", 0.5, 0, 1, 0.1)  :si.smoo;
width     = hslider ("[3]width", 0.5, 0, 1, 0.1)      :si.smoo;
drive     = hslider ("[4]drive", 1, 1, 100, 0.1)      :si.smoo;
FM        = hslider ("[5]FM", 0, 0, 100, 0.1)         :si.smoo;
