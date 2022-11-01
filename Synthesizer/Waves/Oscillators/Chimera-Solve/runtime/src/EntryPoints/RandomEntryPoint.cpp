#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <random>
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
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPoint.hpp"
#include "EntryPointBase/AbstractRandomGenerator.hpp"
#include "EntryPointBase/AbstractRandom.hpp"
#include "EntryPointBase/RandomModule.hpp"
#include "EntryPoints/RandomEntryPoint.hpp"

chimera::runtime::RandomEntryPoint::RandomEntryPoint()
{
    //ctor
}

chimera::runtime::RandomEntryPoint::~RandomEntryPoint()
{
    //dtor
}

const std::string chimera::runtime::RandomEntryPoint::getGUID() const
{
    return chimera::simulation::Naming::EntryPoint_random;
}

bool chimera::runtime::RandomEntryPoint::checkModule(chimera::Module const * const module) const
{
    const chimera::simulation::RandomModule* m = dynamic_cast<const chimera::simulation::RandomModule*>(module);
    return m != nullptr;
}
