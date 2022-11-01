#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent() : numFlutes(61), octave(24)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // specify the number of input and output channels that we want to open
    setAudioChannels(0, 2);
    setWantsKeyboardFocus(true);
    addKeyListener(this);

    // Sliders !
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(0.5, 0.99);          // [1]
    frequencySlider.setTextValueSuffix(" [-]");     // [2]
    frequencySlider.addListener(this);             // [3]

    addAndMakeVisible(frequencyLabel);
    frequencyLabel.setText("ipress", juce::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, true); // [4]

    frequencySlider.setValue(0.9); // [5]


    addAndMakeVisible(biquadSlider);
    biquadSlider.setRange(500, 15000);          // [1]
    biquadSlider.setTextValueSuffix(" [Hz]");     // [2]
    biquadSlider.addListener(this);             // [3]
    biquadSlider.setValue(3000); // [5]

    addAndMakeVisible(biquadLabel);
    biquadLabel.setText("f_LPF", juce::dontSendNotification);
    biquadLabel.attachToComponent(&biquadSlider, true); // [4]



    addAndMakeVisible(vibratoSlider);
    vibratoSlider.setRange(0, 0.5);          // [1]
    vibratoSlider.setTextValueSuffix(" [-]");     // [2]
    vibratoSlider.addListener(this);             // [3]
    vibratoSlider.setValue(0.02); // [5]

    addAndMakeVisible(vibratoLabel);
    vibratoLabel.setText("vibrato", juce::dontSendNotification);
    vibratoLabel.attachToComponent(&vibratoSlider, true); // [4]


    addAndMakeVisible(freq_vibratoSlider);
    freq_vibratoSlider.setRange(0.1, 10);          // [1]
    freq_vibratoSlider.setTextValueSuffix(" [Hz]");     // [2]
    freq_vibratoSlider.addListener(this);             // [3]
    freq_vibratoSlider.setValue(5); // [5]

    addAndMakeVisible(freq_vibratoLabel);
    freq_vibratoLabel.setText("f_vibrato", juce::dontSendNotification);
    freq_vibratoLabel.attachToComponent(&freq_vibratoSlider, true); // [4]


    addAndMakeVisible(durSlider);
    durSlider.setRange(0.01, 10);          // [1]
    durSlider.setTextValueSuffix(" [s]");     // [2]
    durSlider.addListener(this);             // [3]
    durSlider.setValue(0.1); // [5]

    addAndMakeVisible(durLabel);
    durLabel.setText("rel_note", juce::dontSendNotification);
    durLabel.attachToComponent(&durSlider, true); // [4]

}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::sliderValueChanged(Slider* slider)
{

    if (slider == &frequencySlider)
    {
        ipress = frequencySlider.getValue();
    }
    else if (slider == &biquadSlider)
    {
        f_biquad = biquadSlider.getValue();
    }
    else if (slider == &vibratoSlider)
    {
        vibrato = vibratoSlider.getValue();
    }
    else if (slider == &freq_vibratoSlider)
    {
        freq_vibrato = freq_vibratoSlider.getValue();
    }
    else if (slider == &durSlider)
    {
        release_time = durSlider.getValue();
    }
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    freq_chosen = 220.0;

    fs = sampleRate;
    globalCurrentSample = 0;

    ipress = 0.9;
    ibreath = 0.008;
    ifeedback1 = 0.4;
    ifeedback2 = 0.4;
    dur = 2.0;
    gate = 1.0;
    vibrato = 0.0;
    freq_vibrato = 5.0;
    f_biquad = 3000;
    Q_biquad = 0.707;


    int test = 0;
    for (int i = test; i < numFlutes + test; ++i)
    {
        fluteNotes.push_back(std::make_unique<flute>(55.0 * pow(2, i / 12.0), fs));
    }

    preOutputVec.resize(numFlutes, 0);
    keyDownVec.resize(numFlutes, false);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();


    float* const outputL = bufferToFill.buffer->getWritePointer(0);
    float* const outputR = bufferToFill.buffer->getWritePointer(1);

    // updating global model parameters every buffersize.. for efficiency. may cause some clicks
    for (int iFlute = 0; iFlute < numFlutes; ++iFlute)
    {
        fluteNotes[iFlute]->set_ipress(ipress);

        fluteNotes[iFlute]->set_biquad(f_biquad, Q_biquad);
        fluteNotes[iFlute]->update_biquad(f_biquad, Q_biquad, fs);

        fluteNotes[iFlute]->set_vibrato(vibrato);
        fluteNotes[iFlute]->set_freq_vibrato(freq_vibrato);

        fluteNotes[iFlute]->set_release_time(release_time);
    }

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    { 
        for (int i = 0; i < bufferToFill.numSamples; i++)
        {
            if (channel == 0) 
            {
                double preOutput = 0;

                int sizeFlutes = fluteNotes.size();

                for (int iFlute = 0; iFlute < numFlutes; ++iFlute)
                {
                    if (fluteNotes[iFlute]->isActive())
                    {
                        preOutputVec[iFlute] = fluteNotes[iFlute]->process(fs);
                        fluteNotes[iFlute]->updateStates(preOutputVec[iFlute]);

                        preOutput = preOutput + preOutputVec[iFlute];
                    }
                }
                if (abs((preOutput)*globalAmp) > 1)
                {
                    Logger::getCurrentLogger()->outputDebugString("Output is too loud!");
                }
                else {
                    outputL[i] = (preOutput)*globalAmp;
                    outputR[i] = outputL[i];
                }
                ++globalCurrentSample;
            }
        }
    }

}

void MainComponent::releaseResources()
{

}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    auto sliderLeft = 120;
    frequencySlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    biquadSlider.setBounds(sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
    vibratoSlider.setBounds(sliderLeft, 80, getWidth() - sliderLeft - 10, 20);
    freq_vibratoSlider.setBounds(sliderLeft, 110, getWidth() - sliderLeft - 10, 20);
    durSlider.setBounds(sliderLeft, 140, getWidth() - sliderLeft - 10, 20);
}


bool MainComponent::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    switch (key.getKeyCode())
    {
    case 'Z':
        octave = 0;
        break;
    case 'X':
        octave = 12;
        break;
    case 'C':
        octave = 24;
        break;
    case 'V':
        octave = 36;
        break;
    case 'B':
        octave = 48;
        break;
    }
    return true;
}

bool MainComponent::keyStateChanged(bool isKeyDown, Component* originatingComponent)
{
    for (int i = 0; i < 13; i++)
    {
        char k = keys[i];
        int idx = i + octave;
        if (KeyPress::isKeyCurrentlyDown(k))
        {
            fluteNotes[idx]->activate(); 

            if (!keyDownVec[idx])
            {
                fluteNotes[idx]->re_init_Flute_filterStates();
                fluteNotes[idx]->flag_stop_off();
                double dur_check = fluteNotes[idx]->get_dur();
                //Logger::getCurrentLogger()->outputDebugString("dur_check: (" + String(dur_check) + ")");
                //Logger::getCurrentLogger()->outputDebugString("key" + String(k) + "pressed once");
                keyDownVec[idx] = true;
            }
        }
        else
        {
            if (keyDownVec[idx])
            {
                fluteNotes[idx]->flag_stop_on();
                keyDownVec[idx] = false;
                //Logger::getCurrentLogger()->outputDebugString("key" + String(k) + "released ");
            }
        }
    }
    return false;
}





