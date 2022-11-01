#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>

//#include "lua.hpp"
#include "def.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/StateEventListener.hpp"
#include "event/Observer.hpp"
//#include "interfaces/IEventListener.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "simulation/TemporalStateEventProvider.hpp"
//#include "event/DefaultEventProvider.hpp"
//#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractTemporalIntegrator.hpp"
#include "EntryPointBase/TemplateTemporalIntegrator.hpp"


chimera::simulation::TemporalStateEventProvider::TemporalStateEventProvider(size_t timeType, size_t stateType):
    chimera::simulation::AbstractEventProvider(1)
{
    _providerArgs.time_type = timeType;
    _providerArgs.state_type = stateType;
    //_args = new struct chimera::simulation::T_TimeStateArgs();
}

chimera::simulation::TemporalStateEventProvider::~TemporalStateEventProvider()
{
    //delete _args;
}

chimera::simulation::IEventListener* chimera::simulation::TemporalStateEventProvider::provideListener(chimera::simulation::IEventListenerProvider* provider) const
{
    //T_StateProviderArgs args = { .time_type = _integrator->getTimeType(), .state_type = _integrator->getStateType() };
    return provider->provideListener(1, &_providerArgs);
}

/*
bool chimera::simulation::TemporalStateEventProvider::triggerCondition(chimera::simulation::NotificationManager const * const sender)
{
    return true;
}

void* chimera::simulation::TemporalStateEventProvider::getEventArgs(chimera::simulation::NotificationManager const * const sender)
{
    _args->time = _integrator->currentState();
    _args->state = _integrator->currentTime();
    return _args;
}
*/
