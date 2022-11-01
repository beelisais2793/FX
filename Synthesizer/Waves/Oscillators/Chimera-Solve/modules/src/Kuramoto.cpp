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
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "Kuramoto.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new KuramotoModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (KuramotoModule*)module;
    }
}

KuramotoModule::KuramotoModule()
{
    //setValue("keks", {ParameterTypeSystem::getParameterID(Naming::Type_real), (void*)new double(17.349)});
    //registerMethod("test", rk_test);
}

void* KuramotoModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    bool inOmega = false;
    bool inN = false;
    double omega = 0.0;
    int N = 0;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "omega" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inOmega = true;
                omega = p.second;
            }
            if(p.first == "N" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inN = true;
                N = p.second;
            }
        }
    }
    else
    {
        if(parameters.size() > 0 && chimera::systemtypes::PID_NUMBER == parameters[0].getType())
        {
            inN = true;
            N = parameters[0];
        }
        if(parameters.size() > 1 && chimera::systemtypes::PID_NUMBER == parameters[1].getType())
        {
            inOmega = true;
            omega = parameters[1];
        }
    }

    if(!(inOmega && inN))
    {
        return nullptr;
    }

    return new KuramotoOscillator(getContext(), N, omega);
}

const std::string KuramotoModule::getGUID() const
{
    return "Kuramoto";
}

void KuramotoModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void* instance) const
{
    delete ((KuramotoOscillator*)instance);
}

KuramotoOscillator::KuramotoOscillator(chimera::ChimeraContext* context, int N, double omega):
    _context(context),
    _N(N),
    _omega(omega)
{

}

void KuramotoOscillator::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t)
{
    dxdt(0) = _omega;
    for(int i = 1; i <= _N; i++) {
        dxdt(i) = 0;
    }
}

std::unordered_map<std::string, size_t> KuramotoOscillator::getFeatures() const
{
    std::unordered_map<std::string, size_t> features;
    features[chimera::simulation::Naming::Feature_time_type] = chimera::systemtypes::PID_NUMBER;
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    features[chimera::simulation::Naming::Feature_state_type] = _context->getParameterID(vectorRealMetaName);
    features[chimera::simulation::Naming::Feature_size] = _N + 1;
    return features;
}

const std::string KuramotoOscillator::getSystemName() const
{
    return "ode";
}

KuramotoOscillator::~KuramotoOscillator()
{

}
