/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.3

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "Model.h"
//[/Headers]

#include "GraphicalEnvelope.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
GraphicalEnvelope::GraphicalEnvelope (Model* model)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
	addMouseListener(this,true);
	attackHandler = new Rectangle<float>(10, 10,10,10);
	decayHandler = new Rectangle<float>(100,50, 10, 10);
	sustainHandler = new Rectangle<float>(200, 50, 10, 10);
	releaseHandler = new Rectangle<float>(300, 80, 10, 10);
	dashes = new float[2] {5, 5};
    //[/Constructor]
}

GraphicalEnvelope::~GraphicalEnvelope()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
	delete attackHandler;
	delete decayHandler;
	delete sustainHandler;
	delete releaseHandler;
	delete dashes;
    //[/Destructor]
}

//==============================================================================
void GraphicalEnvelope::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff616161));

    //[UserPaint] Add your own custom painting code here..


	g.setColour(Colours::grey);
	Line<float> a = Line<float>(attackHandler->getPosition().x + 5, 0, attackHandler->getPosition().x + 5, getHeight());
	Line<float> d = Line<float>(decayHandler->getPosition().x + 5, 0, decayHandler->getPosition().x + 5, getHeight());
	Line<float> s = Line<float>(sustainHandler->getPosition().x + 5, 0, sustainHandler->getPosition().x + 5, getHeight());
	Line<float> r = Line<float>(releaseHandler->getPosition().x + 5, 0, releaseHandler->getPosition().x + 5, getHeight());

	Line<float> level = Line<float>(0, sustainHandler->getPosition().y + 5, getWidth(), sustainHandler->getPosition().y + 5);

	g.drawDashedLine(a, dashes, 2, 1.0f);
	g.drawDashedLine(d, dashes, 2, 1.0f);
	g.drawDashedLine(s, dashes, 2, 1.0f);
	g.drawDashedLine(r, dashes, 2, 1.0f);
	g.drawDashedLine(level, dashes, 2, 1.0f);
	g.setColour(Colours::darkgrey.darker());
	g.drawLine(15,getHeight(), attackHandler->getPosition().x + 5,attackHandler->getPosition().y + 5, 2.0f);
	g.drawLine(attackHandler->getPosition().x + 5, attackHandler->getPosition().y + 5, decayHandler->getPosition().x + 5, decayHandler->getPosition().y + 5, 2.0f);
	g.drawLine(decayHandler->getPosition().x + 5, decayHandler->getPosition().y + 5, sustainHandler->getPosition().x + 5, sustainHandler->getPosition().y + 5, 2.0f);
	g.drawLine(sustainHandler->getPosition().x + 5, sustainHandler->getPosition().y + 5, releaseHandler->getPosition().x + 5, releaseHandler->getPosition().y + 5, 2.0f);

	if (isInsideAttackHandler) {
		g.setColour(Colours::yellow);
	}
	else {
		g.setColour(Colours::white);
	}

	g.drawRect(*attackHandler, 2.0f);

	if (isInsideDecayHandler) {
		g.setColour(Colours::yellow);
	}
	else {
		g.setColour(Colours::white);
	}

	g.drawRect(*decayHandler, 2.0f);

	if (isInsideSustainHandler) {
		g.setColour(Colours::yellow);
	}
	else {
		g.setColour(Colours::white);
	}

	g.drawRect(*sustainHandler, 2.0f);

	if (isInsideReleaseHandler) {
		g.setColour(Colours::yellow);
	}
	else {
		g.setColour(Colours::white);
	}

	g.drawRect(*releaseHandler, 2.0f);

	g.setColour(Colours::darkgrey.darker());
	g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 2.0f);


    //[/UserPaint]
}

void GraphicalEnvelope::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void GraphicalEnvelope::mouseDown(const MouseEvent& event) {

}

void GraphicalEnvelope::mouseUp(const MouseEvent& event) {

}

void GraphicalEnvelope::mouseDrag(const MouseEvent& event) {
	Point <float> pos = Point<float>(event.getPosition().x - 5, event.getPosition().y - 5);

	if (pos.x < 10)
		pos.x = 10;
	if (pos.x > getWidth() - 10)
		pos.x = getWidth() - 10;
	if (pos.y < 10)
		pos.y = 10;
	if (pos.y > getHeight() - 10)
		pos.y = getHeight() - 10;

	if (isInsideAttackHandler) {

		if (pos.y > 10)
			pos.y = 10;

		if (pos.x + 10 >= decayHandler->getPosition().x) {
			pos.x = decayHandler->getPosition().x - 10;
		}
		attackHandler->setPosition(pos);
	}
	if (isInsideDecayHandler) {
		if (pos.x + 10 >= sustainHandler->getPosition().x) {
			pos.x = sustainHandler->getPosition().x - 10;
		}
		if (pos.x - 10 <= attackHandler->getPosition().x) {
			pos.x = attackHandler->getPosition().x + 10;
		}

		decayHandler->setPosition(pos);
		sustainHandler->setPosition(sustainHandler->getPosition().x, decayHandler->getPosition().y);
	}
	if (isInsideSustainHandler) {

		if (pos.x + 10 >= releaseHandler->getPosition().x) {
			pos.x = releaseHandler->getPosition().x - 10;
		}
		if (pos.x - 10 <= decayHandler->getPosition().x) {
			pos.x = decayHandler->getPosition().x + 10;
		}

		sustainHandler->setPosition(pos);
		decayHandler->setPosition(decayHandler->getPosition().x, sustainHandler->getPosition().y);
	}
	if (isInsideReleaseHandler) {

		if (pos.x - 10 <= sustainHandler->getPosition().x) {
			pos.x = sustainHandler->getPosition().x + 10;
		}
		if (pos.y < getHeight() - 10)
			pos.y = getHeight() - 10;

		releaseHandler->setPosition(pos);
	}

	repaint();

	attack = 2.5 * (getWidth() / 100) * (attackHandler->getPosition().x  - 10)/ 100;
	decay = 2.5 * (getWidth() / 100) * abs(decayHandler->getPosition().x - attackHandler->getPosition().x - 10) / 100;
	sustain = ((getHeight() - sustainHandler->getPosition().y - 10) / getHeight()) * 1.2;
	release = 2.5 * (getWidth() / 100) * abs(sustainHandler->getPosition().x - releaseHandler->getPosition().x - 10) / 100 - 1.5;

	updateModel();

}

void GraphicalEnvelope::mouseMove(const MouseEvent& event) {

	Point <float> pos = Point<float>(event.getPosition().x, event.getPosition().y);
	isInsideAttackHandler = attackHandler->contains(pos);
	isInsideDecayHandler = decayHandler->contains(pos);
	isInsideSustainHandler = sustainHandler->contains(pos);
	isInsideReleaseHandler = releaseHandler->contains(pos);
	repaint();
}

void GraphicalEnvelope::updateModel() {


	if (getName() == "ampEnvelope") {
		model->ampAttack = attack;
		model->ampDecay = decay;
		model->ampSustain = sustain;
		model->ampRelease = release;
	}
	if (getName() == "filterEnvelope") {
		model->fltAttack = attack;
		model->fltDecay = decay;
		model->fltSustain = sustain;
		model->fltRelease = release;
	}
	if (getName() == "auxEnvelope") {
		model->auxAttack = attack;
		model->auxDecay = decay;
		model->auxSustain = sustain;
		model->auxRelease = release;
	}

	sendChangeMessage();

}

void GraphicalEnvelope::setAttack(float attack) {
	this->attack = attack;
}
void GraphicalEnvelope::setDecay(float decay) {
	this->decay = decay;
}

void GraphicalEnvelope::setSustain(float sustain) {
	this->sustain = sustain;
}

void GraphicalEnvelope::setRelease(float release) {
	this->release = release;
}

float GraphicalEnvelope::getAttack() {
	return attack;
}

float GraphicalEnvelope::getDecay() {
	return decay;
}

float GraphicalEnvelope::getSustain() {
	return sustain;
}

float GraphicalEnvelope::getRelease() {
	return release;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="GraphicalEnvelope" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams="Model* model"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff616161"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

