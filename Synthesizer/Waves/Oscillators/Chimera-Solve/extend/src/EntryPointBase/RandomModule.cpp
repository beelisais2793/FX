#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <chrono>
#include <random>

#include "def.hpp"
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
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ChimeraContext.hpp"
#include "event/Observer.hpp"
//#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractRandomGenerator.hpp"
#include "EntryPointBase/AbstractRandom.hpp"
#include "EntryPointBase/RandomModule.hpp"

chimera::simulation::RandomModule::RandomModule()
{
    _generator = nullptr;
}

chimera::simulation::RandomModule::~RandomModule()
{
    if(_generator)
    {
        delete _generator;
    }
}

void* chimera::simulation::RandomModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    return (void*)getRandomInstance(_generator, parameters);
}

chimera::vec_t_LuaItem instance_next(chimera::EntryPoint const * const entrypoint, chimera::Module const * const module, void* instance, const chimera::vec_t_LuaItem& params)
{
    chimera::simulation::AbstractRandom* rnd = (chimera::simulation::AbstractRandom*)instance;
    chimera::vec_t_LuaItem result;
    result.push_back(module->getContext()->createValue(rnd->getType(), rnd->getNext()));
    return result;
}

bool chimera::simulation::RandomModule::load(EntryPoint const * const entryPoint, void const * const params)
{
    if(_generator == nullptr)
    {
        registerMethod("next", instance_next);
        _generator = new DefaultRandomGenerator();
        return true;
    }
    return false;
}

chimera::simulation::DefaultRandomGenerator::DefaultRandomGenerator()
{
    _engine = new std::default_random_engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

chimera::simulation::DefaultRandomGenerator::~DefaultRandomGenerator()
{
    delete _engine;
}

unsigned int chimera::simulation::DefaultRandomGenerator::min()
{
    return _engine->min();
}

unsigned int chimera::simulation::DefaultRandomGenerator::max()
{
    return _engine->max();
}

unsigned int chimera::simulation::DefaultRandomGenerator::operator()()
{
    return _engine->operator()();
}
