#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <vector>
#include <list>

#include "def.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractEventManager.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/ObserverModule.hpp"

static chimera::vec_t_LuaItem instance_function_on(chimera::EntryPoint const * const entrypoint, chimera::Module const * const module, void* instance, const chimera::vec_t_LuaItem& params)
{
    chimera::simulation::AbstractEventManager* evm = (chimera::simulation::AbstractEventManager*)instance;
    if(params.size() == 2)
    {
        if (params[0].getType() == chimera::systemtypes::PID_STRING) {
            if(params[1].getAttribute(chimera::simulation::Naming::Attribute_MultiObserver))
            {
                chimera::simulation::IEventListenerProvider* provider = (chimera::simulation::IEventListenerProvider*)params[1].getValue();
                evm->addListener(params[0], provider);
            } else if(params[1].getAttribute(chimera::simulation::Naming::Attribute_Observer))
            {
                chimera::simulation::IEventListener* listener = (chimera::simulation::IEventListener*)params[1].getValue();
                evm->addListener(params[0], listener);
            }
        }
    }

    return chimera::vec_t_LuaItem();
}

static chimera::vec_t_LuaItem instance_function_observe(chimera::EntryPoint const * const entrypoint, chimera::Module const * const, void* instance, const chimera::vec_t_LuaItem& params)
{
    if (params.size() > 0) {
        chimera::simulation::AbstractEventManager* evm = (chimera::simulation::AbstractEventManager*)instance;
        chimera::simulation::NotificationManager* nm;

        if(params[0].getAttribute(chimera::simulation::Naming::Attribute_Observable))
        {
            nm = (chimera::simulation::NotificationManager*)params[0].getValue();
            evm->assignObservation(nm);
        }
    }

    return chimera::vec_t_LuaItem();
}

chimera::simulation::ObserverModule::ObserverModule()
{
    registerMethod("on", instance_function_on);
    registerMethod("observe", instance_function_observe);
}

chimera::simulation::ObserverModule::~ObserverModule()
{

}

void* chimera::simulation::ObserverModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getEventInstance(parameters);
}

size_t chimera::simulation::ObserverModule::getAttribute(const std::string& attr) const
{
    if (attr == chimera::simulation::Naming::Attribute_Observable ||
        attr == chimera::simulation::Naming::Attribute_MultiObserver)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
