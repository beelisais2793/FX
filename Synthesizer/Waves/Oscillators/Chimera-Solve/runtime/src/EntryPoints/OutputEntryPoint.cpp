#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include "lua.hpp"

//#include "interfaces/EventHandler/IParameterTypeEventHandler.hpp"
//#include "RuntimeNames.hpp"
#include "def.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/OutputMultiModule.hpp"
#include "EntryPointBase/OutputModule.hpp"
#include "EntryPoint.hpp"
#include "EntryPoints/OutputEntryPoint.hpp"

chimera::runtime::OutputEntryPoint::OutputEntryPoint()
{
    //ctor
}

chimera::runtime::OutputEntryPoint::~OutputEntryPoint()
{
    //dtor
}


const std::string chimera::runtime::OutputEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_output;
}

bool chimera::runtime::OutputEntryPoint::checkModule(chimera::Module const * const module) const
{
    const chimera::simulation::OutputModule* ireg = dynamic_cast<const chimera::simulation::OutputModule*>(module);
    if(ireg)
        return true;
    const chimera::simulation::OutputMultiModule* mireg = dynamic_cast<const chimera::simulation::OutputMultiModule*>(module);
    return mireg != nullptr;
}

void chimera::runtime::OutputEntryPoint::init()
{

}

