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
#include "CudaOscillator/CudaOscillator.hpp"

//#ifdef USENVCC

#include "CudaOscillator/CudaOscillator.hu"

// nvcc --shared --compiler-options '-fPIC -std=c++11' -D USENVCC -Ilua-5.3.3/src -Imodules/include -Iextend/include -Icore/include  -c modules/src/CudaOscillator.cpp -o obj/Release/module/CudaOscillator.o
// nvcc --shared -o bin/Release/modules/CudaOscillator.mod obj/Release/module/CudaOscillator.o  -lboost_system

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new CudaOscillatorModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (CudaOscillatorModule*)module;
    }
}

CudaOscillatorModule::CudaOscillatorModule()
{
    main_CudaOscillator();
    //setValue("keks", {ParameterTypeSystem::getParameterID(Naming::Type_real), (void*)new double(17.349)});
    //registerMethod("test", rk_test);
}

void* CudaOscillatorModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    bool inA = false;
    bool inB = false;
    double a = 0.0;
    double b = 0.0;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "a" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inA = true;
                a = p.second;
            }
            if(p.first == "b" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inB = true;
                b = p.second;
            }
        }
    }
    else
    {
        if(parameters.size() > 0 && chimera::systemtypes::PID_NUMBER == parameters[0].getType())
        {
            inA = true;
            a = parameters[0];
        }
        if(parameters.size() > 1 && chimera::systemtypes::PID_NUMBER == parameters[1].getType())
        {
            inB = true;
            b = parameters[1];
        }
    }

    if(!(inA && inB))
    {
        return nullptr;
    }

    return new CudaOscillator(getContext(), a, b);
}

const std::string CudaOscillatorModule::getGUID() const
{
    return "Cuda Oscillator";
}

void CudaOscillatorModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void* instance) const
{
    delete ((CudaOscillator*)instance);
}

CudaOscillator::CudaOscillator(chimera::ChimeraContext* context, double a, double b):
    _a(a),
    _b(b),
    _context(context)
{

}

void CudaOscillator::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t)
{
    dxdt(0) = -x(1) * _a;
    dxdt(1) = x(0) * _b;
}

std::unordered_map<std::string, size_t> CudaOscillator::getFeatures() const
{
    std::unordered_map<std::string, size_t> features;
    features[chimera::simulation::Naming::Feature_time_type] = chimera::systemtypes::PID_NUMBER;
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    features[chimera::simulation::Naming::Feature_state_type] = _context->getParameterID(vectorRealMetaName);
    features[chimera::simulation::Naming::Feature_size] = 2;
    return features;
}

const std::string CudaOscillator::getSystemName() const
{
    return "ode";
}

CudaOscillator::~CudaOscillator()
{

}

//#endif
