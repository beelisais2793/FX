/*
  ==============================================================================

    PhiDial.h
    Created: 9 Feb 2020 11:02:38pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond

//==============================================================================
/*
*/
class PhiDial    : public Slider,
                   public Slider::Listener
{
public:
    PhiDial(std::string, float, float, double = 1.0f, std::string = "", int = 2, Slider::Listener* = nullptr);
    ~PhiDial();
    
    void paint (Graphics& g) override;
    void resized () override;
    
    void lookAndFeelChanged() override;
    
private:
    
    NormalisableRange<double> valueRange;
    
    Path groove, dial, pointerPath;
    AffineTransform pointerRotation;
    
    Rectangle<float> box;
    Rectangle<int> nameBounds;
    Colour colour, grooveColour, nameColour;
    
    const int padding = 10;
    const int thickness = 3;
    const float startAngle = getRotaryParameters().startAngleRadians;
    const float endAngle = getRotaryParameters().endAngleRadians;
    const float angleRange = endAngle - startAngle;
    
    float size, radius;
    float angle = startAngle;
    
    bool shouldDrawText = false;
    
    void updateDial();
    
    void sliderValueChanged (Slider*) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhiDial)
};
