
#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"
#include "AudioSettings.h"
#include "Synth.h"


class MainContentComponent
:
public Component
{
public:
    MainContentComponent()
    :
    m_keyboardComponent(m_keyboardstate, MidiKeyboardComponent::horizontalKeyboard),
    m_inputDeviceManager(m_sharedAudioSettings.getInputDeviceManager()),
    m_outputDeviceManager(m_sharedAudioSettings.getOutputDeviceManager())
	{
        m_synthPlayer = new PercSynthPlayer(m_keyboardstate, m_inputDeviceManager, m_outputDeviceManager);
        
		setSize(900, 300);
        
        addAndMakeVisible (m_keyboardComponent);
        m_keyboardComponent.setAvailableRange(21, 108);
        
        addAndMakeVisible (m_volumeSlider);
        addAndMakeVisible (m_volumeLabel);
        m_volumeLabel.setText ("MASTER VOLUME", dontSendNotification);
        m_volumeLabel.attachToComponent (&m_volumeSlider, false);
        m_volumeSlider.setRange (0.0, 100.0);
        m_volumeSlider.setValue(50.0);
        m_volumeSlider.setComponentID ("volume");
        m_volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
        m_volumeSlider.addListener (m_synthPlayer);
        m_volumeSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        m_volumeSlider.setPopupDisplayEnabled(true, this);
        
        addAndMakeVisible(m_materials);
        m_materials.addItem("Bright metallic",1);
        m_materials.addItem("Dark woody",2);
        m_materials.setText("Select plate 'material'");
        m_materials.addListener(m_synthPlayer);
        m_materials.setComponentID("materials");
        
        addAndMakeVisible (m_dampingSlider);
        addAndMakeVisible (m_dampingLabel);
        m_dampingLabel.setText ("Damping", dontSendNotification);
        m_dampingLabel.attachToComponent (&m_dampingSlider, true);
        m_dampingSlider.setRange (0.0, 1.0);
        m_dampingSlider.setComponentID ("damping");
        m_dampingSlider.addListener (m_synthPlayer);

        addAndMakeVisible (m_stiffnessSlider);
        addAndMakeVisible (m_stiffnessLabel);
        m_stiffnessLabel.setText ("Stiffness", dontSendNotification);
        m_stiffnessLabel.attachToComponent (&m_stiffnessSlider, true);
        m_stiffnessSlider.setRange (0.0, 1.0);
        m_stiffnessSlider.setComponentID ("stiffness");  // to recognise your slider in the callback
        m_stiffnessSlider.addListener (m_synthPlayer);
        
        addAndMakeVisible (m_thickSlider);
        addAndMakeVisible (m_thickLabel);
        m_thickLabel.setText ("Thickness", dontSendNotification);
        m_thickLabel.attachToComponent (&m_thickSlider, true);
        m_thickSlider.setRange (0.05, 0.1);
        m_thickSlider.setComponentID ("thickness");  // to recognise your slider in the callback
        m_thickSlider.addListener (m_synthPlayer);
    }
    
    void resized() override
    {
        const int sliderLeft = 120;
        
        m_volumeSlider.setBounds(750, 40, 100, 120);
        
        m_dampingSlider.setBounds (sliderLeft, 40, getWidth()  * 0.33f - sliderLeft - 10, 40);
        m_stiffnessSlider.setBounds (sliderLeft, 80, getWidth()  * 0.33f - sliderLeft - 10, 40);
        m_thickSlider.setBounds (4 * sliderLeft, 40, getWidth()  * 0.33f + sliderLeft - 300, 40);
        m_materials.setBounds (4 * sliderLeft, 90, 1.5f * sliderLeft, 50);
        
        int w = m_keyboardComponent.getKeyWidth() * (7 * 7 + 3);
        int h = 80;
        m_keyboardComponent.setSize(w, h);
        m_keyboardComponent.setCentrePosition(0.5f * getWidth(), getHeight() - getHeight() / 6);
    }
    

	void paint(Graphics& g) override
    {
        g.fillAll(Colours::slategrey);
        g.setFont(18.0f);
        g.drawText("Press on the keys to strike a plate!", 50, 90, 400, 120, Justification::centred, true);
        g.setFont(11.0f);
        g.drawText("lower octaves~bigger plates // higher octaves~smaller plates", 0, 110, 500, 120, Justification::centred, true);
	}

private:

    MidiKeyboardState m_keyboardstate;
    MidiKeyboardComponent m_keyboardComponent;
    
    
    AudioSettings m_sharedAudioSettings;
    AudioDeviceManager& m_inputDeviceManager;
    AudioDeviceManager& m_outputDeviceManager;
    
    ScopedPointer<PercSynthPlayer> m_synthPlayer;
    
    Slider m_dampingSlider, m_stiffnessSlider,m_thickSlider, m_volumeSlider;
    Label m_dampingLabel, m_stiffnessLabel, m_thickLabel, m_volumeLabel;
    ComboBox m_materials;
    
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};






// This is called by the app startup code to create the main component instance
Component* createMainContentComponent()
{
	return new MainContentComponent(); 
}


#endif  // MAINCOMPONENT_H_INCLUDED
