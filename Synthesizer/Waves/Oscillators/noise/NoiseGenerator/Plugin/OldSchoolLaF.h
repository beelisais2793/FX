/*
  ==============================================================================

    OldScoolLaF.h
    Created: 17 Aug 2020 3:03:04pm
    Author:  John

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class OldScoolLaF : public juce::LookAndFeel_V4
{
public:
    OldScoolLaF();
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    juce::Typeface::Ptr getAppleIIfont(const juce::Font& font);
};
