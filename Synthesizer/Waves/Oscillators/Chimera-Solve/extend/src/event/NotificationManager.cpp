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

chimera::simulation::NotificationManager::NotificationManager()
{
    _providerMap = new std::unordered_map<std::string, AbstractEventProvider*>();
}

chimera::simulation::NotificationManager::~NotificationManager()
{
    delete _providerMap;
}

void chimera::simulation::NotificationManager::registerEvent(const std::string& name, AbstractEventProvider* eventProvider)
{
    if(!hasEvent(name))
    {
        _providerMap->insert(std::make_pair(name, eventProvider));
    }
}

void chimera::simulation::NotificationManager::notifyEvent(const std::string& name, void const * const args)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        available->second->getObserver()-> trigger(this, args);
    }
}

bool chimera::simulation::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        available->second->getObserver()->addListener(listener);
        return true;
    }
    return false;
}

chimera::simulation::IEventListener const * const chimera::simulation::NotificationManager::addListener(const std::string& name, chimera::simulation::IEventListenerProvider* provider)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        chimera::simulation::IEventListener* listener;
        listener = available->second->provideListener(provider);
        if (listener)
        {
            available->second->getObserver()->addListener(listener);
            return listener;
        }
    }

    return nullptr;
}

bool chimera::simulation::NotificationManager::removeListener(const std::string& name, chimera::simulation::IEventListener* listener)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        return available->second->getObserver()->removeListener(listener);
    }
    return false;
}

bool chimera::simulation::NotificationManager::hasEvent(const std::string& name) const
{
    return _providerMap->count(name);
}

size_t chimera::simulation::NotificationManager::getEventType(const std::string& name) const
{
    auto available = _providerMap->find(name);
    if(available != _providerMap->end())
    {
        available->second->getType();
    }
    else
    {
        return 0;
    }
}

chimera::simulation::Observer* chimera::simulation::NotificationManager::getObserver(const std::string& name)
{
    auto available = _providerMap->find (name);
    if(available != _providerMap->end())
    {
        return available->second->getObserver();
    }
    return nullptr;
}
