/*
  ==============================================================================

    CustomLook.h
    Created: 3 May 2019 7:04:31pm
    Author:  quasa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class CustomLook : public LookAndFeel_V4
{
public:
	CustomLook()
	{
		setColour(Slider::rotarySliderFillColourId, Colours::aquamarine);
		setColour(Slider::rotarySliderOutlineColourId, Colours::aliceblue);
		setColour(Slider::thumbColourId, Colours::darkslateblue);
		setDefaultSansSerifTypefaceName("Orator Std Regular");

	}

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider&) override
	{

		float padding = 8;
		float diameter = jmin(width, height) - padding;				// return lesser of two values
		float length = jmin(width, height);
		float centerX = x + width / 2;
		float centerY = y + height / 2;
		float radius = diameter / 2;
		float rx = centerX - radius;
		float ry = centerY - radius;

		float angle = rotaryStartAngle + (sliderPosProportional * (rotaryEndAngle - rotaryStartAngle));

		juce::Rectangle<float> area(rx, ry, diameter, diameter);
		//juce::Rectangle<float> boundary(x, y, width, height);

		//g.setColour(Colours::darkslateblue);
		//g.drawRect(boundary);
		g.setFont(8);
		
		g.setColour(Colours::white);
		g.fillEllipse(area);
		g.setColour(Colours::black);
		g.fillEllipse(area.reduced(1.6));

		g.setColour(Colours::white);
		//g.setColour(Colours::magenta);
		//g.fillEllipse(centerX, centerY, 10, 10);

		Path tick;
		tick.addRectangle(0, -radius, 2.0, radius * 0.4);
		g.fillPath(tick, AffineTransform::rotation(angle).translated(centerX,centerY));


		g.setColour(Colours::orangered);
		Path p;
		p.addPieSegment(area.expanded(1.8), rotaryStartAngle, angle, 1.05);
		g.strokePath(p, PathStrokeType(1.2));


	}

	void drawLinearSlider(Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider& slider) override
	{
		juce::Rectangle<float> area(x + width/2 - 1, y, 2, height);

		g.setColour(Colours::white);
		g.fillRect(area);

		g.setColour(Colours::palevioletred);
		juce::Rectangle<float> pos(x + width / 2 - 8, sliderPos, 16 , 4);
		g.fillRect(pos);
	}

	void drawLinearSliderBackground(Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider& slider) override
	{

	}

	void drawLinearSliderThumb(Graphics& g, int x, int y, int widht, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider& slider) override
	{

	}

	void drawComboBox(Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& comboBox) override
	{

		auto area = comboBox.getLocalBounds();

		g.setColour(Colours::black);
		g.setFont(14);
		g.fillRect(area);

		

		g.setColour(Colours::white);
		g.drawRect(area, 1.2);
		g.drawText(comboBox.getText(), area, Justification::centred, true);

	}

	void drawPopupMenuBackground(Graphics& g, int width, int height) override
	{

		g.setColour(Colours::black);
		juce::Rectangle<float> area(0, 0, width, height);
		g.fillRect(area);
		g.setColour(Colours::white);
		g.drawRect(area, 1);

	}

	/*void drawPopupMenuItem(Graphics& g, const Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const String& text, const String& shortcutKeyText, const Drawable* icon, const Colour* textColour) override
	{

	}*/

	void drawMenuBarBackground(Graphics& g, int width, int height, bool isMouseOverBar, MenuBarComponent& menuBar) override
	{
		auto area = menuBar.getLocalBounds();
		g.setColour(Colours::black);
		g.fillRect(area);
	}


	void positionComboBoxText(ComboBox&, Label&) override
	{

	}


	void drawLabel(Graphics& g, Label& label) override
	{
		setDefaultSansSerifTypefaceName("Orator Std Regular");
		g.setColour(Colours::black);
		g.setFont(12);
		// label.setFont(Font);
		auto area = label.getLocalBounds();
		g.fillRect(area);

		g.setColour(Colours::white);
		g.drawText(label.getText(true), area, Justification::centred, true);

	}

	void drawTextEditorOutline(Graphics& g, int width, int height, TextEditor& textEditor) override
	{
		g.setColour(Colours::white);
		g.setFont(8);
		auto area = textEditor.getLocalBounds();
	}

	void fillTextEditorBackground(Graphics& g, int width, int height, TextEditor& textEditor) override
	{
		g.setColour(Colours::black);
		g.setFont(8);
		auto area = textEditor.getLocalBounds();
		g.fillRect(area);
	}



};