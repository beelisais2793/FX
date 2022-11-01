//
//  ModMatrixPanel .hpp
//  Synthlab - App
//
//  Created by Matthias Pueski on 30.04.18.
//  Copyright © 2018 Pueski. All rights reserved.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ModMatrix.h"
class AttachmentFactory;

class ModMatrixModel : public juce::TableListBoxModel,public ComboBox::Listener, public Slider::Listener {
public:
    
	ModMatrixModel(ModMatrix* matrix, AttachmentFactory* factory);
    
    virtual void paintRowBackground (juce::Graphics& g,
                                     int rowNumber,
                                     int width, int height,
                                     bool rowIsSelected) override;
    virtual void paintCell (juce::Graphics& g,
                            int rowNumber,
                            int columnId,
                            int width, int height,
                            bool rowIsSelected) override;
    virtual int getNumRows() override;

    void update();

    
	Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
	virtual void sliderValueChanged(Slider* slider) override;

    juce::var getDragSourceDescription (const juce::SparseSet<int>& currentlySelectedRows) override{
        
        return juce::var();
    }
    
private:
	ModMatrix* matrix;
	AttachmentFactory* factory;

};


class ModMatrixPanel  : public juce::Component,  public juce::ChangeListener, public ChangeBroadcaster {
    
    
public:
    ModMatrixPanel (ModMatrixModel* model);
    ~ModMatrixPanel ();
    
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void paint (juce::Graphics& g) override;
    void resized() override;
    virtual void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    juce::TableListBox* getTable() {
        return table;
    };
    
    ModMatrixModel* getModel() {
        return model;
    }
    
    

private:	
    ModMatrixModel* model = nullptr;
    juce::TableListBox* table = nullptr;
    juce::Viewport* view = nullptr;

    
};

