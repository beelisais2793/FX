/*
  ==============================================================================

    ModuleUI.cpp
    Created: 21 Apr 2020 7:39:35pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

///@cond
#include <JuceHeader.h>
///@endcond
#include "ModuleUI.h"
#include "ModuleProcessor.h"

//==============================================================================

ModuleUI::ModuleUI(Props arguments) :
AudioProcessorEditor(arguments.processor),
props(arguments)
{
    arguments.processor.isOutput = arguments.isOutput;
    
    // Add all inlets and outlets as children and make them visible
    for (String inletName : props.inlets)
        addAndMakeVisible( inlets.add( std::make_unique<InletPlug>(inletName) ) );
    
    for (String outletName : props.outlets)
        addAndMakeVisible( outlets.add( std::make_unique<OutletPlug>(outletName) ) );
        
    setPaintingIsUnclipped(true);
    setBufferedToImage(true);
}

ModuleUI::~ModuleUI()
{
    props.processor.editorBeingDeleted(this);
}

void ModuleUI::resized()
{
    auto moduleBounds = getLocalBounds();
    moduleBounds = placeInletsAndOutlets( moduleBounds );
    
    // Call the function to resize the module
    wasResized(moduleBounds);
}

void ModuleUI::placePlugs(OwnedArray<Plug>& plugArray, Rectangle<int> plugBounds)
{
    const int plugHeight = (float)plugBounds.getHeight()/(float)plugArray.size();
    
    if (! plugArray.isEmpty())
    {
        // Divide the space to each plug
        for (auto plug : plugArray)
            plug->setBounds( plugBounds.removeFromTop(plugHeight) );
    }
}


const Rectangle<int> ModuleUI::placeInletsAndOutlets(Rectangle<int> moduleBounds)
{
    placePlugs(inlets, moduleBounds.removeFromLeft(30));
    placePlugs(outlets, moduleBounds.removeFromRight(30));
    
    return moduleBounds;
}
