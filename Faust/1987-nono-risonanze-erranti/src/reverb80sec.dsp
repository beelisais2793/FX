import("stdfaust.lib");
import("../faust-libraries/seam.lib");

process =  _ <: rev_ottanta(16,5,3) :> _,_;
