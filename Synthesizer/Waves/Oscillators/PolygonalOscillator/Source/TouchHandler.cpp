

#include "TouchHandler.h"

TouchHandler::TouchHandler()
: rMax(300), areaMax(40000), radThreshold(20), radLimit(200), posIndex(0)
{

}

TouchHandler::~TouchHandler()
{
    arrayOfTouchPoints.clear(true);
}

//==============================================================================

void TouchHandler::addTouchPoint(const MouseEvent& e)
{
    TouchPoint* tp = new TouchPoint(e.source);
    arrayOfTouchPoints.add(tp);

    // update reference parameters
    if(getNumPoints() == 2) // anchor
    {
        radRef = getNormalizedDist(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos);
        alphaRef = getNormalizedAngle(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos);
    }
    else if(getNumPoints() == 3) // polygon
    {
        areaRef = getNormalizedArea(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos, arrayOfTouchPoints[2]->pos);
        originRef = getTriCentroid(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos, arrayOfTouchPoints[2]->pos);
        rotationRef = getNormalizedRotation(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos, arrayOfTouchPoints[2]->pos);
    }
}

void TouchHandler::rmTouchPoint(const MouseEvent& e)
{
    for (int i=0; i < getNumPoints(); i++)
    {
    	if(arrayOfTouchPoints[i]->sourceIndex == e.source.getIndex())
    		arrayOfTouchPoints.removeObject(arrayOfTouchPoints[i]);
    }
    
    // update reference parameters
    if(getNumPoints() == 2) // anchor
    {
        radRef = getNormalizedDist(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos);
        alphaRef = getNormalizedAngle(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos);
    }
    else if(getNumPoints() == 3) // polygon
    {
        areaRef = getNormalizedArea(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos, arrayOfTouchPoints[2]->pos);
        rotationRef = getNormalizedRotation(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos, arrayOfTouchPoints[2]->pos);
    }
}

int TouchHandler::getNumPoints()
{
    return arrayOfTouchPoints.size();
}

void TouchHandler::updatePoints(const MouseEvent& e)
{
    for (int i=0; i < getNumPoints(); i++)
    {
        if(arrayOfTouchPoints[i]->sourceIndex == e.source.getIndex())
            arrayOfTouchPoints[i]->pos = e.source.getScreenPosition();
    }
}

Point<float> TouchHandler::getTouchPos(const int& i)
{
    return arrayOfTouchPoints[i]->pos;
}

void TouchHandler::sampleTouchPointCoordinate(const MouseEvent& e)
{
    posIndex++;
    posIndex = posIndex % 30;
    
    posBuffer[posIndex] = e.source.getScreenPosition();
}
//==============================================================================

float TouchHandler::getAnchorRadiusDelta()
{
    return getNormalizedDist(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos) - radRef;
}

float TouchHandler::getAnchorAngleDelta()
{
    return getNormalizedAngle(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos) - alphaRef;
}

float TouchHandler::getTriAreaDelta()
{
    return getNormalizedArea(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos, arrayOfTouchPoints[2]->pos) - areaRef;
}

float TouchHandler::getTriRotationDelta()
{
    return getNormalizedRotation(arrayOfTouchPoints[0]->pos, arrayOfTouchPoints[1]->pos, arrayOfTouchPoints[2]->pos) - rotationRef;
}

float TouchHandler::getCircularRegression()
{
    Point<float> cc; // circle's centroid
    float r = 0; // circle's radius
    float xc = 0; // centroid's x coordinate
    float yc = 0; // y coordinate

    int iterations = 3;
    int sparsity = 6; // sample sparsity for centroid calculation (gap between two successive points in samples)
    for(int i=1; i<=iterations; i++) // use the past nth sampled points to determine a circle
    {
        cc = getCircleCentroid(posBuffer[negMod(posIndex-i, 30)], posBuffer[negMod(posIndex-(i+sparsity), 30)], posBuffer[negMod(posIndex-(i+sparsity*2), 30)]);
        r += getDist(cc, posBuffer[negMod(posIndex-i, 30)]);

        xc += cc.x;
        yc += cc.y;
    }

    r /= iterations; // mean radius
    cc = Point<float>(xc/iterations, yc/iterations); // mean centroid

    float currAngle = getAngle(posBuffer[posIndex], cc);
    float prevAngle = getAngle(posBuffer[negMod(posIndex-1, 30)], cc);

    // fits current point to the circle's perimeter with given threshold
    if(r < radLimit && getDist(posBuffer[posIndex], cc) - r < radThreshold && abs(prevAngle-currAngle) < MathConstants<float>::pi)
    {
        // determine direction
        if(prevAngle < currAngle) // clockwise
        {
            //std::cout << "-->" << std::endl;
            return abs(prevAngle-currAngle);
        }
        else // anticlockwise
        {
            //std::cout << "<--" << std::endl;
            return -1*abs(prevAngle-currAngle);
        }
    }
    else
    {
        return 0;
    }
}

//==============================================================================

int TouchHandler::negMod(const int& n, const int& m)
{
    return ((n % m) + m) % m;
}

float TouchHandler::linToLog(float x, const Range<float>& inRange, const Range<float>& outRange)
{
    x = inRange.clipValue(x);
    return outRange.getEnd() + (log(x) - log(inRange.getEnd())) / (log(inRange.getStart()) - log(inRange.getEnd())) * (outRange.getStart() - outRange.getEnd());
}

float TouchHandler::linToExp(float x, const Range<float>& inRange, const Range<float>& outRange)
{
    x = inRange.clipValue(x);
    return exp(log(outRange.getEnd()) + (x - inRange.getEnd()) / (inRange.getStart() - inRange.getEnd()) * (log(outRange.getStart()) - log(outRange.getEnd())));
}

//==============================================================================

float TouchHandler::getDist(const Point<float>& a, const Point<float>& b)
{

    float dist = std::sqrt(std::pow(a.x-b.x,2) + std::pow(a.y-b.y,2));
    return dist;
}

float TouchHandler::getNormalizedDist(const Point<float>& a, const Point<float>& b)
{

    float dist = std::sqrt(std::pow(a.x-b.x,2) + std::pow(a.y-b.y,2));
    return dist/rMax;
}
float TouchHandler::getAngle(const Point<float>& a, const Point<float>& b)
{

    return atan2(b.y-a.y, b.x-a.x);
}

float TouchHandler::getNormalizedAngle(const Point<float>& a, const Point<float>& b)
{

    return atan2(b.y-a.y, b.x-a.x) / MathConstants<float>::pi;
}

float TouchHandler::getNormalizedArea(const Point<float>& a, const Point<float>& b, const Point<float>& c)
{
    
    float area = abs((a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y)) / 2);
    return area / areaMax;
}

float TouchHandler::getNormalizedRotation(const Point<float>& a, const Point<float>& b, const Point<float>& c)
{

    return (getAngle(a,originRef)+getAngle(b,originRef)+getAngle(c,originRef))/3 / MathConstants<float>::pi; // return the normalized mean of the 3 angles between vertices and centroid
}

Point<float> TouchHandler::getTriCentroid(const Point<float>& a, const Point<float>& b, const Point<float>& c)
{

    return Point<float>((a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3);
}

Point<float> TouchHandler::getCircleCentroid(const Point<float>& a, const Point<float>& b, const Point<float>& c)
{
    float ma = (b.y-a.y) / (b.x-a.x); // slope of line AB
    float mb = (c.y-b.y) / (c.x-b.x); // slope of line BC

    float xc = (ma*mb*(a.y-c.y) + mb*(a.x+b.x) - ma*(b.x+c.x)) / (2*(mb-ma)); // x coordinate of circle's center
    float yc = (-1/ma)*(xc-(a.x+b.x)/2) + (a.y+b.y)/2; // y coordinate
    
    return Point<float>(xc,yc);
}
