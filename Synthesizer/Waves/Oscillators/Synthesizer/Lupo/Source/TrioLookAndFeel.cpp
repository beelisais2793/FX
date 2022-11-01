/*
  ==============================================================================

    TrioLookAndFeel.cpp
    Created: 2 Jun 2016 6:38:57pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#include "TrioLookAndFeel.h"


TrioLookAndFeel::TrioLookAndFeel() {
    
}

TrioLookAndFeel::~TrioLookAndFeel() {
    
}

void TrioLookAndFeel::drawRotarySlider	(	Graphics & 	g,
                                             int 	x,
                                             int 	y,
                                             int 	width,
                                             int 	height,
                                             float 	sliderPosProportional,
                                             float 	rotaryStartAngle,
                                             float 	rotaryEndAngle,
                                             Slider & 	slider )
{
    //LookAndFeel_V2::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
    
    // This is the binary image data that uses very little CPU when rotating
    Image myStrip = ImageCache::getFromMemory (BinaryData::Knob_64_png, BinaryData::Knob_64_pngSize);
    Image dropShadow = ImageCache::getFromMemory(BinaryData::knob_shadow_64_png, BinaryData::knob_shadow_64_pngSize);

    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const float radius = jmin (width / 2.0f, height / 2.0f) ;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius - 1.0f;
    const float ry = centreY - radius - 1.0f;
    const float rw = radius * 2.0f;
    const float thickness = 0.9f;
    
    const double fractRotation = (slider.getValue() - slider.getMinimum())  /
                                 (slider.getMaximum() - slider.getMinimum()); //value between 0 and 1 for current amount of rotation
    
    const int nFrames = myStrip.getHeight()/myStrip.getWidth(); // number of frames for vertical film strip
    const int frameIdx = (int)ceil(sliderPosProportional  * ((double)nFrames-1.0) ); // current index from 0 --> nFrames-1
    
    // Logger::getCurrentLogger()->writeToLog("==========" +String(sliderPosProportional));
    
    g.setColour(Colours::darkorange.darker());
    {
        Path filledArc;
        filledArc.addPieSegment (rx+1, ry+1, rw-0.5, rw-0.5, rotaryStartAngle, angle, thickness);
        g.fillPath (filledArc);
    }
	

	g.drawImage(dropShadow, 0, 0, slider.getWidth(), slider.getWidth(), 0, 0, dropShadow.getWidth(), dropShadow.getHeight());



    g.drawImage(myStrip,
                (int)rx,
                (int)ry,
                2*(int)radius,
                2*(int)radius,   //Dest
                0,
                frameIdx*myStrip.getWidth(),
                myStrip.getWidth(),
                myStrip.getWidth()); //Source
    
}


void TrioLookAndFeel::drawToggleButton (Graphics& g, ToggleButton& button,
                                        bool isMouseOverButton, bool isButtonDown)
{
    
    if (button.getToggleState()) {
        drawShinyButtonShape(g,
                                             0,0,
                                             button.getHeight(),button.getHeight(),
                                             5.0f,
                                             Colours::darkorange.darker(),
                                             1.0f,
                                             false,false,false,false);
    }
    
    else {
        drawShinyButtonShape(g,
                                             0,0,
                                             button.getHeight(),button.getHeight(),
                                             5.0f,
                                             Colours::darkgrey,
                                             1.0f,
                                             false,false,false,false);
    }
    
    
    
    
    float fontSize = jmin (15.0f, button.getHeight() * 0.75f);
    g.setColour (button.findColour (ToggleButton::textColourId));
    g.setFont (fontSize);
    
    const int textX = button.getWidth() / 2 + 5;
     
    g.drawFittedText (button.getButtonText(),
                      textX, 0,
                      button.getWidth() - textX - 2, button.getHeight(),
                      Justification::centredLeft, 10);
    
}

Font TrioLookAndFeel::getPopupMenuFont()
{
    return (Font("Verdana", "bold", 12.0f));
}

void TrioLookAndFeel::drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                        const bool isSeparator, const bool isActive,
                                        const bool isHighlighted, const bool isTicked,
                                        const bool hasSubMenu, const String& text,
                                        const String& shortcutKeyText,
                                        const Drawable* icon, const Colour* const textColourToUse)
{
    if (isSeparator)
    {
        Rectangle<int> r (area.reduced (5, 0));
        r.removeFromTop (r.getHeight() / 2 - 1);
        
        g.setColour (Colour (0x33000000));
        g.fillRect (r.removeFromTop (1));
        
        g.setColour (Colour (0x66ffffff));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        Colour textColour (findColour (PopupMenu::textColourId));
        
        if (textColourToUse != nullptr)
            textColour = *textColourToUse;
        
        Rectangle<int> r (area.reduced (1));
        
        if (isHighlighted)
        {
            g.setColour (Colours::darkgrey);
            g.fillRect (r);
            
            g.setColour (findColour (PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour);
        }
        
        if (! isActive)
            g.setOpacity (0.3f);
        
        Font font (getPopupMenuFont());
        
        const float maxFontHeight = area.getHeight() / 1.3f;
        
        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);
        
        g.setFont (font);
        
        Rectangle<float> iconArea (r.removeFromLeft ((r.getHeight() * 5) / 4).reduced (3).toFloat());
        
        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
        }
        else if (isTicked)
        {
            const Path tick (getTickShape (1.0f));
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea, true));
        }
        
        if (hasSubMenu)
        {
            const float arrowH = 0.6f * getPopupMenuFont().getAscent();
            
            const float x = (float) r.removeFromRight ((int) arrowH).getX();
            const float halfH = (float) r.getCentreY();
            
            Path p;
            p.addTriangle (x, halfH - arrowH * 0.5f,
                           x, halfH + arrowH * 0.5f,
                           x + arrowH * 0.6f, halfH);
            
            g.fillPath (p);
        }
        
        r.removeFromRight (3);
        g.drawFittedText (text, r, Justification::centredLeft, 1);
        
        if (shortcutKeyText.isNotEmpty())
        {
            Font f2 (font);
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);
            
            g.drawText (shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}

void TrioLookAndFeel::drawPopupMenuBackground(Graphics &g, int width, int height)
{
	auto background = Colours::darkgrey;

	g.fillAll(background);
	g.setColour(background.overlaidWith(Colour(0x2badd8e6)));



#if ! JUCE_MAC
	g.setColour(findColour(PopupMenu::textColourId).withAlpha(0.6f));
	g.drawRect(0, 0, width, height);
#endif
}

void TrioLookAndFeel::drawLabel (Graphics& g, Label& label)
{
    
    g.fillAll (label.findColour (Label::backgroundColourId));
    
    if (! label.isBeingEdited())
    {
        const float alpha = label.isEnabled() ? 1.0f : 0.5f;
        const Font font (getLabelFont (label));
        
        g.setColour (Colours::lightgrey.withMultipliedAlpha (alpha));
        g.setFont (Font("Arial","bold",13.0f));
        
        Rectangle<int> textArea (label.getBorderSize().subtractedFrom (label.getLocalBounds()).expanded(3, 1));
    
     
        g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                          jmax (1, (int) (textArea.getHeight() / font.getHeight())),
                          label.getMinimumHorizontalScale());
     
         
        g.setColour (label.findColour (Label::outlineColourId).withMultipliedAlpha (alpha));
    }
    else if (label.isEnabled())
    {
        g.setColour (label.findColour (Label::outlineColourId));
    }
    
    g.drawRect (label.getLocalBounds());
    
    //LookAndFeel_V2::drawLabel(g, label);
}

void TrioLookAndFeel::drawComboBox (Graphics& g, int width, int height, const bool isButtonDown,
                                   int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box)
{
    // LookAndFeel_V1::drawComboBox(g, width, height, isButtonDown, buttonX, buttonY, buttonW, buttonH, box);
    
    
    g.fillAll (Colours::darkgrey);
    
    if (box.isEnabled() && box.hasKeyboardFocus (false))
    {
        g.setColour (box.findColour (ComboBox::buttonColourId));
        g.drawRect (0, 0, width, height, 2);
    }
    else
    {
        g.setColour (box.findColour (ComboBox::outlineColourId));
        g.drawRect (0, 0, width, height);
    }
    
    const float outlineThickness = box.isEnabled() ? (isButtonDown ? 1.2f : 0.5f) : 0.3f;
	const Colour baseColour = Colours::darkgrey;
    /*
    const Colour baseColour (LookAndFeelHelpers::createBaseColour (box.findColour (ComboBox::buttonColourId),
                                                                   box.hasKeyboardFocus (true),
                                                                   false, isButtonDown)
                             .withMultipliedAlpha (box.isEnabled() ? 1.0f : 0.5f));
							 */
    
    drawGlassLozenge (g,
                      buttonX + outlineThickness, buttonY + outlineThickness,
                      buttonW - outlineThickness * 2.0f, buttonH - outlineThickness * 2.0f,
                      baseColour, outlineThickness, -1.0f,
                      true, true, true, true);
    
    if (box.isEnabled())
    {
        const float arrowX = 0.3f;
        const float arrowH = 0.2f;
        
        Path p;
        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);
        
        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);
        
        g.setColour (box.findColour (ComboBox::arrowColourId));
        g.fillPath (p);
    }
   
}


void TrioLookAndFeel::drawGroupComponentOutline(Graphics& g, int width, int height,
	const String& text, const Justification& position,
	GroupComponent& group)
{
	const float textH = 15.0f;
	const float indent = 3.0f;
	const float textEdgeGap = 4.0f;
	auto cs = 2.0f;

	Font f(textH);

	Path p;
	auto x = indent;
	auto y = f.getAscent() + 0.0f;
	auto w = jmax(0.0f, width - x * 2.0f);
	auto h = jmax(0.0f, height - y - indent);
	cs = jmin(cs, w * 0.5f, h * 0.5f);
	auto cs2 = 2.0f * cs;

	auto textW = text.isEmpty() ? 0 : jlimit(0.0f, jmax(0.0f, w - cs2 - textEdgeGap * 2), f.getStringWidth(text) + textEdgeGap * 2.0f);
	auto textX = cs + textEdgeGap;

	if (position.testFlags(Justification::horizontallyCentred))
		textX = cs + (w - cs2 - textW) * 0.5f;
	else if (position.testFlags(Justification::right))
		textX = w - cs - textW - textEdgeGap;

	p.startNewSubPath(x + textX + textW, y);
	p.lineTo(x + w - cs, y);

	p.addArc(x + w - cs2, y, cs2, cs2, 0, MathConstants<float>::halfPi);
	p.lineTo(x + w, y + h - cs);

	p.addArc(x + w - cs2, y + h - cs2, cs2, cs2, MathConstants<float>::halfPi, MathConstants<float>::pi);
	p.lineTo(x + cs, y + h);

	p.addArc(x, y + h - cs2, cs2, cs2, MathConstants<float>::pi, MathConstants<float>::pi * 1.5f);
	p.lineTo(x, y + cs);

	p.addArc(x, y, cs2, cs2, MathConstants<float>::pi * 1.5f, MathConstants<float>::twoPi);
	p.lineTo(x + textX, y);

	auto alpha = group.isEnabled() ? 1.0f : 0.5f;


	g.setColour(Colours::darkgrey.withMultipliedAlpha(alpha));
	
	g.fillPath(p);
	g.setColour(Colours::darkgrey.darker().withMultipliedAlpha(alpha));
	g.strokePath(p, PathStrokeType(1.0f));

	g.drawRect(roundToInt(x + textX), 3,
		roundToInt(textW),
		roundToInt(textH+2));

	g.setColour(Colours::lightgrey.withMultipliedAlpha(alpha));
	g.setFont(f);
	g.drawText(text,
		roundToInt(x + textX), 3,
		roundToInt(textW),
		roundToInt(textH),
		Justification::centred, true);

}

void TrioLookAndFeel::drawScrollbarButton (Graphics& g, ScrollBar& bar,
                                          int width, int height, int buttonDirection,
                                          bool isScrollbarVertical,
                                          bool isMouseOverButton,
                                          bool isButtonDown)
{

    
    if (isScrollbarVertical)
        width -= 2;
    else
        height -= 2;
    
    Path p;
    
    if (buttonDirection == 0)
        p.addTriangle (width * 0.5f, height * 0.2f,
                       width * 0.1f, height * 0.7f,
                       width * 0.9f, height * 0.7f);
    else if (buttonDirection == 1)
        p.addTriangle (width * 0.8f, height * 0.5f,
                       width * 0.3f, height * 0.1f,
                       width * 0.3f, height * 0.9f);
    else if (buttonDirection == 2)
        p.addTriangle (width * 0.5f, height * 0.8f,
                       width * 0.1f, height * 0.3f,
                       width * 0.9f, height * 0.3f);
    else if (buttonDirection == 3)
        p.addTriangle (width * 0.2f, height * 0.5f,
                       width * 0.7f, height * 0.1f,
                       width * 0.7f, height * 0.9f);
    
    if (isButtonDown)
        g.setColour (Colours::grey);
    else if (isMouseOverButton)
        g.setColour (Colours::grey.withAlpha (0.7f));
    else
        g.setColour (bar.findColour (ScrollBar::thumbColourId).withAlpha (0.5f));
    
    g.fillPath (p);
    
    g.setColour (Colours::black.withAlpha (0.5f));
    g.strokePath (p, PathStrokeType (0.5f));
}

void TrioLookAndFeel::drawScrollbar (Graphics& g, ScrollBar& bar,
                                    int x, int y, int width, int height,
                                    bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                                    bool isMouseOver, bool isMouseDown)
{
    g.fillAll (Colours::darkgrey.darker());
    
    g.setColour (bar.findColour (ScrollBar::thumbColourId)
                 .withAlpha ((isMouseOver || isMouseDown) ? 0.4f : 0.15f));
    
    if (thumbSize > 0.0f)
    {
        Rectangle<int> thumb;
        
        if (isScrollbarVertical)
        {
            width -= 2;
            g.fillRect (x + roundToInt (width * 0.35f), y,
                        roundToInt (width * 0.3f), height);
            
            thumb.setBounds (x + 1, thumbStartPosition,
                             width - 2, thumbSize);
        }
        else
        {
            height -= 2;
            g.fillRect (x, y + roundToInt (height * 0.35f),
                        width, roundToInt (height * 0.3f));
            
            thumb.setBounds (thumbStartPosition, y + 1,
                             thumbSize, height - 2);
        }
        
        g.setColour (Colours::darkgrey.withAlpha ((isMouseOver || isMouseDown) ? 0.95f : 0.7f));
        
        g.fillRect (thumb);
        
        g.setColour (Colours::black.withAlpha ((isMouseOver || isMouseDown) ? 0.4f : 0.25f));
        g.drawRect (thumb.getX(), thumb.getY(), thumb.getWidth(), thumb.getHeight());
        
        if (thumbSize > 16)
        {
            for (int i = 3; --i >= 0;)
            {
                const float linePos = thumbStartPosition + thumbSize / 2 + (i - 1) * 4.0f;
                g.setColour (Colours::red.withAlpha (0.15f));
                
                if (isScrollbarVertical)
                {
                    g.drawLine (x + width * 0.2f, linePos, width * 0.8f, linePos);
                    g.setColour (Colours::white.withAlpha (0.15f));
                    g.drawLine (width * 0.2f, linePos - 1, width * 0.8f, linePos - 1);
                }
                else
                {
                    g.drawLine (linePos, height * 0.2f, linePos, height * 0.8f);
                    g.setColour (Colours::white.withAlpha (0.15f));
                    g.drawLine (linePos - 1, height * 0.2f, linePos - 1, height * 0.8f);
                }
            }
        }
    }
}

void TrioLookAndFeel::drawShinyButtonShape(Graphics& g,
	float x, float y, float w, float h,
	float maxCornerSize,
	const Colour& baseColour,
	const float strokeWidth,
	const bool flatOnLeft,
	const bool flatOnRight,
	const bool flatOnTop,
	const bool flatOnBottom) 
{
	if (w <= strokeWidth * 1.1f || h <= strokeWidth * 1.1f)
		return;

	const float cs = jmin(maxCornerSize, w * 0.5f, h * 0.5f);

	Path outline;
	outline.addRoundedRectangle(x, y, w, h, cs, cs,
		!(flatOnLeft || flatOnTop),
		!(flatOnRight || flatOnTop),
		!(flatOnLeft || flatOnBottom),
		!(flatOnRight || flatOnBottom));

	ColourGradient cg(baseColour, 0.0f, y,
		baseColour.overlaidWith(Colour(0x070000ff)), 0.0f, y + h,
		false);

	cg.addColour(0.5, baseColour.overlaidWith(Colour(0x33ffffff)));
	cg.addColour(0.51, baseColour.overlaidWith(Colour(0x110000ff)));

	g.setGradientFill(cg);
	g.fillPath(outline);

	g.setColour(Colour(0x80000000));
	g.strokePath(outline, PathStrokeType(strokeWidth));
}

void TrioLookAndFeel::drawTableHeaderBackground(Graphics &g, TableHeaderComponent &header)
{
	g.fillAll(Colours::darkgrey);

	/*
	Rectangle<int> area(header.getLocalBounds());
	area.removeFromTop(area.getHeight() / 2);

	g.setGradientFill(ColourGradient(Colour(0xffe8ebf9), 0.0f, (float)area.getY(),
		Colour(0xfff6f8f9), 0.0f, (float)area.getBottom(),
		false));
	g.fillRect(area);

	g.setColour(Colour(0x33000000));
	g.fillRect(area.removeFromBottom(1));
	*/

	for (int i = header.getNumColumns(true); --i >= 0;)
		g.fillRect(header.getColumnPosition(i).removeFromRight(1));
}
/*
void TrioLookAndFeel::drawTableHeaderColumn(Graphics &g, const String & columnName, int columnId, int width, int height, bool isMouseOver, bool isMouseDown, int columnFlags)
{
	if (isMouseDown)
		g.fillAll(Colours::red);
	else if (isMouseOver)
		g.fillAll(Colours::darkorange.darker());

	Rectangle<int> area(width, height);
	area.reduce(4, 0);

	if ((columnFlags & (TableHeaderComponent::sortedForwards | TableHeaderComponent::sortedBackwards)) != 0)
	{
		Path sortArrow;
		sortArrow.addTriangle(0.0f, 0.0f,
			0.5f, (columnFlags & TableHeaderComponent::sortedForwards) != 0 ? -0.8f : 0.8f,
			1.0f, 0.0f);

		g.setColour(Colours::black);
		g.fillPath(sortArrow, sortArrow.getTransformToScaleToFit(area.removeFromRight(height / 2).reduced(2).toFloat(), true));
	}

	g.setColour(Colours::black);
	g.setFont(Font(height * 0.5f, Font::bold));
	g.drawFittedText(columnName, area, Justification::centredLeft, 1);
}*/

void TrioLookAndFeel::drawTabButton(TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
{
	Path tabShape;
	createTabButtonShape(button, tabShape, isMouseOver, isMouseDown);

	auto activeArea = button.getActiveArea();
	tabShape.applyTransform(AffineTransform::translation((float)activeArea.getX(),
		(float)activeArea.getY()));

	// DropShadow(Colours::black.withAlpha(0.5f), 2, Point<int>(0, 1)).drawForPath(g, tabShape);

	fillTabButtonShape(button, g, tabShape, isMouseOver, isMouseDown);
	drawTabButtonText(button, g, isMouseOver, isMouseDown);
}

void TrioLookAndFeel::fillTabButtonShape(TabBarButton& button, Graphics& g, const Path& path,
	bool /*isMouseOver*/, bool /*isMouseDown*/)
{
	auto tabBackground = button.getTabBackgroundColour();
	const bool isFrontTab = button.isFrontTab();

	if (isFrontTab)
		g.setColour(Colours::lightgrey);
	else
		g.setColour(Colours::grey);

	g.fillPath(path);

	g.setColour(button.findColour(isFrontTab ? TabbedButtonBar::frontOutlineColourId
		: TabbedButtonBar::tabOutlineColourId, false)
		.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

	g.strokePath(path, PathStrokeType(isFrontTab ? 1.0f : 0.5f));
}
