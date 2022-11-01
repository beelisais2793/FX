

#include "OscComponent.h"

OscComponent::OscComponent(const Point<float>& p, int fs, int samplesPerFrame)
: osc(new Oscillator(fs, samplesPerFrame)), touchHandler(new TouchHandler()),
touchIndicatorSize(50), touchIndicatorThickness(1), touchIndicatorAlpha(0.2f), touchIndicatorCol(Colours::white), dashFrame(0),
compSize(300), lineThickness(5), col(Colour().fromHSV(Random().nextFloat(), 1.0f, 1.0f, 1.0f)), alphaRange(Range<float>(0.2f, 0.9f)), regressionRange(Range<float>(1.0f, 50.0f)), saturationRange(Range<float>(0.2f, 1.0f)),
refreshRate(30), idleCounter(0)
{
	setBounds(p.x-compSize/2, p.y-compSize/2, compSize, compSize);
    size = compSize-50;
    
    idleIndicatorSize = compSize*0.85;

    regressionRef = regressionRange.getStart();
    orderRef = osc->getOrder();
    teethRef = osc->getTeeth();
    phaseRef = osc->getPhaseOffset();
    rRef = osc->getRadius();

    setSaturation(regressionRef/regressionRange.getEnd()+saturationRange.getStart()); // set saturation according to f0

    drawPoly();
    
    startTimerHz(refreshRate);
}


OscComponent::~OscComponent()
{
	delete touchHandler;
	delete osc;
}

//==============================================================================

void OscComponent::setActive()
{
	active = true;

}

void OscComponent::setInactive()
{
    active = false;
}

void OscComponent::markAsActive()
{
	toFront(true);
}

void OscComponent::synthWaveform(float* buff)
{
    return osc->synthesizeWaveform(buff);
}

void OscComponent::renderTouchPoints(Graphics& g)
{
    g.setColour(touchIndicatorCol);
    g.setOpacity(touchIndicatorAlpha);

    for(int i = 0; i < touchHandler->getNumPoints(); i++) // touch indicator circle for all touch points
    {
        g.drawEllipse(touchHandler->getTouchPos(i).x-touchIndicatorSize/2, touchHandler->getTouchPos(i).y-touchIndicatorSize/2, touchIndicatorSize, touchIndicatorSize, touchIndicatorThickness);
        
    }

    if(touchHandler->getNumPoints() == 2) // 2 touchpoints
    {
        g.drawDashedLine(Line<float>(touchHandler->getTouchPos(0), touchHandler->getTouchPos(1)), touchIndicatorDash, 2, touchIndicatorThickness, 0);
    }
    else if(touchHandler->getNumPoints() == 3) // 3 touchpoints
    {
        // triangle outline
        g.drawDashedLine(Line<float>(touchHandler->getTouchPos(0), touchHandler->getTouchPos(1)), touchIndicatorDash, 2, touchIndicatorThickness, 0);
        g.drawDashedLine(Line<float>(touchHandler->getTouchPos(0), touchHandler->getTouchPos(2)), touchIndicatorDash, 2, touchIndicatorThickness, 0);
        g.drawDashedLine(Line<float>(touchHandler->getTouchPos(1), touchHandler->getTouchPos(2)), touchIndicatorDash, 2, touchIndicatorThickness, 0);
        // triangle fill
        g.setOpacity(touchIndicatorAlpha*0.1);
        Path tri;
        tri.addTriangle(touchHandler->getTouchPos(0), touchHandler->getTouchPos(1), touchHandler->getTouchPos(2));
        g.fillPath(tri);
    }
    
}
//==============================================================================

void OscComponent::setBrightness(const float& brightness)
{
    col = col.withBrightness(brightness);
}

void OscComponent::setSaturation(const float& saturation)
{
    col = col.withSaturation(saturationRange.clipValue(saturation));
}

void OscComponent::setTransparency(const float& alpha)
{
    setAlpha(alphaRange.clipValue(alpha));
}

//==============================================================================

void OscComponent::timerCallback()
{   
    if(touchHandler->getNumPoints() == 1)
        idleCounter++; // increment idle time counter 

	repaint();
}

//==============================================================================

void OscComponent::paint(Graphics& g)
{
    renderSelectionIndicator(g);
    renderPoly(g);

    if(idleCounter >= refreshRate/4)
    {
        setTransparency(1.0f); // render full opaque on interaction
        renderIdleIndicator(g);
    }
    else
        setTransparency(*ramp); // change transparency according to envelope

}
void OscComponent::resized()
{

}

//==============================================================================

void OscComponent::mouseDown(const MouseEvent& e)
{
    idleCounter = 0; // reset idle time counter
    touchHandler->addTouchPoint(e);
    
    if(touchHandler->getNumPoints() == 1) // selection and dragging of oscillators
    {
        markAsActive();

        setBounds(getX()-compSize/2, getY()-compSize/2, compSize*2, compSize*2); // resize component to facilitate interactions
        drawPoly();
    }
}

void OscComponent::mouseUp(const MouseEvent& e)
{
    idleCounter = 0; // reset idle time counter
    touchHandler->rmTouchPoint(e);

    // update parameter reference for incremental change
    orderRef = osc->getOrder();
    teethRef = osc->getTeeth();
    phaseRef = osc->getPhaseOffset();
    rRef = osc->getRadius();

    if(touchHandler->getNumPoints() == 0)
    {
        setBounds(getX()+compSize/2, getY()+compSize/2, compSize, compSize); // reset size
        drawPoly();
    }
}

void OscComponent::mouseDrag(const MouseEvent& e)
{
    touchHandler->updatePoints(e);

    switch(touchHandler->getNumPoints()) // mapping based on number of touch points
    {
        case 1:
            if(idleCounter >= refreshRate/4) // exceeding 0.5 second idle time
            {
                touchHandler->sampleTouchPointCoordinate(e); // sample the coordinates of the touch point over time
                regressionRef = regressionRange.clipValue(regressionRef + touchHandler->getCircularRegression());

                osc->updateFreq(TouchHandler::linToExp(regressionRef, regressionRange, Range<float>(osc->getFreqLimits().getStart(), osc->getFreqLimits().getEnd())));
                setSaturation(regressionRef/regressionRange.getEnd()+saturationRange.getStart());
            }
            else
            {
                idleCounter = 0; // reset idle time counter
                setCentrePosition(e.getScreenX(), e.getScreenY());
            }
            break;
        case 2:
            osc->updateRadius(rRef + touchHandler->getAnchorRadiusDelta()); // ref + delta
            osc->updateOrder(orderRef + touchHandler->getAnchorAngleDelta()*20);

            osc->generatePolygon();
            drawPoly(); // re-draw polygon
            break;
        case 3:
            osc->updateTeeth(teethRef - touchHandler->getTriAreaDelta());
            osc->updatePhaseOffset(phaseRef - touchHandler->getTriRotationDelta()*4);

            osc->generatePolygon();
            drawPoly();
            break;
        default:
            break;
    }
}
//==============================================================================

void OscComponent::mapRamp(float* amp)
{
    ramp = amp;
}

//==============================================================================

void OscComponent::renderSelectionIndicator(Graphics& g)
{
    if(active)
    {
        // selection indicator graphics
    }
}

void OscComponent::renderIdleIndicator(Graphics& g)
{
        Path idleCircle;
        idleCircle.addEllipse(compSize-idleIndicatorSize/2, compSize-idleIndicatorSize/2, idleIndicatorSize, idleIndicatorSize);
        g.setColour(touchIndicatorCol);
        g.setOpacity(touchIndicatorAlpha);
        PathStrokeType idleStroke = PathStrokeType(touchIndicatorThickness/2);
        dashFrame++;
        idleStroke.createDashedStroke(idleCircle, idleCircle, idleIndicatorDash[int(floor(dashFrame/(refreshRate/5))) % 4], 4);
        g.strokePath(idleCircle, idleStroke);
}

void OscComponent::renderPoly(Graphics& g)
{
    g.setColour(col);
    g.strokePath(polyPath, PathStrokeType(lineThickness));
}

void OscComponent::drawPoly()
{
    polyPath.clear();
    polyPath.startNewSubPath(mapToScreenCoords(Point<float>(osc->getDrawCoords(0)))); // starting the path at the first point

    for (int i=1; i < osc->getBufferSize(); i++)
    {
        polyPath.lineTo(mapToScreenCoords(Point<float>(osc->getDrawCoords(i)))); // add line segment to consequitve points
    }

    polyPath.lineTo(mapToScreenCoords(Point<float>(osc->getDrawCoords(0)))); // connect last and first point to fill gap
}

Point<float> OscComponent::mapToScreenCoords(const Point<float>& coords)
{

    Point<float> p = coords;
    
    p.x+= 1.0f; // nudge the range to 0 to 2
    p.y+= 1.0f;

    p.x *= size/2; // scale
    p.y *= size/2;

    p.x+= (getWidth()-size)/2; // offset
    p.y+= (getHeight()-size)/2;
    
    return p;
}
