/*
  ==============================================================================

    MainPatcher.h
    Created: 29 Mar 2020 12:51:46am
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "ModuleBox.h"
#include "Connections.h"
#include "AudioEngine.h"

//==============================================================================
/// The main view of Phi, this class handles all the module creation, hosting and patching
class MainPatcher    : public Component,
                       public ChangeListener,
                       public LassoSource<ModuleBox*>
{
public:
    MainPatcher(std::unique_ptr<AudioEngine>);
    ~MainPatcher();

    void paint (Graphics&) override;
    void resized() override;
    
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    
    /// Toggles between two patch-cord drawing routines
    void togglePatchCordType(bool);
    
    /// Toggles inlet/outlet name display type
    void toggleInoutType(bool);
    
    bool keyPressed (const KeyPress& key) override;
    
private:
    /// An AudioEngine instance to run the patcher
    std::unique_ptr<AudioEngine> audioEngine;
    
    /// An OwnedArray for storing and accessing all the modules (ModuleBox's) in the patcher
    OwnedArray<ModuleBox> modules;
    
    /// The list of selected modules, it gets passed to each module because they subscribe themselves to the list, they also use it for dragging behaviour
    SelectedItemSet<ModuleBox*> selectedModules;
    
    /// The graphical component that represents connections between modules. It is drawn always on top of everything in the patcher.
    Connections connections;
    
    TooltipWindow tooltipWindow;
    
    class PhiLasso: public LassoComponent<ModuleBox*> {
        void paint(Graphics& g)
        {
            Path path;
            path.addRectangle(getLocalBounds());
            
            const float dash = 5.0f;
            PathStrokeType(2.0f).createDashedStroke(path, path, &dash, 1);
            
            g.setColour(Colours::grey);
            g.fillPath(path);
        }
    } lasso;
    
    
//  STATE undo/redo
//  ========================
//    struct CreateModule: public UndoableAction {
//        CreateModule(std::unique_ptr<ModuleProcessor> processor, Point<float> position, MainPatcher* patcher):
//        processor(std::move(processor)),
//        position(position),
//        patcher(patcher)
//        {}
//        
//        bool perform() override {
//            moduleBox = patcher->createModule(std::move(processor), position);
//            return true;
//        }
//        
//        bool undo() override {
//            if (moduleBox != nullptr) {
//                patcher->deleteModule(moduleBox);
//                return true;
//            }
//            else return false;
//        }
//        
//        std::unique_ptr<ModuleProcessor> processor;
//        Point<float> position;
//        MainPatcher* patcher;
//        ModuleBox* moduleBox;
//    };
//    
//    struct DeleteModule: public UndoableAction {
//        DeleteModule(ModuleBox* moduleBox, MainPatcher* patcher):
//        moduleBox(moduleBox)
//        {}
//        
//        bool perform() override {
//            patcher->deleteModule(moduleBox);
//            return true;
//        }
//        
//        bool undo() override {
//            
//            return true;
//        }
//    
//        Point<float> position;
//        MainPatcher* patcher;
//        ModuleBox* moduleBox;
//    };
//    
//    UndoManager undoManager;
//    
    
    
    /** Registers an array of plugs with the connections component.
     This function performs three jobs for each plug:
     registers with connections, sets the resulting registry ID in the inlet/outlet and adds connections as a listener so that it may receive actions from it. */
    void registerPlugs(OwnedArray<Plug>&, uint32);
    
    /// Runs registerPlugs() for each inlet and outet of the module
    void registerInletsAndOutlets(ModuleUI&);
    
    /// Creates a module from its ModuleProcessor at a position in the patcher,
    /// it also registers its connections and adds it to the audioEngine
    ModuleBox* createModule(std::unique_ptr<ModuleProcessor>, Point<float>);
    
    /// Deletes a module and all its connections from the patcher and audioEngine, unregisters it from connections
    void deleteModule(ModuleBox*);
    /// Runs deleteModule() for every selected module
    void deleteAllSelectedModules();
    
    void openMenu(const MouseEvent& e);
    
    /// We use this currently to get notified from Connections when a new connection is made, so that it may be correspondingly applied in the AudioEngine
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    void findLassoItemsInArea (Array<ModuleBox*>& itemsFound, const Rectangle<int>& area) override;
    SelectedItemSet<ModuleBox*>& getLassoSelection() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPatcher)
};
