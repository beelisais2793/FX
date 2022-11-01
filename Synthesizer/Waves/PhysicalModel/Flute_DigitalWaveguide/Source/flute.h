/*
  ==============================================================================

    flute.h
    Created: 26 Jul 2021 11:46:17am
    Author:  MGO

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define BUFFER_SIZE 32768

//==============================================================================
/*
*/
class flute : public juce::Component
{
public:
    flute(double freq, double fs);

    ~flute() override;

    void re_init_Flute_filterStates();

    double process(double fs);
    void updateStates(double preOutput);

    void paint(juce::Graphics&) override;
    void resized() override;

    bool isActive() { return active; };
    void activate() { active = true; };
    void deactivate() { active = false; };

    void set_ipress(double val) { ipress = val; }
    void set_biquad(double val, double val2) { f_biquad = val; Q_biquad = val2; }

    void set_vibrato(double val) { vibrato = val; }
    void set_freq_vibrato(double val) { freq_vibrato = val; }

    void set_release_time(double val) { release_time = val; }

    void set_dur(double val) { dur = val; }
    void set_sus_time(double val) { sus_time = val; }
    void set_rel_time(double val) { rel_time = val; }

    double get_dur() { return dur; }
    double get_currentSample() { return currentSample; }

    void flag_stop_on() { flag_stop = true; }
    void flag_stop_off() { flag_stop = false; }

    void update_biquad(double f_biquad_, double Q_biquad_, double fs_ );

    void resetCurrentSample() { currentSample = 0; }

private:


    bool active = false;
    bool flag_stop = false;

    double fs, freq;
    
    double sus_time, rel_time;
    double ifqc;

    double release_time;

    double scl, pch, ipress, ibreath, ifeedback1, ifeedback2, dur, gate, vibrato, freq_vibrato, f_biquad, Q_biquad;

    double currentSample;
    double preOutput;

    double a0, a1, a2, b0, b1, b2; // biquad filter coeff

    int gWritePointer_bore = 0;
    int gWritePointer_emb = 0;

    int gReadPointer_bore = 0;
    int gReadPointer_emb = 0;

    std::vector<double> delayLine_bore;
    std::vector<double> delayLine_emb;

    double lowPass_in_nm2;
    double lowPass_in_nm1;
    double lowPass_in_n;
    double lowPass_out_nm2;
    double lowPass_out_nm1;
    double lowPass_out_n;

    double feedbackArray;
    double feedbackArray_prev;

    double out_nm1;
    double in_n;
    double in_nm1;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (flute)
};
