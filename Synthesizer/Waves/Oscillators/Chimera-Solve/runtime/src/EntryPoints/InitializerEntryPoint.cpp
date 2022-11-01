#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include "lua.hpp"

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
#include "EntryPoint.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractInitializer.hpp"
#include "EntryPointBase/InitializerModule.hpp"
#include "EntryPoints/InitializerEntryPoint.hpp"

chimera::runtime::InitializerEntryPoint::InitializerEntryPoint()
{
    //ctor
}

chimera::runtime::InitializerEntryPoint::~InitializerEntryPoint()
{
    //dtor
}

const std::string chimera::runtime::InitializerEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_initilizer;
}

bool chimera::runtime::InitializerEntryPoint::checkModule(chimera::Module const * const module) const
{
    const chimera::simulation::InitializerModule* modcast = dynamic_cast<const chimera::simulation::InitializerModule*>(module);
    return modcast != nullptr;
}
