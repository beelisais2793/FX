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
#include "EntryPoints/UtilEntryPoint.hpp"

chimera::runtime::UtilEntryPoint::UtilEntryPoint()
{

}

chimera::runtime::UtilEntryPoint::~UtilEntryPoint()
{
    //dtor
}

const std::string chimera::runtime::UtilEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_util;
}
