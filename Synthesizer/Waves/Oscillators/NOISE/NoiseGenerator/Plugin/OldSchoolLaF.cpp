/*
  ==============================================================================

    OldSchoolLaF.cpp
    Created: 17 Aug 2020 5:45:57pm
    Author:  John

  ==============================================================================
*/

#include "OldSchoolLaF.h"

OldScoolLaF::OldScoolLaF()
{
	// embed the AppleII font
	juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::PrintChar21_ttf, BinaryData::PrintChar21_ttfSize);
	setDefaultSansSerifTypeface(tface);
}

void OldScoolLaF::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown)
{
	auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
		.withMultipliedAlpha(button.isEnabled() ? 0.9f : 0.5f);

	if (isButtonDown || isMouseOverButton)
		baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.1f);

	auto width = button.getWidth() - 1.0f;
	auto height = button.getHeight() - 1.0f;

	if (width > 0 && height > 0)
	{
		g.setGradientFill(juce::ColourGradient::vertical(baseColour, 0.0f,
			baseColour.darker(0.1f), height));

		g.fillRect(button.getLocalBounds());
	}
	g.setFont(juce::Font(12.0f, juce::Font::bold));
}

void OldScoolLaF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
{
	g.fillAll(slider.findColour(juce::Slider::backgroundColourId));

	if (style == juce::Slider::LinearBar || style == juce::Slider::LinearBarVertical)
	{
		juce::Path p;

		if (style == juce::Slider::LinearBarVertical)
			p.addRectangle((float)x, sliderPos, (float)width, 1.0f + height - sliderPos);
		else
			p.addRectangle((float)x, (float)y, sliderPos - x, (float)height);

		auto baseColour = slider.findColour(juce::Slider::rotarySliderFillColourId)
			.withMultipliedSaturation(slider.isEnabled() ? 1.0f : 0.5f)
			.withMultipliedAlpha(0.8f);

		g.setColour(baseColour);
		g.fillPath(p);

		auto lineThickness = juce::jmin(15.0f, juce::jmin(width, height) * 0.45f) * 0.1f;
		g.drawRect(slider.getLocalBounds().toFloat(), lineThickness);
	}
}
