/*
  ==============================================================================

    ModuleUI.h
    Created: 21 Apr 2020 7:39:35pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "Plug.h"
class ModuleProcessor;

//==============================================================================
/*
 ModuleUI - The base class for all modules' UI view
*/
class ModuleUI    : public AudioProcessorEditor
{
public:
    
    struct Props
    {
        const String name; /// This module's name
        const Array<String> inlets; /// An Array of inlet names to display in the UI (will correspond to the processor's inlets starting from the first)
        const Array<String> outlets; /// An Array of outlet names to display in the UI (will correspond to the processor's outlets starting from the first)
        const int width; /// The default width to use when creating this module
        const int height; /// The default height to use when creating this module
        const int minimumHeight; /// The minimum height that the module should have to present its UI, any less, and the ModuleBox will collapse to only display the header
        ModuleProcessor& processor; /// A reference to the ModuleProcessor that spawned this UI
        bool isOutput = false;
    };
    
    ModuleUI(Props);
    ~ModuleUI();
    
    /** This module's props
    @see ModuleUI::Props
     */
    Props props;
    
    /// The module's inlets
    OwnedArray<Plug> inlets;
    /// The module's outlets
    OwnedArray<Plug> outlets;
    
    /// The NodeID belonging to the parent module, this is used for referencing the module's processing node in the audio graph
    AudioProcessorGraph::NodeID nodeID;
    
    /**
     The base-class overrides resized() and exposes wasResized(),
     containing the actual module's rectangle.
    */
    void resized() override;
    /** The function used by derived classes of ModuleUI to resize their contents,
    its argument is the rectangle that the content of a derived class should be bound to.
    */
    virtual void wasResized(Rectangle<int>) {};
    
private:
    
    /// Places all plugs in an Array equidistant in a column ( top -> bottom ).
    void placePlugs(OwnedArray<Plug>&, Rectangle<int>);
    /**
     Places the inlets and outlets of this ModuleUI given a rectangle and returns the remaining area
     The result is used to bound the module contents via wasResized().
    */
    const Rectangle<int> placeInletsAndOutlets (Rectangle<int>);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModuleUI)
};
