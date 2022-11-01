/*
  ==============================================================================

    ModuleBox.h
    Created: 8 Feb 2020 12:28:45pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "PhiColours.h"
#include "modules/Modules.h"
#include "ui/PhiToggleButton.h"

//==============================================================================
/*
*/
class ModuleBox    : public Component,
                     public ChangeListener,
                     public ChangeBroadcaster,
                     public Button::Listener,
                     public ComponentDragger,
                     public ComponentBoundsConstrainer
{
//==================================================================================
public:
    ModuleBox(std::unique_ptr<ModuleUI>, SelectedItemSet<ModuleBox*>&);
    ~ModuleBox();

    void paint(Graphics&) override;
    void resized() override;
    void moved() override;
    
    /// The hosted moduleUI
    std::unique_ptr<ModuleUI> moduleUI;
    
    /// Sets the highlight colour of the module
    void setHighlightColour(Colour&&);

//==================================================================================
private:
    const float HEADER_HEIGHT = 27.0f;
    const float CONTENT_PADDING = 10.0f;
    
    /// Our LookAndFeel instance for this module box
    struct ModuleLookAndFeel  : public LookAndFeel_V4
    {
        ModuleLookAndFeel()
        {
            setHighlightColour(Colours::cyan.withSaturation(0.5f));
            setColour(PhiColourIds::Module::Background, Colours::darkgrey.darker());
            setColour(PhiColourIds::Module::OutlineAndText, Colours::grey);
            setColour(PhiColourIds::Module::SelectedOutlineAndText, Colours::grey.brighter());
            setColour(PhiColourIds::Module::HeaderLine, Colours::grey);
            
            setColour(Slider::thumbColourId, findColour(PhiColourIds::Module::Highlight));
            setColour(Slider::rotarySliderOutlineColourId, Colour::greyLevel(0.21f));
            setColour(Slider::rotarySliderFillColourId, Colour::greyLevel(0.17f));
            setColour(Slider::textBoxHighlightColourId, Colour::greyLevel(0.2f));
            setColour(Slider::textBoxTextColourId, Colours::grey.brighter());
            setColour(Slider::textBoxOutlineColourId, Colour()); // no color
            setColour(Label::backgroundWhenEditingColourId, Colour::greyLevel(0.3f));
            setColour(CaretComponent::caretColourId, Colour::greyLevel(0.8f));
            setColour(TextEditor::focusedOutlineColourId, Colour());
            setColour(TextEditor::highlightedTextColourId, Colour::greyLevel(0.7f));
            setColour(TextButton::textColourOnId, Colours::grey.brighter());
            
            setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
        }
        
        void setModuleOn(bool isOn)
        {
            if (isOn)
            {
                setColour(Slider::thumbColourId, findColour(PhiColourIds::Module::Highlight));
                setColour(Slider::textBoxTextColourId, Colours::grey.brighter());
                setColour(TextButton::textColourOnId, Colours::grey.brighter());
            }
            else
            {
                setColour(Slider::thumbColourId, Colours::grey);
                setColour(Slider::textBoxTextColourId, Colours::grey);
                setColour(TextButton::textColourOnId, Colours::grey);
            }
        }
        
        void drawCallOutBoxBackground (CallOutBox& box, Graphics& g, const Path&, Image&) override {
            auto bounds = box.getLocalBounds().reduced(15).toFloat();
            
            g.setColour (findColour(PhiColourIds::Module::Background));
            g.fillRoundedRectangle(bounds, 2.0f);
            
            g.setColour (findColour(PhiColourIds::Module::SelectedOutlineAndText));
            g.drawRoundedRectangle(bounds, 2.0f, 2.0f);
        };
        
        /// Sets the highlight colour of the module
        void setHighlightColour(Colour colour)
        {
            setColour(PhiColourIds::Module::Highlight, colour);
        }
        
        
    } lookandfeel;

    /// This box's rectangle
    Rectangle<float> moduleBoxRectangle;
    /// The line dividing the header from the module
    Rectangle<float> headerLine;
    /// The module name's rectangle
    Rectangle<float> nameRectangle;

    //==================================================================================
    
    /// The top-left button for enabling and disabling the contained module
    PhiToggleButton powerButton;

    /// Imposes a draggable corner on the component for resizing
    ResizableCornerComponent resizer;

    /// Pointer to the MainPatcher's selection set
    SelectedItemSet<ModuleBox*>& moduleSelection;
    
    /** Used to store mouseDown selection
     @see SelectedItemSet::addToSelectionOnMouseDown, SelectedItemSet::addToSelectionOnMouseUp
    */
    bool selectionResult;

    //==================================================================================
    
    /// Opens a ColourSelector for this module's highlight colour
    void openColourSelector();

    /// Runs a funtion on every currently selected ModuleBox
    void forEachSelected(std::function<void(ModuleBox*)>);

    /// Callback for Buttons (e.g. power button)
    void buttonClicked (Button*) override;

    void mouseDown(const MouseEvent&) override;
    void mouseUp(const MouseEvent&) override;
    void mouseDrag(const MouseEvent&) override;

    /// Currently only used to get notitiffed when the module selection changes (draws a border around the module when selected))
    void changeListenerCallback (ChangeBroadcaster*) override;
    
    ///@cond
    void buttonStateChanged (Button*) override {};
    ///@endcond

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModuleBox)
};
