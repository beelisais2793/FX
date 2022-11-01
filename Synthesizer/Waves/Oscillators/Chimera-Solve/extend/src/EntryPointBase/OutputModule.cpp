#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include "lua.hpp"

#include "def.hpp"
#include "ExtensionNaming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "StateSynchrony.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "EntryPointBase/OutputModule.hpp"

chimera::simulation::OutputModule::OutputModule()
{
    //ctor
}

chimera::simulation::OutputModule::~OutputModule()
{
    //dtor
}

void* chimera::simulation::OutputModule::getInstance(chimera::EntryPoint const * const entrypoint, vec_t_LuaItem& parameters) const
{
    return (void*)getOutputInstance(parameters);
}

size_t chimera::simulation::OutputModule::getAttribute(const std::string& attr) const
{
    if (attr == chimera::simulation::Naming::Attribute_Observer)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
