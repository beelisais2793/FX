#pragma once

#include <JuceHeader.h>
//#include "../SenselWrapper/SenselWrapper.h"
#include "flute.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                       public KeyListener,
                       public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;


    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    virtual bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
    virtual bool keyStateChanged(bool isKeyDown, Component* originatingComponent) override;

    void sliderValueChanged(Slider* slider) override;



private:
    //==============================================================================
    // Your private member variables go here...

    bool keyDown = false;

    std::vector<bool> keyDownVec;

    bool graphicsToggle = true;

    double fs;

    double freq_chosen;

    double scl, pch, ipress, ibreath, ifeedback1, ifeedback2, dur, gate, vibrato, freq_vibrato, f_biquad, Q_biquad;

    double release_time;

    // SLIDER STUFF

    Slider frequencySlider;
    Label frequencyLabel;

    Slider biquadSlider;
    Label biquadLabel;

    Slider vibratoSlider;
    Label vibratoLabel;

    Slider freq_vibratoSlider;
    Label freq_vibratoLabel;

    Slider durSlider;
    Label durLabel;

    double globalAmp = 0.07 ;
    double globalCurrentSample;

    std::vector<double> preOutputVec;

    int numFlutes;
    int octave;
    std::vector<char> keys = { 'A', 'W', 'S', 'E', 'D', 'F', 'T', 'G', 'Y', 'H', 'U', 'J', 'K' };

    std::vector<std::unique_ptr<flute>> fluteNotes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};