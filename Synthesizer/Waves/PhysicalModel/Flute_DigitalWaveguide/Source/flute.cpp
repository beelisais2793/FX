/*
  ==============================================================================

    flute.cpp
    Created: 26 Jul 2021 11:46:17am
    Author:  MGO

  ==============================================================================
*/

#include <JuceHeader.h>
#include "flute.h"



// Structure for biquad coeff return values
struct returnValues_biquad_coeff {        // Declare a local structure 
    double a0_, a1_, a2_, b0_, b1_, b2_;
};

returnValues_biquad_coeff biquad_LPF(double f, double Q, double Fs) { // returns a structure with the filter coefficients.. see prev struct declaration

    returnValues_biquad_coeff retVals;

    double w0 = 2 * double_Pi * f / Fs;
    double cos_w0 = cos(w0);
    double sin_w0 = sin(w0);

    double alpha = sin(w0) / (2 * Q);

    double a0 = 1 + alpha;

    double b0 = ((1 - cos(w0)) / 2) / a0; // i am normilizing the orig equations with a0. this is how Matlab freqz wants its stuff..
    double b1 = (1 - cos(w0)) / a0;
    double b2 = (1 - cos(w0)) / 2 / a0;
    double a1 = -2 * cos(w0) / a0;
    double a2 = (1 - alpha) / a0;


    retVals.a0_ = 1;
    retVals.a1_ = a1;
    retVals.a2_ = a2;
    retVals.b0_ = b0;
    retVals.b1_ = b1;
    retVals.b2_ = b2;

    return retVals;

}


double linearADSR(double fs, double sampleNo, double dur, double attack_time,
    double decay_time, double sustain_time, double release_time,
    double ampl_val_decay) {

    double k = 1 / fs;
    double sample_attack = floor(attack_time * fs);
    double sample_decay = floor(decay_time * fs);
    double sample_sustain = floor(sustain_time * fs);
    double sample_release = floor(release_time * fs);
    double sample_total = floor(dur * fs);

    double val_env = 0;

    if ((sampleNo >= 0) && (sampleNo < sample_attack)) // attack portion of ADSR
    {
        val_env = 0 + (1 - 0) * (sampleNo - 0) / (sample_attack - 0);
    }
    else if ((sampleNo >= sample_attack) && (sampleNo < sample_decay)) // decay portion of ADSR
    {
        val_env = 1 + (1 - ampl_val_decay) * (sampleNo - sample_attack) / (sample_attack - sample_decay);
    }
    else if ((sampleNo >= sample_decay) && (sampleNo < sample_sustain)) // sustain portion of ADSR
    {
        val_env = ampl_val_decay;
    }
    else if ((sampleNo >= sample_sustain) && (sampleNo < sample_release)) // release portion of ADSR
    {
        val_env = ampl_val_decay + (ampl_val_decay - 0) * (sampleNo - sample_sustain) / (sample_sustain - sample_release);
    }
    else if ((sampleNo >= sample_release))
    {
        val_env = 0;
    }

    return val_env;
}




//==============================================================================
flute::flute(double freq, double fs) : fs(fs), freq(freq)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    ifqc = freq;
    //fs = fs;

    ipress = 0.9;
    ibreath = 0.008;
    ifeedback1 = 0.4;
    ifeedback2 = 0.4;
    dur = 1.0;
    gate = 1.0;
    vibrato = 0.00;
    freq_vibrato = 5.0;
    f_biquad = 3000;
    Q_biquad = 0.707;

    dur = 10003.0;
    sus_time = 10002.0;
    rel_time = 10001.0;

    returnValues_biquad_coeff biquad_coeffs = biquad_LPF(f_biquad, Q_biquad, fs);

    a0 = biquad_coeffs.a0_;
    a1 = biquad_coeffs.a1_;
    a2 = biquad_coeffs.a2_;
    b0 = biquad_coeffs.b0_;
    b1 = biquad_coeffs.b1_;
    b2 = biquad_coeffs.b2_;

    currentSample = 0;

    delayLine_bore.resize(BUFFER_SIZE, 0);
    delayLine_emb.resize(BUFFER_SIZE, 0);

    feedbackArray = 0;
    feedbackArray_prev = 0;

    lowPass_in_nm2 = 0;
    lowPass_in_nm1 = 0;
    lowPass_in_n = 0;
    lowPass_out_nm2 = 0;
    lowPass_out_nm1 = 0;
    lowPass_out_n = 0;

    in_n = 0;
    in_nm1 = 0;
    out_nm1 = 0;

    preOutput = 0;
}

flute::~flute()
{
}

void flute::re_init_Flute_filterStates()
{

    sus_time = 10000.0; // some really large values so as to keep
    rel_time = 10001.0;
    dur = 10002.0;

    returnValues_biquad_coeff biquad_coeffs = biquad_LPF(f_biquad, Q_biquad, fs);

    a0 = biquad_coeffs.a0_;
    a1 = biquad_coeffs.a1_;
    a2 = biquad_coeffs.a2_;
    b0 = biquad_coeffs.b0_;
    b1 = biquad_coeffs.b1_;
    b2 = biquad_coeffs.b2_;

    currentSample = 0;

    delayLine_bore.resize(BUFFER_SIZE, 0);
    delayLine_emb.resize(BUFFER_SIZE, 0);

    feedbackArray = 0;
    feedbackArray_prev = 0;

    lowPass_in_nm2 = 0;
    lowPass_in_nm1 = 0;
    lowPass_in_n = 0;
    lowPass_out_nm2 = 0;
    lowPass_out_nm1 = 0;
    lowPass_out_n = 0;

    in_n = 0;
    in_nm1 = 0;
    out_nm1 = 0;

    preOutput = 0;
}

void flute::update_biquad(double f_biquad, double Q_biquad, double fs)
{
    returnValues_biquad_coeff biquad_coeffs = biquad_LPF(f_biquad, Q_biquad, fs);

    a0 = biquad_coeffs.a0_;
    a1 = biquad_coeffs.a1_;
    a2 = biquad_coeffs.a2_;
    b0 = biquad_coeffs.b0_;
    b1 = biquad_coeffs.b1_;
    b2 = biquad_coeffs.b2_;
}


double flute::process(double fs)
{
    if (active)
    {
        int r = rand() % 2 + 1; // random integer in range 1 to 2
        double valExcite = 1.0;
        if (r == 1)
        {
            double valExcite = 1.0;
        }
        else if (r == 2)
        {
            double valExcite = -1.0;
        }

        double kenv1 = linearADSR(fs, currentSample, dur, 0.05, 0.15, sus_time, rel_time, ipress);
        //double kenv1 = linearADSR(fs, currentSample, dur, 0.05 * dur, 0.1 * dur, dur - 0.15 * dur, dur, ipress);
        double aflow1 = valExcite * kenv1;

        double kvibr = vibrato * sin(2 * double_Pi * freq_vibrato * currentSample / fs);


        double asum1 = (ibreath * aflow1) + kenv1 + kvibr; 

        double afqc = 1 / ifqc - (asum1 / 20000) - (9 / fs) + (ifqc / 12000000);

        int afqc_samples = floor(afqc * fs);

        gReadPointer_bore = (gWritePointer_bore - afqc_samples + BUFFER_SIZE) % BUFFER_SIZE;

        feedbackArray = feedbackArray_prev;
        double aflute1 = feedbackArray;

        double asum2 = asum1 + (aflute1 * ifeedback1);

        delayLine_emb.at(gWritePointer_emb) = asum2;

        double delayTime_emb = afqc * 0.5;
        int delayTime_emb_samples = floor(delayTime_emb * fs);

        gReadPointer_emb = (gWritePointer_emb - delayTime_emb_samples + BUFFER_SIZE) % BUFFER_SIZE;


        double ax = delayLine_emb.at(gReadPointer_emb);
        double apoly = ax - ax * ax * ax;

        double asum3 = apoly + aflute1 * ifeedback2;

        lowPass_in_n = asum3;

        lowPass_out_n = b0 * lowPass_in_n + b1 * lowPass_in_nm1 + b2 * lowPass_in_nm2 - a1 * lowPass_out_nm1 - a2 * lowPass_out_nm2;


        delayLine_bore.at(gWritePointer_bore) = lowPass_out_n;

        feedbackArray = delayLine_bore.at(gReadPointer_bore);

        //double kenv2 = linearADSR(fs, currentSample, dur, 0.05 * dur, 0.1 * dur, dur - 0.15 * dur, dur, 1);
        double kenv2 = linearADSR(fs, currentSample, dur, 0.05, 0.05, sus_time, rel_time, 1);

        in_n = lowPass_out_n * kenv2;

        preOutput = in_n - in_nm1 + 0.995 * out_nm1;

        return preOutput;
    }
    else
    {
        return 0.0;
    }
}


void flute::updateStates(double preOutput)
{

    if (flag_stop)
    {
        //Logger::getCurrentLogger()->outputDebugString("flag stop true");

        sus_time = (currentSample + 100) / fs;
        rel_time = (currentSample + 100) / fs + release_time; // it gets very weird if you make a loong release time like 2 seconds or sth and by weird i mean buggy
        dur = (currentSample + 100) / fs + release_time;
        flag_stop = false;
        //Logger::getCurrentLogger()->outputDebugString("dur: (" + String(dur) + ")");
        //Logger::getCurrentLogger()->outputDebugString("flag stop false");

    }

    out_nm1 = preOutput;
    in_nm1 = in_n;

    lowPass_in_nm2 = lowPass_in_nm1;
    lowPass_in_nm1 = lowPass_in_n;
    lowPass_out_nm2 = lowPass_out_nm1;
    lowPass_out_nm1 = lowPass_out_n;

    feedbackArray_prev = feedbackArray;

    gWritePointer_emb = gWritePointer_emb + 1;
    if (gWritePointer_emb >= BUFFER_SIZE)
        gWritePointer_emb = 0;

    gWritePointer_bore = gWritePointer_bore + 1;
    if (gWritePointer_bore >= BUFFER_SIZE)
        gWritePointer_bore = 0;

    ++currentSample;
    if (currentSample > floor(dur * fs))
    {
        deactivate();
        //Logger::getCurrentLogger()->outputDebugString("deactivated");

    }
}



void flute::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("flute", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void flute::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..


}
