import("stdfaust.lib");
import("../faust-libraries/seam.lib");

process = os.osc(1000) : harmonizer;
