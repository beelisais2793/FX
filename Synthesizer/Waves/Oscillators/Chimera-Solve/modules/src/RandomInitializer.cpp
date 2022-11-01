#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>

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
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractInitializer.hpp"
#include "EntryPointBase/AbstractRandom.hpp"
#include "EntryPointBase/InitializerModule.hpp"
#include "RandomInitializer.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new RandomInitializerModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (RandomInitializerModule*)module;
    }
}

RandomInitializerModule::RandomInitializerModule()
{

}

RandomInitializerModule::~RandomInitializerModule()
{

}

const std::string RandomInitializerModule::getGUID() const
{
    return "Random Initializer";
}

void RandomInitializerModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const
{
    delete (chimera::simulation::AbstractInitializer*)instance;
}

chimera::simulation::AbstractInitializer* RandomInitializerModule::getInitializerInstance(chimera::vec_t_LuaItem& parameters) const
{
    size_t randomEntryPoint = (size_t)getContext()->getEntryPoint(chimera::simulation::Naming::EntryPoint_random);
    if(randomEntryPoint != 0)
    {
        if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
        {
            chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
            for(auto p : *paramMap)
            {
                if(p.first == "engine" && getContext()->getParameterBase(p.second.getType()) == chimera::systemtypes::PID_INSTANCE && getContext()->getParameterTag(p.second.getType()) == randomEntryPoint)
                {
                    return new RandomInitializer(getContext(), (chimera::simulation::AbstractRandom*)p.second.getValue());
                }
            }
        }
        else if(parameters.size() == 1 && getContext()->getParameterBase(parameters[0].getType()) == chimera::systemtypes::PID_INSTANCE && getContext()->getParameterTag(parameters[0].getType()) == randomEntryPoint)
        {
            return new RandomInitializer(getContext(), (chimera::simulation::AbstractRandom*)parameters[0].getValue());
        }
    }
    return nullptr;
}

RandomInitializer::RandomInitializer(chimera::ChimeraContext* context, chimera::simulation::AbstractRandom* random)
{
    _context = context;
    _random = random;
    _context->addDependency(this, random);
}

RandomInitializer::~RandomInitializer()
{
    _context->removeDependencyItem(this);
}

void RandomInitializer::initialize(chimera::simulation::AbstractSystemDynamic* system, void* state)
{
    static const size_t pid_vecReal = _context->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t pid_vecvecReal = _context->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));

    std::unordered_map<std::string, size_t> features = system->getFeatures();
    auto stateType = features.find(chimera::simulation::Naming::Feature_state_type);
    if(stateType != features.end())
    {
        if(stateType->second == pid_vecReal)
        {
            boost::numeric::ublas::vector<double>* vState = (boost::numeric::ublas::vector<double>*)state;
            double* next;
            for(unsigned int i = 0; i < vState->size(); i ++)
            {
                next = (double*)_random->getNext();
                (*vState)[i] = *next;
                delete next;
            }
        }
        else if(stateType->second == pid_vecvecReal)
        {
            boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >* vState = (boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >*)state;
            double* next;
            for(unsigned int i = 0; i < vState->size(); i++)
            {
                for(unsigned int j = 0; j < (*vState)[i].size(); j++)
                {
                    next = (double*)_random->getNext();
                    (*vState)[i][j] = *next;
                    delete next;
                }
            }
        }
    }
}
