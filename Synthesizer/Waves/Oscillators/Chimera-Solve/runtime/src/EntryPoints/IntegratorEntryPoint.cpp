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
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "EntryPoint.hpp"
#include "EntryPoints/InitializerEntryPoint.hpp"
#include "EntryPoints/IntegratorEntryPoint.hpp"

chimera::runtime::IntegratorEntryPoint::IntegratorEntryPoint(InitializerEntryPoint* initializer)
{
    _initializer = initializer;
}

chimera::runtime::IntegratorEntryPoint::~IntegratorEntryPoint()
{
    //dtor
}

const std::string chimera::runtime::IntegratorEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_integrator;
}

bool chimera::runtime::IntegratorEntryPoint::checkModule(chimera::Module const * const module) const
{
    const chimera::simulation::IntegratorModule* ireg = dynamic_cast<const chimera::simulation::IntegratorModule*>(module);
    return ireg != nullptr;
}

void chimera::runtime::IntegratorEntryPoint::init()
{
    return;
}

void const * const chimera::runtime::IntegratorEntryPoint::getModuleLoadParams() const
{
    return (void*)_initializer;
}
