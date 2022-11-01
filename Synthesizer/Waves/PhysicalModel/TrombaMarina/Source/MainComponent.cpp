/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : pluckNow(false), bowVelocity (0.1f), frequency (220), bowPoint(0.1), bowForce(0.0)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // specify the number of input and output channels that we want to open
    setAudioChannels (2, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    fs = sampleRate;
    spring.setSampleRate(sampleRate);
    vio.setSampleRate(sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    
    if (pluckNow.load())
    {
        //vio.setBowingPoint(bowPoint.load());//delay.setDelayLengthInSamples(fs/frequency.load());
        //vio.setFrequency(frequency.load());
    }
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        float* const channelData = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
        
        if (channel == 0)
        {
            for (int i = 0; i < bufferToFill.buffer->getNumSamples(); i++)
            {
                
                float out = 0.0f;
                
                //if (pluckNow.load())
                out = vio.getOutput(bowForce.load(), bowVelocity.load());
                
                channelData[i] = out;
            }
        }
        else
        {
            memcpy (channelData,
                    bufferToFill.buffer->getReadPointer (0),
                    bufferToFill.numSamples * sizeof (float));
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::mouseDown (const MouseEvent& e)
{
    pluckNow.store (true);
    bowForce.store (0.01);
}
void MainComponent::mouseDrag (const MouseEvent& e)
{
    float freq = e.getScreenY()+ 50;
   // bowVelocity.store (e.getScreenY()/1200.0f);
    //bowForce.store (e.getScreenX()/1200.0f);
    bowForce.store (0.01);

    frequency.store (freq);
    //std::cout << e.getScreenX()/1200.0f << "\n";
}
void MainComponent::mouseUp (const MouseEvent& e)
{
    pluckNow.store (false);
    bowForce.store (0.0f);
    bowPoint.store (e.getScreenX()/1200.0f);
}
