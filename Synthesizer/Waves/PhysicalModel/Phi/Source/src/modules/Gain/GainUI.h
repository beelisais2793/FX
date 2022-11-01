/*
  ==============================================================================

    Gain.h
    Created: 29 Mar 2020 3:14:32pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "../../ModuleProcessor.h"
#include "../../ui/PhiDial.h"

class GainUI    : public ModuleUI
{
public:
    GainUI(ModuleProcessor& processor) :
    ModuleUI({
        // All modules must initialize these properties
        .name =  "Gain",
        .inlets = {"In"},
        .outlets = {"Out"},
        .width = 150,
        .height = 150,
        .minimumHeight = 100,
        .processor = processor
    }),
    gainDial("Gain", -70.0f, 12.0f, 1.0, " dB"),
    gainAttachment(*processor.params.getParameter("gain"), gainDial)
    {
        addAndMakeVisible(gainDial);
    }
    
    ~GainUI() {};

    void paint (Graphics& g) override {};
    
    void wasResized(Rectangle<int> moduleBounds) override
    {
        gainDial.setBounds( moduleBounds );
    }

private:
    PhiDial gainDial;
    SliderParameterAttachment gainAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainUI)
};
