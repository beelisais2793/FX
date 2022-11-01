#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
//#include <chrono>
#include <random>

#include "def.hpp"
#include "Naming.hpp"
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
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ChimeraContext.hpp"
#include "EntryPointBase/AbstractRandomGenerator.hpp"
#include "EntryPointBase/AbstractRandom.hpp"
#include "EntryPointBase/RandomModule.hpp"
#include "UniformRandom.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new UniformRandomModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (UniformRandomModule*)module;
    }
}

UniformRandomModule::UniformRandomModule()
{
    //ctor
}

UniformRandomModule::~UniformRandomModule()
{
    //dtor
}

const std::string UniformRandomModule::getGUID() const
{
    return "Uniform Distribution";
}

void UniformRandomModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void* instance) const
{
    delete ((UniformDistribution*)instance);
}

const size_t UniformRandomModule::getType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

chimera::simulation::AbstractRandom* UniformRandomModule::getRandomInstance(chimera::simulation::AbstractRandomGenerator* generator, chimera::vec_t_LuaItem& parameters) const
{
    double min = 0.0;
    double max;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inMax;
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "min" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                min = p.second;
            }
            if(p.first == "max" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inMax = true;
                max = p.second;
            }
        }
        if(inMax && min < max)
        {
            return new UniformDistribution(generator, getContext(), min, max);
        }
    }
    else
    {
        if((parameters.size() == 1 || parameters.size() == 2) && chimera::systemtypes::PID_NUMBER == parameters[0].getType())
        {
            if(parameters.size() == 2)
            {
                if(chimera::systemtypes::PID_NUMBER == parameters[1].getType())
                {
                    min = parameters[0];
                    max = parameters[1];
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                max = parameters[0];
                if(max < 0)
                {
                    min = max;
                    max = 0;
                }
            }
            if(min < max)
            {
                return new UniformDistribution(generator, getContext(), min, max);
            }
        }
    }
    return nullptr;
}

UniformDistribution::UniformDistribution(chimera::simulation::AbstractRandomGenerator* generator, chimera::ChimeraContext* context, double a, double b)
{
    _uniRealDist = new std::uniform_real_distribution<double>(a, b);
    context->addDependency(this, generator);
    _generator = generator;
}

UniformDistribution::~UniformDistribution()
{
    delete _uniRealDist;
}

const size_t UniformDistribution::getType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

void* UniformDistribution::getNext()
{
    return new double((*_uniRealDist)(*_generator));
}
