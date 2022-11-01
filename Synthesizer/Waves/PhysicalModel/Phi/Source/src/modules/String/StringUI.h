/*
  ==============================================================================

    StringUI.h
    Created: 25 Mar 2020 3:14:32pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "../../ModuleProcessor.h"
#include "../../ui/PhiDial.h"
#include "../../ui/PhiSliderButton.h"

class StringUI    : public ModuleUI
{
public:
    StringUI(ModuleProcessor& processor) :
    ModuleUI({
        // All modules must initialize these properties
        .name =  "String",
        .inlets = {"In 1", "In 2"},
        .outlets = {"Out 1", "Out 2"},
        .width = 330,
        .height = 160,
        .minimumHeight = 100,
        .processor = processor
    }),
    frequencyDial( "Frequency" , 20.0f, 10000.0f, 0.3f, " Hz"),
    positionDial(  "Position"  , 0.0f , 1.0f    , 1.0f, " %",  0),
    dampDial(      "Damping"   , 0.0f , 1.0f    , 1.0f, " %",  0),
    decayDial(     "Decay"     , 0.0f , 1.0f    , 1.0f, " %",  0),
    modeButton(    "Mode", PhiSliderButton::LabelPosition::Left),
    frequencyAttachment(*processor.params.getParameter("freq"), frequencyDial),
    positionAttachment(*processor.params.getParameter("pos"), positionDial),
    dampAttachment(*processor.params.getParameter("damp"), dampDial),
    decayAttachment(*processor.params.getParameter("decay"), decayDial),
    modeAttachment(*processor.params.getParameter("mode"), modeButton)
    {
        addAndMakeVisible(frequencyDial);
        addAndMakeVisible(positionDial);
        addAndMakeVisible(dampDial);
        addAndMakeVisible(decayDial);
        addAndMakeVisible(modeButton);
        
        frequencyDial.setValue(440.0f);
    }
    
    ~StringUI() {}

    void paint (Graphics&) override {}
    
    void wasResized(Rectangle<int> moduleBounds) override
    {
        const int dialWidth = (float)moduleBounds.getWidth()/4.0f;
        
        auto buttonRow = moduleBounds.removeFromTop(30);
        
        modeButton.setBounds(buttonRow.removeFromLeft(70));
        
        frequencyDial.setBounds( moduleBounds.removeFromLeft(dialWidth) );
        positionDial.setBounds( moduleBounds.removeFromLeft(dialWidth) );
        dampDial.setBounds( moduleBounds.removeFromLeft(dialWidth) );
        decayDial.setBounds( moduleBounds.removeFromLeft(dialWidth) );
    }

private:
    PhiDial frequencyDial, positionDial, dampDial, decayDial;
    PhiSliderButton modeButton;
    SliderParameterAttachment frequencyAttachment, positionAttachment, dampAttachment, decayAttachment;
    ButtonParameterAttachment modeAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringUI)
};
