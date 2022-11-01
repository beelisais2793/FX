/*
  ==============================================================================

    PhiSliderButton.cpp
    Created: 15 Apr 2020 12:20:11am
    Author:  Alexandre Rodrigues

  ==============================================================================
*/
///@cond
#include <JuceHeader.h>
///@endcond
#include "PhiSliderButton.h"

//==============================================================================
PhiSliderButton::PhiSliderButton(const String& labelText, LabelPosition labelPositionToUse) :
Button{labelText},
labelPosition{labelPositionToUse},
labelTextJustification{Justification::centredTop}
{
    addAndMakeVisible(knob);
    knob.setPaintingIsUnclipped(true);
    
    setClickingTogglesState (true);
    setPaintingIsUnclipped(true);
    
    addListener(this);
}

PhiSliderButton::~PhiSliderButton()
{
}

void PhiSliderButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    if (shouldDraw)
    {
        g.setColour(textColour);
        g.drawFittedText(getName(), labelBounds, labelTextJustification, 1);
        
        g.setColour(Colour::greyLevel(0.18));
        g.fillRoundedRectangle(sliderBounds.reduced(1), SLIDER_CORNER);
    }
}

void PhiSliderButton::resized()
{
    Path knobpath{};
    
    if (getWidth() > SLIDER_WIDTH && getWidth() > SLIDER_SIZE)
    {
        shouldDraw = true;
        
        Rectangle<float> bounds{getLocalBounds().toFloat()};
        
        switch (labelPosition) {
            case LabelPosition::Above:
                sliderBounds = bounds.removeFromBottom(SLIDER_SIZE);
                labelTextJustification = Justification::centredTop;
                break;
            case LabelPosition::Below:
                sliderBounds = bounds.removeFromTop(SLIDER_SIZE);
                labelTextJustification = Justification::centredBottom;
                break;
            case LabelPosition::Left:
                sliderBounds = bounds.removeFromRight(SLIDER_WIDTH);
                labelTextJustification = Justification::centredLeft;
                break;
            case LabelPosition::Right:
                sliderBounds = bounds.removeFromLeft(SLIDER_WIDTH);
                labelTextJustification = Justification::centredRight;
                break;
        }
        
        sliderBounds = sliderBounds.withSizeKeepingCentre(SLIDER_WIDTH, SLIDER_SIZE);
        
        labelBounds = bounds.toNearestInt();
        
        Point<int> ellipsePoint(getKnobBounds().getTopLeft());
        knobpath.addEllipse(ellipsePoint.x, ellipsePoint.y, SLIDER_SIZE, SLIDER_SIZE);
    }
    else shouldDraw = false;
    
    knob.setPath(knobpath);
}

Rectangle<int> PhiSliderButton::getKnobBounds()
{
    return sliderBounds
           .toNearestInt()
           .withWidth(SLIDER_SIZE)
           .translated( getToggleState() ? sliderBounds.getWidth()-SLIDER_SIZE : 0, 0);
}

void PhiSliderButton::buttonStateChanged (Button* button)
{
    if (button == this)
    {
        Desktop::getInstance().getAnimator().animateComponent(&knob, getKnobBounds(), 1.0f, 300, false, 0.5f, 0.5f);
    }
}

void PhiSliderButton::lookAndFeelChanged()
{
    knob.setFill(findColour(Slider::thumbColourId));
    textColour = findColour(TextButton::textColourOnId);
}
