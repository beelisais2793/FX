/*
  ==============================================================================

    Connections.cpp
    Created: 20 Feb 2020 8:18:35pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

///@cond
#include <JuceHeader.h>
///@endcond
#include "Connections.h"

//==============================================================================

Connections::Connections() :
getConnectionPath(patchCordTypeBCallback)
{
    addChildComponent(lasso);
    
    selectedConnections.addChangeListener(this);
    
    setAlwaysOnTop(true);
    setPaintingIsUnclipped(true);
    setInterceptsMouseClicks(false, false);
}

Connections::~Connections()
{
}

void Connections::paint (Graphics& g)
{
    if(! dragPath.isEmpty())
    {
        g.setColour (Colours::grey.withAlpha(0.5f));
        g.strokePath ( dragPath, strokeType );
    }

    for (auto& connection : connections)
    {
        g.setColour (connection->colour);
        g.fillPath (connection->path);
        
        if (selectedConnections.isSelected(connection)) {
            g.setColour (findColour(PhiColourIds::Connection::SelectedStroke));
            g.strokePath (connection->path, PathStrokeType(2.0f));
        }
    }
}

void Connections::resized ()
{
}

PlugID Connections::registerPlug (uint32 moduleId, Plug* plug)
{
    return idStore.storePlug(moduleId, plug);
}

Point<float> Connections::getPlugCenterPositionFromId (PlugMode plugMode, const PlugID plugID)
{
    auto plug = idStore.getPlug(plugMode, plugID);
    
    return getLocalPoint(&*plug, plug->getLocalBounds().getCentre().toFloat()) ;
}

void Connections::updateDragPath()
{
    if (isHoldingConnection()) {
        dragPath = getConnectionPath (dragPathAnchor, getMouseXYRelative().toFloat());
        repaint();
    }
}

void Connections::updateConnectionPath (PhiConnection& connection)
{
    auto path = getConnectionPath (getPlugCenterPositionFromId(PlugMode::Inlet, connection.destination)
                                          , getPlugCenterPositionFromId(PlugMode::Outlet, connection.source));
    
    Path strokePath;
    
    strokeType.createStrokedPath(strokePath, path);
    
    connection.path.swapWithPath(strokePath);
}

void Connections::updateAllConnectionPaths ()
{
    for (auto& connection : connections)
        updateConnectionPath(*connection);
 
    repaint();
}

void Connections::onConnectionStart(PlugMode plugMode, PlugID plugID)
{
    dragPathAnchor = getPlugCenterPositionFromId(plugMode, plugID);
}

void Connections::onConnectionEnd(std::pair<PlugID, PlugID> sourceDestination)
{
    createConnection(sourceDestination);
    repaint();
}

void Connections::onConnectionRelease()
{
    dragPath.clear();
    repaint();
}

void Connections::onConnectionDrag()
{
    updateDragPath();
}

void Connections::deselectAll()
{
    selectedConnections.deselectAll();
    repaint();
}

bool Connections::containsConnectionWith (std::pair<PlugID,PlugID>& sourceDestination)
{
    auto* e = connections.begin();

    for (; e != connections.end(); ++e)
        if (sourceDestination.first == (*e)->source && sourceDestination.second == (*e)->destination)
            return true;

    return false;
}

void Connections::createConnection(std::pair<PlugID,PlugID> sourceDestination)
{
    if (!containsConnectionWith(sourceDestination)) {
        auto newConnection = connections.add(std::make_unique<PhiConnection>(sourceDestination));
        
        sendChangeMessage(); // trigger connections refresh
        
        newConnection->colour = findColour(PhiColourIds::Connection::DefaultFill);
        updateConnectionPath(*newConnection);
        repaint();
    }
}

void Connections::removeConnectionsIf(std::function<bool(PhiConnection&)> predicate)
{
    int i = 0;
    while (i < connections.size())
    {
        if (predicate(*connections[i]))
        {
            connections.remove(i);
        }
        else i++;
    }
}

void Connections::removeModule(uint32 moduleId)
{
    removeConnectionsIf( [moduleId] (PhiConnection& connection) {
        return connection.source.moduleID() == moduleId || connection.destination.moduleID() == moduleId;
    });
    idStore.removeModule(moduleId);
}

const OwnedArray<PhiConnection>& Connections::getConnections()
{
    return connections;
}

void Connections::togglePatchCordType(bool toggle)
{
    getConnectionPath = toggle ? patchCordTypeACallback : patchCordTypeBCallback;
    updateAllConnectionPaths();
}

Point<float> Connections::getMiddlePoint (Point<float> point1, Point<float> point2, bool applyWeight = false)
{
    const float distance = point1.getDistanceFrom(point2);
    const Point<float> middlePoint = point1.getPointOnCircumference(distance * 0.5, point1.getAngleToPoint(point2));
    
    if (applyWeight)
        return middlePoint.translated(0.0f, distance * CORD_WEIGHT);
    
    return middlePoint;
}

Path Connections::patchCordTypeACallback (Point<float> positionA, Point<float> positionB)
{
    Path path{};
    path.startNewSubPath (positionA);
    path.cubicTo (positionA, getMiddlePoint(positionA, positionB, true), positionB);
    return path;
}

Path Connections::patchCordTypeBCallback (Point<float> positionA, Point<float> positionB)
{
    const Point<float> middlePoint = getMiddlePoint(positionA, positionB);
    
    const float hDistance = fabsf( positionA.getX() - positionB.getX() ) * 0.2f;

    bool order = positionA.x > positionB.x;

    auto cubicHandleA = positionA.translated(order ? -hDistance : hDistance, 0);
    auto cubicHandleB = positionB.translated(order ? hDistance : -hDistance, 0);

    Path path{};
    path.startNewSubPath (positionA);
    path.cubicTo (positionA , cubicHandleA , middlePoint);
    path.cubicTo (middlePoint , cubicHandleB , positionB);
    return path;
}

void Connections::openColourSelector(Point<int> pointTo, Colour initialColour)
{
    auto colourSelector = std::make_unique<ColourSelector>(ColourSelector::showColourspace);
    colourSelector->setSize (150, 130);
    colourSelector->addChangeListener(this);
    colourSelector->setCurrentColour(initialColour);

    auto& callOutBox = CallOutBox::launchAsynchronously(
        std::move(colourSelector),
        getTopLevelComponent()->getLocalArea(this, Rectangle<int>(pointTo.x, pointTo.y, 1, 1)),
        getTopLevelComponent()
    );
    
    callOutBox.setLookAndFeel(&getParentComponent()->getLookAndFeel());
}

void Connections::mouseDown(const MouseEvent& e)
{
    // If it clicks anywhere else but a Plug
    if (! dynamic_cast<Plug*>(e.source.getComponentUnderMouse())) {
        abortConnection();
    }

    for (auto& connection : connections)
    {
        if (connection->path.contains(e.position)) {
            selectedConnections.addToSelectionOnMouseDown(connection, e.mods);
            lasso.endLasso();
            return;
        }
    }
    deselectAll();
}

void Connections::mouseMove(const MouseEvent& e)
{
    updateDragPath();
}

bool Connections::onMouseRightButton(const MouseEvent& e)
{
    for (auto& connection : connections)
    {
        if (connection->path.contains(e.position)) {
            openColourSelector(e.position.toInt(), connection->colour);
            return true;
        }
    }
    return false;
}

void Connections::deleteAllSelectedConnections()
{
    removeConnectionsIf( [this] (PhiConnection& connection) {
        return selectedConnections.isSelected(&connection);
    });
    deselectAll();
}

void Connections::refresh()
{
    updateAllConnectionPaths();
}

bool Connections::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::backspaceKey)
    {
        deleteAllSelectedConnections();
        sendChangeMessage(); // trigger connections refresh
        repaint();
        return true;
    }
    return false;
}

void Connections::findLassoItemsInArea (Array<PhiConnection*>& itemsFound, const Rectangle<int>& area)
{
    for (auto& connection : connections)
    {
        float length = connection->path.getLength();
        float distance = 0.0f;
        while (distance < length){
            if (area.contains(connection->path.getPointAlongPath(distance += 10.0f).toInt()))
            {
                itemsFound.add(connection);
                break;
            }
        }
    }
}

SelectedItemSet<PhiConnection*>& Connections::getLassoSelection() {return selectedConnections;}

void Connections::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &selectedConnections) {
        repaint();
    } else if (auto colourSelector = dynamic_cast<ColourSelector*>(source)){
//        This is where you change the colors for all selected connections
//        connections need to store their own colour to be drawn with
        
        auto selected = selectedConnections.getItemArray();
        for (auto& connection : selected)
            connection->colour = colourSelector->getCurrentColour();
        
        repaint();
    }
}
