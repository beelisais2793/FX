#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "def.hpp"
#include "ExtensionNaming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "StateSynchrony.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "EntryPointBase/OutputMultiModule.hpp"

chimera::simulation::OutputMultiModule::OutputMultiModule()
{
    //ctor
}

chimera::simulation::OutputMultiModule::~OutputMultiModule()
{
    //dtor
}

void* chimera::simulation::OutputMultiModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getOutputInstance(parameters);
}

size_t chimera::simulation::OutputMultiModule::getAttribute(const std::string& attr) const
{
    if (attr == chimera::simulation::Naming::Attribute_MultiObserver)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
