/*
  ==============================================================================

    ModuleBox.cpp
    Created: 8 Feb 2020 12:28:45pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

///@cond
#include <JuceHeader.h>
///@endcond
#include "ModuleBox.h"

//==============================================================================
ModuleBox::ModuleBox(std::unique_ptr<ModuleUI> moduleUi, SelectedItemSet<ModuleBox*>& selectionChangeSource) :
moduleUI{std::move(moduleUi)},
powerButton{},
resizer(this, this),
moduleSelection{selectionChangeSource}
{
    powerButton.setToggleState(true, NotificationType());
    
    // Listeners ======================================================
    powerButton.addListener(this);
    moduleSelection.addChangeListener(this);
    
    // Sizes ======================================================
    // Box size constraints
    setSizeLimits(100, HEADER_HEIGHT + 5, 500, 300);
    resizer.setSize(8,8);
    
    // Visibles ======================================================
    addAndMakeVisible(*moduleUI);
    addAndMakeVisible(powerButton);
    addAndMakeVisible(resizer);
    setPaintingIsUnclipped(true);
    setBufferedToImage(true);
    setBroughtToFrontOnMouseClick(true);
    
    setLookAndFeel(&lookandfeel);
    sendLookAndFeelChange();
    
    setSize(moduleUI->props.width, moduleUI->props.height);
}

ModuleBox::~ModuleBox()
{
    moduleSelection.removeChangeListener(this);
    setLookAndFeel (nullptr);
}

//==============================================================================
void ModuleBox::paint (Graphics& g)
{
    // Box
    g.setColour (findColour(PhiColourIds::Module::Background));
    g.fillRoundedRectangle(moduleBoxRectangle, 2.0f);
    
    bool isSelected = moduleSelection.isSelected(this);
    // Outline
    g.setColour (findColour(isSelected ? PhiColourIds::Module::SelectedOutlineAndText : PhiColourIds::Module::OutlineAndText));
    g.drawRoundedRectangle(moduleBoxRectangle, 2.0f, isSelected ? 2.0f : 0.5f);
    
    // Module Name
    g.drawText(moduleUI->props.name, nameRectangle, Justification::centredLeft, false); // (uses color from outline)
    
    // Header Line
    g.setColour (findColour(PhiColourIds::Module::HeaderLine));
    g.fillRect(headerLine);
}

void ModuleBox::resized()
{
    // Check height to constrain size
    bool isCollapsed = getHeight() < moduleUI->props.minimumHeight;
    if (isCollapsed)
        setSize(getWidth(), HEADER_HEIGHT);
    
    // Module Box area (padded)
    moduleBoxRectangle = getLocalBounds().toFloat().reduced(1.5f, 1.5f);
    
    // Set box position constraints
    /**
     setMinimumOnscreenAmounts assures the full module should be on screen,
        although it still makes it possible for modules to be off screen when the window size changes,
           should learn more about this behaviour, we don't want to open a patch and have everything jumbled in a small window
     */
    setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
    
    // Place resizer in bottom left corner
    resizer.setBoundsToFit(getLocalBounds().reduced(3), Justification::bottomRight, true);
    
    // Place header line
    headerLine = Rectangle<float>(CONTENT_PADDING, HEADER_HEIGHT - 2.0f, getWidth()-CONTENT_PADDING * 2.0f, isCollapsed ? 0.0f : 1.0f);
    
    // Module area
    auto moduleRect = getLocalBounds();
    // Header area
    auto boxHeader = moduleRect.removeFromTop(HEADER_HEIGHT);
    
    // Place Power button
    powerButton.setBounds(boxHeader.removeFromLeft(35).reduced(CONTENT_PADDING,6));
    
    // Place Text
    nameRectangle = boxHeader.toFloat();
    
    // Place Module
    moduleUI->setBounds(isCollapsed ? Rectangle<int>(-13, HEADER_HEIGHT*0.5f, getWidth()+26, 0) : moduleRect.reduced(CONTENT_PADDING));// (padded)
    
    sendChangeMessage();
}

void ModuleBox::moved()
{
    sendChangeMessage();
}

void ModuleBox::setHighlightColour(Colour&& colour)
{
    lookandfeel.setHighlightColour(colour);
    lookandfeel.setModuleOn(powerButton.getToggleState());
    sendLookAndFeelChange();
}

//==============================================================================

void ModuleBox::forEachSelected(std::function<void(ModuleBox*)> callback)
{
    auto selected = moduleSelection.getItemArray();
    for (auto& module : selected)
       callback(module);
}

void ModuleBox::openColourSelector()
{
    auto colourSelector = std::make_unique<ColourSelector>(ColourSelector::showColourspace);
    colourSelector->setCurrentColour (findColour(PhiColourIds::Module::Highlight));
    colourSelector->setSize (150, 130);
    colourSelector->addChangeListener(this);

    auto& callOutBox = CallOutBox::launchAsynchronously(
        std::move(colourSelector),
        getTopLevelComponent()->getLocalArea(this, getLocalBounds()),
        getTopLevelComponent()
    );
    
    callOutBox.setLookAndFeel(&lookandfeel);
}

void ModuleBox::mouseDown(const MouseEvent& e)
{
    selectionResult = moduleSelection.addToSelectionOnMouseDown(this, e.mods);
    auto selected = moduleSelection.getItemArray();
    for (auto module : selected)
        module->startDraggingComponent(module, e);
    
    if (e.mods.isRightButtonDown()) openColourSelector();
}

void ModuleBox::mouseUp(const MouseEvent& e)
{
    moduleSelection.addToSelectionOnMouseUp(this, e.mods, e.mouseWasDraggedSinceMouseDown(), selectionResult);
}

void ModuleBox::mouseDrag(const MouseEvent& e)
{
    forEachSelected([&e](ModuleBox* module){module->dragComponent(module, e, module);});
}


//==============================================================================
void ModuleBox::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &moduleSelection) repaint();
    else if (auto colourSelector = dynamic_cast<ColourSelector*>(source)){
        forEachSelected( [colourSelector] (ModuleBox* module) {
            module->setHighlightColour(colourSelector->getCurrentColour());
        });
    }
}


void ModuleBox::buttonClicked (Button* button)
{
    if (button == &powerButton){
        auto state = button->getToggleState();
        
        moduleUI->props.processor.suspendProcessing(!state);
        lookandfeel.setModuleOn(state);
        sendLookAndFeelChange();
    }
        
}
