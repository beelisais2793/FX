#include "Utils.h"
#include "ZVerb.h"
#include <math.h>
#include <stdexcept>
#include <vector>

ZVerb::ZVerb() {}

ZVerb::ZVerb(float wet, float roomsize, float samplingRate) {
    int ret;
    if ((ret = setup(wet, roomsize, samplingRate)))
        throw std::runtime_error("feedback comb: decay is less then delay");
}

int ZVerb::setup(float wet, float roomsize, float samplingRate) {
    if (wet < 0) {
        return -1;
    }
    wet_ = wet;
    roomsize_ = roomsize;
    samplingRate_ = samplingRate;
    lpfilter = new I1P(1500.0 / samplingRate_);
    update();
    return 0;
}

void ZVerb::update() {
    for (unsigned int i = 0; i < ZVERB_COMB; i++) {
        comb[i] = FeedbackComb(randfloatp(0.01, 0.099), randfloatp(0.1, 4.0),
                               randfloat(0.1, 0.75), samplingRate_);
    }
    for (unsigned int i = 0; i < ZVERB_ALLPASS; i++) {
        allpass[i] = Allpass(randfloatp(0.05, 0.199), randfloat(2, 4),
                             randfloat(0.1, 0.75), samplingRate_);
    }
}

float ZVerb::process(float input) {
    float out = 0;
    for (unsigned int i = 0; i < ZVERB_COMB; i++) {
        out += comb[i].process(input);
    }
    for (unsigned int i = 0; i < ZVERB_ALLPASS; i++) {
        out = allpass[i].process(out);
    }
    out = lpfilter->process(out);
    return (1 - wet_) * input + (wet_ * out);
}
