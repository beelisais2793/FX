/*
  ==============================================================================

    PhiSliderButton.h
    Created: 15 Apr 2020 12:20:11am
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
class PhiSliderButton    : public Button,
                           public Button::Listener
{
public:
    enum class LabelPosition
    {
        Above,
        Below,
        Left,
        Right
    };
    
    explicit PhiSliderButton(const String&, LabelPosition = LabelPosition::Above);
    ~PhiSliderButton();

    void paintButton (Graphics&, bool, bool) override;
    void resized() override;
    
    void lookAndFeelChanged() override;

private:
    const float SLIDER_SIZE = 12;
    const float SLIDER_WIDTH = SLIDER_SIZE * 2.5;
    const float SLIDER_CORNER = (SLIDER_SIZE-2) * 0.5;
    
    bool shouldDraw = true;
    
    LabelPosition labelPosition;
    Justification labelTextJustification;
    
    DrawablePath knob;
    
    Rectangle<int> labelBounds;
    Rectangle<float> sliderBounds;
    
    Colour textColour;
    
    Rectangle<int> getKnobBounds();
    
    void buttonStateChanged(Button*) override;
    void buttonClicked (Button*) override {};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhiSliderButton)
};
