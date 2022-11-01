/*
  ==============================================================================

    PhiDial.cpp
    Created: 9 Feb 2020 11:02:38pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

///@cond
#include <JuceHeader.h>
///@endcond
#include "PhiDial.h"

PhiDial::PhiDial(std::string name, float rangeLow, float rangeHigh, double skewFactor, std::string valueSuffix, int decimals, Slider::Listener* listener) :
valueRange{rangeLow, rangeHigh, 0, skewFactor}
{
    setNormalisableRange         (valueRange);
    setSliderStyle               (RotaryVerticalDrag);
    setTextBoxStyle              (TextBoxBelow, true, 80, 20);
    setTextBoxIsEditable         (true);
    
    setName                      (name);
    setTextValueSuffix           (valueSuffix);
    setNumDecimalPlacesToDisplay (decimals);
    
    if (listener) addListener(listener);
    addListener(this);
    
    setPaintingIsUnclipped(true);
    
    updateDial();
}

 PhiDial::~PhiDial()
{
}


void PhiDial::paint (Graphics& g)
{
    
    if (shouldDrawText)
    {
        g.setColour(nameColour);
        g.drawFittedText(getName(), nameBounds, Justification::centredBottom, 1);
    }

    g.setColour(grooveColour);
    g.strokePath(groove, PathStrokeType(thickness, PathStrokeType::JointStyle::mitered, PathStrokeType::EndCapStyle::rounded));
    
    g.setColour(colour);
    g.strokePath(dial, PathStrokeType(thickness, PathStrokeType::JointStyle::mitered, PathStrokeType::EndCapStyle::rounded));
    
    if (radius > 17)
    {
        g.addTransform(pointerRotation);
        g.fillPath(pointerPath);
        g.addTransform(pointerRotation.inverted());
    }

}

void PhiDial::resized ()
{
    Slider::resized();
    
    bool hasName = getName() != "";
    bool enoughHeightForName = getHeight() > 40;
    
    shouldDrawText = hasName && enoughHeightForName;
    
    auto bounds = getLocalBounds();
    
    nameBounds = bounds.removeFromTop(15);
    
    size = jmin(bounds.getWidth(), bounds.getHeight()-10);
    box = bounds.toFloat().withSizeKeepingCentre(size, size).reduced(padding).translated(0, -7);
    radius = box.getWidth() * 0.55;
    
    groove.clear();
    groove.addCentredArc(box.getCentreX(), box.getCentreY(), radius, radius, 0, startAngle, endAngle, true);
    
    updateDial();
}

void PhiDial::lookAndFeelChanged ()
{
    Slider::lookAndFeelChanged();
    colour = findColour(thumbColourId);
    grooveColour = findColour(rotarySliderOutlineColourId);
    nameColour = findColour(textBoxTextColourId);
}

void PhiDial::updateDial ()
{
    angle = startAngle + valueRange.convertTo0to1(getValue()) * angleRange;
    
    dial.clear();
    dial.addCentredArc(box.getCentreX(), box.getCentreY(), radius, radius, 0, startAngle, angle, true);
    
    if (radius > 17)
    {
        pointerPath.clear();
        pointerPath.addRoundedRectangle(box.withSizeKeepingCentre(thickness, 9).withY(box.getY()+thickness+1), thickness * 0.5);
        pointerRotation = AffineTransform::rotation(angle, box.getCentreX(), box.getCentreY());
    }
}

void PhiDial::sliderValueChanged (Slider* slider)
{
    if (slider == this) updateDial();
}
