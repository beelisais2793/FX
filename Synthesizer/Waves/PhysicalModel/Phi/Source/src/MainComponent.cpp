
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(std::unique_ptr<AudioEngine> audioEngine) :
mainPatcher(std::move(audioEngine)),
patchCordTypeButton("Gravity"),
inoutNamesTypeButton("Hint / Label")
{
    setLookAndFeel (&phiLookAndFeel);
    
    setSize (1000, 600);
    
    addChildComponent(mainPatcher);
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&mainPatcher, false);
    
    addAndMakeVisible(patchCordTypeButton);
    addAndMakeVisible(inoutNamesTypeButton);
    
    patchCordTypeButton.addListener(this);
    inoutNamesTypeButton.addListener(this);
    
    sendLookAndFeelChange();
    
    setPaintingIsUnclipped(true);
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::darkgrey);
    g.setColour(Colours::darkgrey.darker());
    g.fillRect(topBarBounds);
}

void MainComponent::resized()
{
    Rectangle<int> windowBounds(getLocalBounds());
    topBarBounds = windowBounds.removeFromTop(50);
    
    Rectangle<int> buttonBounds(topBarBounds);
    
    patchCordTypeButton.setBounds(buttonBounds.removeFromRight(100).reduced(10));
    inoutNamesTypeButton.setBounds(buttonBounds.removeFromRight(100).reduced(10));
    
    viewport.setBounds(windowBounds);
    mainPatcher.setSize(windowBounds.getWidth()*3, windowBounds.getHeight()*3);
}

void MainComponent::buttonClicked (Button* button)
{
    if (button == &patchCordTypeButton)
        mainPatcher.togglePatchCordType(button->getToggleState());
    else if (button == &inoutNamesTypeButton)
        mainPatcher.toggleInoutType(button->getToggleState());
}
