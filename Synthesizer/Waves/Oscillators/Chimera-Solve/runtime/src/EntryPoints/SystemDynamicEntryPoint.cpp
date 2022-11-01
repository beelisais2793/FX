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
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPoint.hpp"
#include "EntryPoints/SystemDynamicEntryPoint.hpp"

chimera::runtime::SystemDynamicEntryPoint::SystemDynamicEntryPoint()
{

}

chimera::runtime::SystemDynamicEntryPoint::~SystemDynamicEntryPoint()
{
    //dtor
}

const std::string chimera::runtime::SystemDynamicEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_dynamics;
}

bool chimera::runtime::SystemDynamicEntryPoint::checkModule(chimera::Module const * const module) const
{
    const chimera::simulation::SystemDynamicModule* ireg = dynamic_cast<const chimera::simulation::SystemDynamicModule*>(module);
    return ireg != nullptr;
}

void chimera::runtime::SystemDynamicEntryPoint::init()
{
    //setValue("use", {ParameterTypeSystem::getParameterID(Naming::Type_function), (void*)lua_sysdyn_use});
}
