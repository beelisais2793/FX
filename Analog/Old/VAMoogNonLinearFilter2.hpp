#pragma once

namespace Analog::Filters::Moog::NonLinear2
{
    class MoogFilter {
        float frequency;
        float g;
        float resonance;
        float drive;
        int sampleRate;
        
        float y_a;
        float y_b;
        float y_c;
        float y_d;
        float y_d_1;
        
    public:
        MoogFilter();
        ~MoogFilter();
        void processSamples(float *samples, int numSamples);
        
        float getFrequency();
        float getResonance();
        float getDrive();
        
        void setFrequency(float f);
        void setResonance(float r);
        void setSampleRate(int s);
        void setDrive(float d);
    };

    MoogFilter::MoogFilter() {
        y_a = 0;
        y_b = 0;
        y_c = 0;
        y_d = 0;
        y_d_1 = 0;
        
        frequency = 2000;
        resonance = 1;
        drive = 1;
    }
    MoogFilter::~MoogFilter() {
        
    }
    void MoogFilter::processSamples(float *samples, int numSamples){
        for (int i = 0; i < 2 * numSamples; i++) {
            samples[i/2] = tanhf(samples[i/2] * drive);
            y_a = y_a + g * (tanhf(samples[i/2] - resonance * ((y_d_1 + y_d)/2) - tanhf(y_a)));
            y_b = y_b + g * (tanhf(y_a) - tanhf(y_b));
            y_c = y_c + g * (tanhf(y_b) - tanhf(y_c));
            y_d_1 = y_d;
            y_d = y_d + g * (tanhf(y_c) - tanhf(y_d));
            samples[i/2] = y_d;
        }
    }
    float MoogFilter::getFrequency() {
        return frequency;
    }
    float MoogFilter::getResonance() {
        return resonance;
    }
    float MoogFilter::getDrive() {
        return drive;
    }

    void MoogFilter::setFrequency(float f) {
        if (f > 12000.0f) f = 12000.0f;
        if (f < 0.0f) f = 0.0f;
        frequency = f;
        g = 1 - expf(-2 * tanf(2 * M_PI * frequency/(2 * sampleRate)));
    }
    void MoogFilter::setResonance(float r) {
        if (r > 5.0f) r = 5.0f;
        if (r < 0.0f) r = 0.0f;
        resonance = r;
    }
    void MoogFilter::setSampleRate(int s) {
        sampleRate = s;
    }
    void MoogFilter::setDrive(float d) {
        if (d > 10.0f) d = 10.0f;
        if (d < 0.1f) d = 0.1f;
        drive = d;
    }
};