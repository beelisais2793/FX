/*
  ==============================================================================

    Modules.h
    Created: 3 Apr 2020 4:57:53pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

#include "Gain/GainProcessor.h"
#include "Impulse/ImpulseProcessor.h"
#include "Output/OutputProcessor.h"
#include "String/StringProcessor.h"
// Add Module processor headers here

class Modules {
    struct ModuleInfo{
        ModuleInfo(String name): name(name) {}
        String name;
        virtual std::unique_ptr<ModuleProcessor> create() = 0;
        virtual ~ModuleInfo() {};
    };

    template<class ProcessorType>
    struct ModuleEntry: public ModuleInfo{
        ModuleEntry(String name): ModuleInfo(name) {}
        std::unique_ptr<ModuleProcessor> create() override {
            return std::make_unique<ProcessorType>();
        }
        ~ModuleEntry() override = default;
    };

    template<class ProcessorType>
    static std::unique_ptr<ModuleInfo> moduleInfo(String name) {
        return std::make_unique<ModuleEntry<ProcessorType>>(name);
    }

    template<typename T, typename... Args>
    static std::vector<std::unique_ptr<ModuleInfo>> initFromMoveable(Args&&... args)
    {
        std::vector<std::unique_ptr<T>> vec;
        vec.reserve(sizeof...(Args));
        (vec.emplace_back(std::forward<Args>(args)), ...);
        return vec;
    }
    
public:
    static std::vector<std::unique_ptr<ModuleInfo>> moduleList() {
        return initFromMoveable<ModuleInfo>(
            moduleInfo<ImpulseProcessor>("Impulse"),
            moduleInfo<StringProcessor>("String"),
            moduleInfo<GainProcessor>("Gain"),
            moduleInfo<OutputProcessor>("Output")
            // Add Modules to list here
        );
    }
    
    static PopupMenu getMenu()
    {
        PopupMenu modulesMenu;
        
        int index = 1;
        for (auto& module : moduleList()) {
            modulesMenu.addItem(index++, module->name);
        }
        
        return modulesMenu;
    }

    static std::unique_ptr<ModuleProcessor> createProcessorFromMenuIndex(int index) { return moduleList()[index-1]->create(); }
};
