/*
  ==============================================================================

	Stores individual touch points, and provides parameters from raw touch data.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "math.h"

class TouchHandler
{
public:

	TouchHandler();
	~TouchHandler();

	void addTouchPoint(const MouseEvent& e);
	void rmTouchPoint(const MouseEvent& e);
	int getNumPoints(); // return the total number of touch points on the screen
    void updatePoints(const MouseEvent& e); // update the location of touch points
    Point<float> getTouchPos(const int& i); // returns screen position for indexed touch point
	void sampleTouchPointCoordinate(const MouseEvent& e); // samples the coordinates of the specified touch point

	float getAnchorRadiusDelta();
	float getAnchorAngleDelta();
	float getTriAreaDelta();
	float getTriRotationDelta();
	float getCircularRegression();

	static int negMod(const int& n, const int& m); // modulo operator for negative divisors
	static float linToLog(float x, const Range<float>& inRange, const Range<float>& outRange); // linear to logarithmic scaling
	static float linToExp(float x, const Range<float>& inRange, const Range<float>& outRange);

private:

	float getDist(const Point<float>& a, const Point<float>& b); // distance between 2 points
    float getNormalizedDist(const Point<float>& a, const Point<float>& b); // calculates the normalized distance between 2 points
    float getAngle(const Point<float>& a, const Point<float>& b); // calculates the angle between 2 points
    float getNormalizedAngle(const Point<float>& a, const Point<float>& b); // calculates the normalized angle between 2 points
    float getNormalizedArea(const Point<float>& a, const Point<float>& b, const Point<float>& c); // calculates the area of a triangle defined by 3 points
    float getNormalizedRotation(const Point<float>& a, const Point<float>& b, const Point<float>& c); // calculates the rotation of a triangle defined by 3 points
    Point<float> getTriCentroid(const Point<float>& a, const Point<float>& b, const Point<float>& c); // calculates a triangles centroid defined by 3 points
	Point<float> getCircleCentroid(const Point<float>& a, const Point<float>& b, const Point<float>& c); // calulates a circles centroid defined by 3 points

    float rMax; // maximum anchor radius
    float areaMax; // maximum triangle area

    float radThreshold; // threshold for detecting a fitting point to a circles perimeter
    float radLimit; // radius limit to consider a valid circle

    float radRef; // reference for incremental change
    float alphaRef;
    float areaRef;
    float rotationRef;
    Point<float> originRef; // the triangle's centroid defined by 3 touch points

    Point<float> posBuffer[30]; // circular buffer storing sampled touch point coordinates
    int posIndex; // posBuffer index

	typedef struct TouchPoint
	{
		Point<float> pos; // position
		int sourceIndex; // MouseInputSource index
		TouchPoint(const MouseInputSource& mouseInput)
        {
			pos = mouseInput.getScreenPosition();
			sourceIndex = mouseInput.getIndex();
        }
	} TouchPoint;

	OwnedArray<TouchPoint> arrayOfTouchPoints;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TouchHandler);
	
};
