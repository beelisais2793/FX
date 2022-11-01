#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::AbstractEventProvider::AbstractEventProvider()
{
    _observer = new Observer(0);
}

chimera::simulation::AbstractEventProvider::AbstractEventProvider(size_t eventType)
{
    _observer = new Observer(eventType);
}

chimera::simulation::AbstractEventProvider::~AbstractEventProvider()
{
    delete _observer;
}

size_t chimera::simulation::AbstractEventProvider::getType() const
{
    return _observer->getType();
}

chimera::simulation::Observer* chimera::simulation::AbstractEventProvider::getObserver()
{
    return _observer;
}

