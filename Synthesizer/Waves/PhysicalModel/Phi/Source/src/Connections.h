/*
  ==============================================================================

    Connections.h
    Created: 20 Feb 2020 8:18:35pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "PhiColours.h"
#include "Plug.h"

//==============================================================================
/// A connection holds an inlet and an outlet (outlet, inlet)
struct PhiConnection
{
    PhiConnection(){}
    
    PhiConnection(PlugID source, PlugID destination):
    source(source),
    destination(destination)
    {}
    
    PhiConnection(std::pair<PlugID,PlugID> sourceDestination):
    source(sourceDestination.first),
    destination(sourceDestination.second)
    {}
    
    PhiConnection(const PhiConnection& other):
    source(other.source),
    destination(other.destination)
    {}
    
    bool operator== (const PhiConnection& other) const noexcept
    {
        return other.source == source && other.destination == destination;
    }
    
    PlugID source, destination;
    Path path;
    Colour colour;
};

/// The connection manager and drawer
class Connections : public Component,
                    public ChangeBroadcaster,
                    public ChangeListener,
                    public PlugHandler,
                    public LassoSource<PhiConnection*>
{
//==============================================================================
public:
    Connections();
    ~Connections();
    
    void paint (Graphics&) override;
    void resized () override;
    
    /// Registers an inlet or outlet with the Connections component, making it patchable
    PlugID registerPlug (uint32, Plug*);
    
    /// Returns all existing connections as an Array of PlugID pairs (outlet, inlet)
    const OwnedArray<PhiConnection>& getConnections();
    
    /// Removes a module and unregisters all its inlets and outlets given its nodeID
    void removeModule(uint32);
    
    /// Chooses between two drawing styles for the connections and updates them
    void togglePatchCordType(bool);
    
    void mouseDown(const MouseEvent& e) override;
    void mouseMove(const MouseEvent& e) override;
    
    bool onMouseRightButton(const MouseEvent& e);
    
    void deselectAll();
    
    void refresh();
    
    /// Listener for key presses
    bool keyPressed (const KeyPress& key) override;
    
    class HiddenLasso: public LassoComponent<PhiConnection*> {
        void paint(Graphics& g) {}
    } lasso;
    
//==============================================================================
private:
    /// This class holds the information on the inlets and outlets that currently exist in the patcher in the form of two 2D hash-maps and functions to add & remove entries.
    class IdStore
    {
        typedef std::map<uint32, std::map<uint32, Plug*>> PlugMap;
    public:
        /// The inlets map holds Inlet* and is accessed by two keys: nodeID & inletID
        PlugMap inlets;
        /// The inlets map holds Outlet* and is accessed by two keys: nodeID & outletID
        PlugMap outlets;
        
        /// Adds an entry to inlets or outlets and returns the resulting unique identifier
        PlugID storePlug (uint32 moduleID, Plug* plug)
        {
            uint32 plugId = newPlugId(moduleID, *plug);
            mapOfMode(plug->getMode())[moduleID][plugId] = plug;
            return PlugID(std::pair(moduleID, plugId));
        }
        
        /// Gets a stored inlet or outlet
        Plug* getPlug (PlugMode plugMode, PlugID plugID)
        {
            return mapOfMode(plugMode)[plugID.moduleID()][plugID.plugID()];
        }
        
        /// Generates a new ID for an inlet or outlet, given a nodeID
        uint32 newPlugId (uint32 moduleID, Plug& plug)
        {
            auto plugs = mapOfMode(plug.getMode());
            if (plugs.find(moduleID) == plugs.end()) return 0;
            return plugs[moduleID].rbegin()->first + 1;
        }
        
        PlugMap& mapOfMode(PlugMode mode)
        {
            return mode == PlugMode::Inlet ? inlets : outlets;
        }
        
        /// Unregisters all inlets and outlets, given a nodeID
        void removeModule (const uint32 moduleID)
        {
            inlets.erase(moduleID);
            outlets.erase(moduleID);
        }
        
    } idStore;
    //============================================================
    
    static constexpr float CORD_WEIGHT = 0.2;
    PathStrokeType strokeType = PathStrokeType(5.0f, PathStrokeType::JointStyle::mitered, PathStrokeType::EndCapStyle::rounded);
    
    /// All the existing connections are stored in this Array
    OwnedArray<PhiConnection> connections;
    
    /// A solo path to use when dragging connections
    Path dragPath;
    /// The point used to anchor the dragging patch cord, the other point is assumed to be the mouse
    Point<float> dragPathAnchor;
    
    /// The curretly used patch cord drawing routine
    std::function<Path(Point<float>,Point<float>)> getConnectionPath;
    
    /// The list of selected connections
    SelectedItemSet<PhiConnection*> selectedConnections;

    //============================================================
    
    void updateDragPath();
    
    /// Forces an update of all patch cords, evaulating all inlet/outlet positions
    void updateAllConnectionPaths ();
    
    void updateConnectionPath (PhiConnection& connection);
    
    void removeConnectionsIf(std::function<bool(PhiConnection&)> predicate);
    
    void deleteAllSelectedConnections();
  
    /// Finds the middle point between two points with an optional weight factor
    static Point<float> getMiddlePoint (const Point<float>, const Point<float>, bool);
    /// A callback for drawing patch cords that applies a vertical weight
    static Path patchCordTypeACallback (const Point<float>, const Point<float>);
    /// A callback for drawing patch cords with a horizontal S shape
    static Path patchCordTypeBCallback (const Point<float>, const Point<float>);
    
    /// Fetches the center position (relative to this component) of an inlet or outlet
    Point<float> getPlugCenterPositionFromId (PlugMode, const PlugID);
    
    /// Adds an entry to the connections Array and notifies ChangeListeners
    void createConnection  (std::pair<PlugID,PlugID>);
    
    bool containsConnectionWith (std::pair<PlugID,PlugID>&);
    
    void openColourSelector(Point<int> pointTo, Colour initialColour);
    
    void onConnectionStart (PlugMode, PlugID) override;
    void onConnectionEnd (std::pair<PlugID, PlugID>) override;
    void onConnectionDrag () override;
    void onConnectionRelease () override;
    
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    void findLassoItemsInArea (Array<PhiConnection*>& itemsFound, const Rectangle<int>& area) override;
    SelectedItemSet<PhiConnection*>& getLassoSelection() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Connections)
};
