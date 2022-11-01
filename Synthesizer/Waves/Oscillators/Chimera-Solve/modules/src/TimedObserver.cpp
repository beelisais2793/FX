#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <limits>
#include <complex>

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
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "EntryPointBase/AbstractEventManager.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/StateEventListener.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractTemporalIntegrator.hpp"
#include "simulation/AbstractSimulation.hpp"
#include "simulation/TemporalStateEventProvider.hpp"
#include "EntryPointBase/ObserverModule.hpp"
#include "TimedObserver.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new TimedObserverModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (TimedObserverModule*)module;
    }
}

TimedObserverModule::TimedObserverModule()
{

}

TimedObserverModule::~TimedObserverModule()
{

}

const std::string TimedObserverModule::getGUID() const
{
    return "Timed Observer";
}

void TimedObserverModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void* instance) const
{
    delete ((TimedObserver*)instance);
}

chimera::simulation::AbstractEventManager* TimedObserverModule::getEventInstance(chimera::vec_t_LuaItem& parameters) const
{
    bool inNM = false;
    chimera::simulation::NotificationManager* nm;
    double start = 0.0;
    double step = 1.0;
    double end = std::numeric_limits<double>::infinity();

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "observe" && p.second.getAttribute(chimera::simulation::Naming::Attribute_Observable))
            {
                inNM = true;
                nm = (chimera::simulation::NotificationManager*)p.second.getValue();
            }
            if(p.first == "start" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                start = p.second;
            }
            if(p.first == "step" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                if(step <= 0)
                {
                    return nullptr;
                }
                step = p.second;
            }
            if(p.first == "end" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                end = p.second;
            }
        }
        if (inNM) {
            return new TimedObserver(nm, start, step, end);
        } else {
            return nullptr;
        }
    }
    else
    {
        if(parameters.size() > 0 && parameters[0].getAttribute(chimera::simulation::Naming::Attribute_Observable))
        {
            inNM = true;
            nm = (chimera::simulation::NotificationManager*)parameters[0].getValue();
        } else {
            return nullptr;
        }
        if(parameters.size() == 4)
        {
            if(chimera::systemtypes::PID_NUMBER == parameters[1].getType() &&
               chimera::systemtypes::PID_NUMBER == parameters[2].getType() &&
               chimera::systemtypes::PID_NUMBER == parameters[3].getType())
            {
                start = parameters[1];
                step = parameters[2];
                end = parameters[3];
            }
            else
            {
                return nullptr;
            }
        }
        else  if(parameters.size() == 3)
        {
            if(chimera::systemtypes::PID_NUMBER == parameters[1].getType() &&
               chimera::systemtypes::PID_NUMBER == parameters[2].getType())
            {
                start = parameters[1];
                step = parameters[2];
            }
            else
            {
                return nullptr;
            }
        }
        else  if(parameters.size() == 2)
        {
            if(chimera::systemtypes::PID_NUMBER == parameters[1].getType())
            {
                step = parameters[1];
            }
            else
            {
                return nullptr;
            }
        }
    }

    if(step <= 0 || !inNM)
    {
        return nullptr;
    }
    auto result = new TimedObserver(nm, start, step, end);
    getContext()->addDependency(result, nm);
    return result;
}

TimedObserver::TimedObserver(chimera::simulation::NotificationManager* nm, double start, double step, double end):
    _start(start),
    _step(step),
    _end(end),
    _last(start - step)
{
    _elapsedEvent = nullptr;
    assignObservation(nm);
    chimera::simulation::AbstractSimulation* sim = dynamic_cast<chimera::simulation::AbstractSimulation*>(getObservationObject());
    if (sim)
    {
        chimera::simulation::AbstractTemporalIntegrator* integrator = dynamic_cast<chimera::simulation::AbstractTemporalIntegrator*>(sim->getIntegrator());
        if (integrator)
        {
            _elapsedEvent = new chimera::simulation::TemporalStateEventProvider(integrator->getTimeType(), integrator->getStateType());
            registerEvent(TIMED_OBSERVER_EVENT_NAME, _elapsedEvent);
        }
    }
}

TimedObserver::~TimedObserver()
{
    delete _elapsedEvent;
}

chimera::simulation::NotificationManager* TimedObserver::getObservationObject()
{
    chimera::simulation::AbstractEventManager* eventManager = dynamic_cast<chimera::simulation::AbstractEventManager*>(_nm);
    if (eventManager)
    {
        return eventManager->getObservationObject();
    }
    else
    {
        return _nm;
    }
}

void TimedObserver::assignObservation(chimera::simulation::NotificationManager* baseObject)
{
    _nm = baseObject;
    _nm->addListener("step", (IEventListener*)this);
}

chimera::simulation::IEventListener* TimedObserver::provideListener(size_t id, void const * const args)
{
    switch(id){
    case 1: // StateEventListener
        {
            struct T_StateProviderArgs* spArgs = (struct T_StateProviderArgs*)args;
            if(spArgs->time_type == chimera::systemtypes::PID_NUMBER)
            {
                return (chimera::simulation::IEventListener*)this;
            }
        }
    }
    return nullptr;
}

void TimedObserver::notify(chimera::simulation::NotificationManager const * const sender, void const * const args)
{
    const double time = *((double*)((struct chimera::simulation::T_TimeStateArgs const * const)args)->time);
    if(time >= _last + _step)
    {
        while(_last + _step < time)
        {
            _last += _step;
        }
        notifyEvent(TIMED_OBSERVER_EVENT_NAME, args);
    }
}
