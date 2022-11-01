#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>

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
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractEventManager.hpp"
#include "EntryPoint.hpp"
//#include "EntryPointBase/AbstractSystemDynamic.hpp"
//#include "EntryPointBase/AbstractInitializer.hpp"
#include "EntryPointBase/ObserverModule.hpp"
#include "EntryPoints/ObserverEntryPoint.hpp"

chimera::runtime::ObserverEntryPoint::ObserverEntryPoint()
{
    //ctor
}

chimera::runtime::ObserverEntryPoint::~ObserverEntryPoint()
{
    //dtor
}

const std::string chimera::runtime::ObserverEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_initilizer;
}

bool chimera::runtime::ObserverEntryPoint::checkModule(chimera::Module const * const module) const
{
    const chimera::simulation::ObserverModule* modcast = dynamic_cast<const chimera::simulation::ObserverModule*>(module);
    return modcast != nullptr;
}
