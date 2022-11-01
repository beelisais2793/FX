#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <complex>

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ChimeraContext.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "event/StateEventListener.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractTemporalIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "RungeKutta/RungeKuttaSystem.hpp"
#include "RungeKutta/RungeKutta_double_vecDouble.hpp"
#include "RungeKutta/RungeKutta_double_vecComplex.hpp"
#include "RungeKutta/RungeKutta_double_vecvecDouble.hpp"
#include "RungeKutta/RungeKutta_double_vecvecComplex.hpp"
#include "RungeKutta/RungeKuttaModule.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new RungeKuttaModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (RungeKuttaModule*)reg;
    }
}

RungeKuttaModule::RungeKuttaModule()
{
    _init = nullptr;
}

RungeKuttaModule::~RungeKuttaModule()
{

}

void RungeKuttaModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const
{
    delete (chimera::simulation::AbstractIntegrator*)instance;
}

bool RungeKuttaModule::load(chimera::EntryPoint const * const entryPoint, void const * const params)
{
    _init = (chimera::EntryPoint*)params;
    return true;
}

chimera::simulation::AbstractIntegrator* RungeKuttaModule::getSystem(const chimera::ParameterValue& param, const double& dt) const
{
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorComplexMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));
    static const std::string vectorVectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorVectorComplexMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));

    static const size_t vectorRealType = getContext()->getParameterID(vectorRealMetaName);
    static const size_t vectorComplexType = getContext()->getParameterID(vectorComplexMetaName);
    static const size_t vectorVectorRealType = getContext()->getParameterID(vectorVectorRealMetaName);
    static const size_t vectorVectorComplexType = getContext()->getParameterID(vectorVectorComplexMetaName);

    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    static const size_t tagtype = (size_t)(getContext()->getEntryPoint(chimera::simulation::Naming::EntryPoint_dynamics));

    if(getContext()->getParameterBase(param.getType()) == basetype &&
       getContext()->getParameterTag(param.getType()) == tagtype)
    {
        chimera::simulation::AbstractSystemDynamic* dyn = (chimera::simulation::AbstractSystemDynamic*)param.getValue();
        auto features = dyn->getFeatures();

        auto timeType = features.find(chimera::simulation::Naming::Feature_time_type);
        auto stateType = features.find(chimera::simulation::Naming::Feature_state_type);
        if(timeType != features.end() && stateType != features.end()) {
            if(timeType->second == chimera::systemtypes::PID_NUMBER) {
                if(stateType->second == vectorRealType) {
                    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* sys;
                    sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(dyn);
                    if(sys != nullptr) {
                        return (chimera::simulation::AbstractIntegrator*)new RungeKutta_double_vecDouble(getContext(), _init, sys, dt);
                    }
                }
                if(stateType->second == vectorComplexType) {
                    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* sys;
                    sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >*>(dyn);
                    if(sys != nullptr) {
                        return (chimera::simulation::AbstractIntegrator*)new RungeKutta_double_vecComplex(getContext(), _init, sys, dt);
                    }
                }
                if(stateType->second == vectorVectorRealType) {
                    chimera::simulation::TemplateOdeSystem<double, vec_vec_real>* sys;
                    sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, vec_vec_real>*>(dyn);
                    if(sys != nullptr) {
                        return (chimera::simulation::AbstractIntegrator*)new RungeKutta_double_vecvecDouble(getContext(), _init, sys, dt);
                    }
                }
                if(stateType->second == vectorVectorComplexType) {
                    chimera::simulation::TemplateOdeSystem<double, vec_vec_complex>* sys;
                    sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, vec_vec_complex>*>(dyn);
                    if(sys != nullptr) {
                        return (chimera::simulation::AbstractIntegrator*)new RungeKutta_double_vecvecComplex(getContext(), _init, sys, dt);
                    }
                }
            }
        }
    }
    return nullptr;
}

chimera::simulation::AbstractIntegrator* RungeKuttaModule::getIntegratorInstance(chimera::vec_t_LuaItem& parameters) const
{
    double dt = RungeKuttaModule::DEFAULT_DT;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::ParameterValue pvSystem;
        bool inSystem = false;
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "system")
            {
                inSystem = true;
                pvSystem = p.second;
            }
            if(p.first == "h" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                dt = p.second;
                if(dt <= 0.0) dt = RungeKuttaModule::DEFAULT_DT;
            }
        }

        if(!inSystem)
        {
            return nullptr;
        }
        return getSystem(pvSystem, dt);
    }
    else
    {
        if(parameters.size() > 0) {
            if(parameters.size() > 1 && parameters[1].getType() == chimera::systemtypes::PID_NUMBER)
            {
                dt = *((double*)parameters[1].getValue());
                if(dt <= 0.0) dt = RungeKuttaModule::DEFAULT_DT;
            }

            return getSystem(parameters[0], dt);
        }
    }
    return nullptr;
}

bool RungeKuttaModule::checkFeatures(const chimera::map_t_size& features) const
{
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorComplexMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));
    static const std::string vectorVectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorVectorComplexMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));

    static const size_t vectorRealType = getContext()->getParameterID(vectorRealMetaName);
    static const size_t vectorComplexType = getContext()->getParameterID(vectorComplexMetaName);
    static const size_t vectorVectorRealType = getContext()->getParameterID(vectorVectorRealMetaName);
    static const size_t vectorVectorComplexType = getContext()->getParameterID(vectorVectorComplexMetaName);

    auto timeType = features.find(chimera::simulation::Naming::Feature_time_type);
    auto stateType = features.find(chimera::simulation::Naming::Feature_state_type);
    if(timeType != features.end() && stateType != features.end()) {
        if(timeType->second == chimera::systemtypes::PID_NUMBER) {
            if(stateType->second == vectorRealType ||
               stateType->second == vectorComplexType ||
               stateType->second == vectorVectorRealType ||
               stateType->second == vectorVectorComplexType)
            {
                return true;
            }
        }
    }

    return false;
}

const std::string RungeKuttaModule::getGUID() const
{
    return "Runge-Kutta";
}

const std::string RungeKuttaModule::getSystemName() const
{
    return "ode";
}
