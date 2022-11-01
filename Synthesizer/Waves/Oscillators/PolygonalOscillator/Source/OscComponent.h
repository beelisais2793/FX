/*
  ==============================================================================

	Renders GUI for the oscillators and handles interactions.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Oscillator.h"
#include "TouchHandler.h"


class OscComponent : public Component, public Timer
{
public:

    OscComponent(const Point<float>& p, int fs, int samplesPerFrame);
	~OscComponent();

    void setActive();
    void setInactive();
    void markAsActive();
    void synthWaveform(float* buff); // calls Oscillator::synthesizeWaveform()
    void renderTouchPoints(Graphics& g); // renders location of touch points and their distances

    void setBrightness(const float& brightness);
    void setSaturation(const float& saturation);
    void setTransparency(const float& alpha);
    
	void timerCallback() override;

	void paint(Graphics& g) override;
    void resized() override;

    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    
    void mapRamp(float* amp); 

private:

    void renderSelectionIndicator(Graphics& g); // renders indicator for selected component
    void renderIdleIndicator(Graphics& g); // renders an indicator if trajectory based interaction is allowed
    void renderPoly(Graphics& g); // renders the geometry
    void drawPoly(); // draws the geometry from line segments
    Point<float> mapToScreenCoords(const Point<float>& coords); // maps the polygon coordinates to the component's screen coordinates

	bool active;
	
	Oscillator* osc;
	TouchHandler* touchHandler;

	float compSize; // size of the component
    float size; // size of rendered polygon
    float lineThickness; // line thickness of polygon
	Colour col;
	Path polyPath; // sequence of line segments to draw the geometry
    float* ramp; // pointer to the envelope's amplitude
    Range<float> alphaRange;
    Range<float> saturationRange;
    Range<float> regressionRange;
    
    
    float touchIndicatorSize;
    float touchIndicatorThickness;
    float touchIndicatorAlpha;
    Colour touchIndicatorCol;
    float touchIndicatorDash[2] = {15.0f, 10.0f};
    float idleIndicatorSize;
    float idleIndicatorDash[4][4] = { {15.0f, 10.0f, 15.0f, 10.0f}, {0.0f, 5.0f, 15.0f, 5.0f}, {0.0f, 10.0f, 15.0f, 0.0f}, {7.5f, 10.0f, 7.5f, 0.0f} };
    int dashFrame; // index keeping track of dash animation frame

    float regressionRef;
    float orderRef;
    float teethRef;
    float phaseRef;
    float rRef;

    int refreshRate; // refresh rate of timer callback
    int idleCounter; // counter to keep track of idle time on interaction

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscComponent);
};
