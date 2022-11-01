/*
  ==============================================================================

    Plug.cpp
    Created: 16 Apr 2020 11:08:52pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Plug.h"

//==============================================================================
namespace PlugOptions
{
    bool drawName = false;
}

Plug::Plug(PlugMode modeToUse, const String& nameToUse) :
name(nameToUse),
mode(modeToUse)
{
    colors = mode == PlugMode::Inlet
             ? ColourPair(Colours::grey, Colours::darkgrey)
             : ColourPair(Colours::darkgrey, Colours::grey);
    
    setPaintingIsUnclipped(true);
}

void Plug::setId(uint64 ioId)
{
    plugID = ioId;
}

void Plug::paint (Graphics& g)
{
    if (PlugOptions::drawName && canFitText)
    {
        g.setColour(Colours::grey);
        g.drawFittedText(name, nameBounds, nameJustification, 3);
        setTooltip("");
    }
    else setTooltip(name); // Display the tooltip if not showing text
    
    g.setColour(colors.first);
    g.fillEllipse(bounds);
    g.setColour(colors.second);
    g.drawEllipse(bounds, 3);
}

void Plug::resized()
{
    nameJustification = Justification::Flags::centredBottom;
    nameBounds = getLocalBounds().withTrimmedBottom(getHeight() * 0.5 + 12);
    canFitText = nameBounds.getHeight() > 11;
    
    bounds = getLocalBounds().withSizeKeepingCentre(12, 12).toFloat();
}

void Plug::mouseDown(const MouseEvent& e)
{
    emitEvent(PlugEvent(PlugEvent::StartConnection, e, mode, plugID));
}

void Plug::mouseUp(const MouseEvent& e)
{
    auto topLevelComponent = getTopLevelComponent();
    auto mousePosition = topLevelComponent->getLocalPoint(this, e.position.toInt());
    auto componentUnderMouse = topLevelComponent->getComponentAt(mousePosition);
    
    if (auto plug = dynamic_cast<Plug*>(componentUnderMouse)) {
        emitEvent(PlugEvent(PlugEvent::EndConnection, e, plug->mode, plug->plugID));
    } else {
        emitEvent(PlugEvent(PlugEvent::AbortConnection, e));
    }
}

void Plug::mouseDrag(const MouseEvent& e)
{
    emitEvent(PlugEvent(PlugEvent::Drag, e, mode, plugID));
}
