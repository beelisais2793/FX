
#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "PhiColours.h"
#include "MainPatcher.h"
#include "ui/PhiSliderButton.h"

//==============================================================================
/// The lowest-order component that includes all the content
class MainComponent   : public Component,
                        public Button::Listener

{
//==============================================================================
public:
    MainComponent(std::unique_ptr<AudioEngine>);
    ~MainComponent();

    void paint (Graphics& g) override;
    void resized() override;
    
    struct PhiLookAndFeel  : public LookAndFeel_V4
    {
        PhiLookAndFeel()
        {
            // PopupMenu
            setColour(PopupMenu::textColourId, Colours::lightgrey);
            setColour(PopupMenu::backgroundColourId, Colours::darkgrey.darker());
            setColour(PopupMenu::highlightedBackgroundColourId, Colour::greyLevel(0.2));
            
            // Texbutton
            setColour(TextButton::textColourOnId , Colours::grey.brighter());
            setColour(TextButton::buttonColourId , Colours::transparentBlack);
            
            // Scrollbar
            setColour(ScrollBar::thumbColourId, Colours::lightgrey.withAlpha(0.5f));
            
            // Knob
            setColour(Slider::thumbColourId, Colour::greyLevel(0.8f));
            
            // Hints
            setColour(TooltipWindow::outlineColourId, Colours::transparentBlack);
            setColour(TooltipWindow::backgroundColourId, Colours::grey);
            setColour(TooltipWindow::textColourId, Colours::darkgrey.darker());
            
            setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
            
            LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName ("Helvetica Neue");
            
            setColour(PhiColourIds::Module::Background, Colours::darkgrey.darker());
            setColour(PhiColourIds::Module::OutlineAndText, Colours::grey);
            setColour(PhiColourIds::Module::SelectedOutlineAndText, Colours::grey.brighter());
            setColour(PhiColourIds::Module::HeaderLine, Colours::grey);
            setColour(PhiColourIds::Connection::DefaultFill, Colours::grey);
            setColour(PhiColourIds::Connection::SelectedStroke, Colours::grey.brighter());
        }
        
        void drawCallOutBoxBackground (CallOutBox& box, Graphics& g, const Path&, Image&) override {
            auto bounds = box.getLocalBounds().reduced(15).toFloat();
            
            g.setColour (findColour(PhiColourIds::Module::Background));
            g.fillRoundedRectangle(bounds, 2.f);
            
            g.setColour (findColour(PhiColourIds::Module::SelectedOutlineAndText));
            g.drawRoundedRectangle(bounds, 2.f, 2);
        };
        
    } phiLookAndFeel;


//==============================================================================
private:
    /// The main editor, it has the means to create modules, connections and produce the resulting audio
    MainPatcher mainPatcher;
    
    /// The Viewport component that presents the view of mainPatcher to allow scrolling
    Viewport viewport;
    
    /// A simple button to change the patch-cord drawing method (just because)
    PhiSliderButton patchCordTypeButton;
    
    /// A simple button to toggle between inlet/outlet names being hinted or labeled
    PhiSliderButton inoutNamesTypeButton;
    
    /// The rectangle that constrains the top bar on the window
    Rectangle<int> topBarBounds;
    
    /// The callback for any button on this component
    void buttonClicked (Button*) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
