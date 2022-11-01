/*
  ==============================================================================

    Main class for handling audio.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscComponent.h"
#include "Sequencer.h"
#include "Envelope.h"
#include "FirLpf.h"

class MainComponent : public AudioAppComponent, public ComponentListener
{
public:

    MainComponent();
    ~MainComponent();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(Graphics& g) override;
    void resized() override;

    void mouseDoubleClick(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent& e) override;

    void componentBroughtToFront(Component& component) override; // called when an oscillator instances is selected
    void componentMovedOrResized (Component &component, bool wasMoved, bool wasResized) override; // called when an oscillator is dragged

private:

    int fs; // sampling rate
    int numberOfChannels; // number of output channels
    int samplesPerFrame; // number of samples per frame

    AudioBuffer<float> synthBuff; // stores the output of individual oscillators

    void createOscillator(const Point<float>& p); // instantiates a new oscillator - gui component pair
    void removeOscillator(const String& id);
    void setActiveComponent(const String& id); // sets the selected oscillator active, and deactivates the rest

    typedef struct OscInstance
    {
        OscComponent* oscComp; // oscillator component
        Sequencer* seq;
        Envelope* env;
        FirLpf* lpf;
        OscInstance(const Point<float>& p, int fs, int samplesPerFrame, int channels)
        {
            seq = new Sequencer();
            env = new Envelope(Envelope::AR);
            env->setSamplingRate(fs);
            oscComp = new OscComponent(p, fs, samplesPerFrame);
            oscComp->mapRamp(env->getAmplitude()); // return pointer to the envelope's amplitude
            lpf = new FirLpf(fs, channels, samplesPerFrame, 21);
        }
        ~OscInstance()
        {
            delete oscComp;
            delete seq;
            delete env;
            delete lpf;
        }
    } OscInstance;

    OwnedArray<OscInstance> oscillatorBank;

    int attackMax, releaseMax; // maximum attack and release in milliseconds

    Image bg; // background image

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent);
};
