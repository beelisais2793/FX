//
//  ModMatrixPanel .cpp
//  Synthlab - App
//
//  Created by Matthias Pueski on 30.04.18.
//  Copyright © 2018 Pueski. All rights reserved.
//

#include "ModMatrixPanel.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "AttachmentFactory.h"

#ifdef JUCE_WINDOWS
#include <Windows.h>
#endif // JUCE_WI

#include <map>

using juce::File;
using juce::WildcardFileFilter;
using juce::TableListBox;
using juce::Viewport;
using juce::Colour;
using juce::String;
using juce::DirectoryContentsList;
using juce::ChangeBroadcaster;
using juce::Graphics;
using juce::ValueTree;
using juce::OutputStream;
using juce::XmlElement;
using juce::XmlDocument;
using juce::ScopedPointer;

ModMatrixPanel::ModMatrixPanel (ModMatrixModel* model)  {
    
	
    // addMouseListener(this, true);
	table = new TableListBox("modMatrixTable", model);
    table->getHeader().addColumn("Source", 1, 100);
    table->getHeader().addColumn("Destination", 2, 100);
	table->getHeader().addColumn("Amount", 3, 250);
    table->setAutoSizeMenuOptionShown(true);
    table->getHeader().setStretchToFitActive(true);
	table->setOpaque(false);
	
    this->model = model;
	
	view  = new Viewport();
    view->setViewedComponent(table);
	addAndMakeVisible(view);
    

    table->addMouseListener(this, true);
    
    model->update();
	table->updateContent();
    repaint();
}

ModMatrixPanel::~ModMatrixPanel () {
    
    delete table;
    delete view;

}

void ModMatrixPanel::mouseDrag(const juce::MouseEvent &event) {

}

void ModMatrixPanel::paint(juce::Graphics &g) {
	g.fillAll(Colours::darkgrey);
    // g.fillRect (getLocalBounds());

}

void ModMatrixPanel::resized() {
	if (view != nullptr) {
        view->setSize(getWidth(), getHeight());
		view->resized();
	}
	if (table != nullptr) {
        table->setSize(getWidth(), getHeight() - 30);		
	}
	/*
    if (getParentComponent() != nullptr) {
        setSize(getParentWidth()/2, getParentHeight());
    }
	*/
}

void ModMatrixPanel::changeListenerCallback (ChangeBroadcaster* source) {
    table->updateContent();
}


void ModMatrixPanel::mouseDown(const juce::MouseEvent &event) {


}

void ModMatrixPanel::mouseDoubleClick(const juce::MouseEvent &event) {
    
    if (table->getSelectedRow() > 0) {
    }
    else {
    }

}

//===========================================================================
// Model
//===========================================================================

ModMatrixModel::ModMatrixModel(ModMatrix* matrix, AttachmentFactory* factory) {
	this->matrix = matrix;
	this->factory = factory;
}

int ModMatrixModel::getNumRows() {
	return 6;
}

void ModMatrixModel::paintCell (Graphics& g,
                int rowNumber,
                int columnId,
                int width, int height,
                bool rowIsSelected) {
    
    
    
    String text = "";
    
    if (columnId == 1) {
		text = "Source";
    }
    else if (columnId == 2) {
		text = "Target";
    }
	else if (columnId == 3) {
		text = "10";
	}
    
	g.setColour(juce::Colours::white);
	g.drawText(text, 0, 0, width, height, juce::Justification::centredLeft);
    	
}

void ModMatrixModel::comboBoxChanged(ComboBox * comboBoxThatHasChanged)
{
	const int index = comboBoxThatHasChanged->getName().substring(comboBoxThatHasChanged->getName().lastIndexOf("_") + 1).getIntValue();
	
	if (comboBoxThatHasChanged->getName().startsWith("Source")) {
		for (int i = 0; i < 6; i++) {
			if (i == index) {
				matrix->getModulations()[i]->setModulator(matrix->getModulators().at(comboBoxThatHasChanged->getSelectedItemIndex()));
				break;
			}

		}
	}
	if (comboBoxThatHasChanged->getName().startsWith("Target")) {
		for (int i = 0; i < 6; i++) {
			if (i == index) {
				matrix->getModulations()[i]->setTarget(matrix->getModTargets().at(comboBoxThatHasChanged->getSelectedItemIndex()));
				break;
			}

		}

	}

}

void ModMatrixModel::sliderValueChanged(Slider * slider)
{
	const int index = slider->getName().substring(slider->getName().lastIndexOf("_") + 1).getIntValue();

	Modulator* m = matrix->getModulations()[index]->getModulator();
 
	if (m != NULL) {
		m->setModAmount(slider->getValue());
	}
}


void ModMatrixModel::paintRowBackground (Graphics& g,
                         int rowNumber,
                         int width, int height,
                         bool rowIsSelected) {

    
    if (rowIsSelected) {
        g.setColour(juce::Colours::orange);
    }
    else {
		g.setColour(Colours:: darkgrey);
    }
    
    g.fillRect(0,0,width,height);
	
}

void ModMatrixModel::update() {

}

Component * ModMatrixModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component * existingComponentToUpdate)
{
	
	if (columnId == 1) {
		ComboBox* sourceCombo = dynamic_cast<ComboBox*>(existingComponentToUpdate);

		if (sourceCombo == nullptr) {
			sourceCombo = new ComboBox("Source_"+String(rowNumber));

			for (std::map<int, String>::iterator it = matrix->getSources()->begin(); it != matrix->getSources()->end();) {			
				sourceCombo->addItem(it->second, it->first);
				it++;
			}
			sourceCombo->addListener(this);
			factory->createComboAttachment("Source_" + String(rowNumber), sourceCombo);
		}
		sourceCombo->setName("Source_" + String(rowNumber));
		return sourceCombo;
	}
	else if (columnId == 2) {
		
		ComboBox* targetCombo = dynamic_cast<ComboBox*>(existingComponentToUpdate);
		
		if (targetCombo == nullptr) {
			targetCombo = new ComboBox("Target_"+String(rowNumber));
			for (std::map<int, String>::iterator it = matrix->getTargets()->begin(); it != matrix->getTargets()->end();) {
				targetCombo->addItem(it->second, it->first);
				it++;
			}
			targetCombo->addListener(this);
			factory->createComboAttachment("Target_" + String(rowNumber), targetCombo);
		}
		targetCombo->setName("Target_" + String(rowNumber));
		return targetCombo;	
	}

	else if (columnId == 3) {
		
		Slider* amountSlider = dynamic_cast<Slider*>(existingComponentToUpdate);

		if (amountSlider == nullptr) {
			amountSlider = new Slider(Slider::SliderStyle::LinearBar,juce::Slider::NoTextBox);
			amountSlider->setRange(0, 20, 0.1);
			amountSlider->addListener(this);
			factory->createSliderAttachment("Amount_" + String(rowNumber), amountSlider);
		}
		amountSlider->setName("Amount_" + String(rowNumber));

		return amountSlider;
		
	}

	else {
		return nullptr;
	}
}


