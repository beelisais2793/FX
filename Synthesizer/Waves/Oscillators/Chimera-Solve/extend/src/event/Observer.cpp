#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::Observer::Observer():
    Observer(0)
{

}

chimera::simulation::Observer::Observer(size_t eventType):
    _eventType(eventType)
{
    _listeners = new std::unordered_set<IEventListener*>;
}

chimera::simulation::Observer::~Observer()
{
    delete _listeners;
}


void chimera::simulation::Observer::trigger(NotificationManager* sender, void const * const args)
{
    if(_listeners->size())
    {
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            (*it)->notify(sender, args);
        }
    }
}

bool chimera::simulation::Observer::addListener(chimera::simulation::IEventListener* listener)
{
    if(!_listeners->count(listener)) {
        _listeners->insert(listener);
        return true;
    }
    return false;
}

bool chimera::simulation::Observer::removeListener(chimera::simulation::IEventListener* listener)
{
    if(_listeners->count(listener)) {
        _listeners->erase(listener);
        return true;
    }
    return false;
}

bool chimera::simulation::Observer::empty() const
{
    return _listeners->size() == 0;
}

size_t chimera::simulation::Observer::getType() const
{
    return _eventType;
}
