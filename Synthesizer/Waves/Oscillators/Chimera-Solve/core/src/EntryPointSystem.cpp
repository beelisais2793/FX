#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "def.hpp"
#include "StateSynchrony.hpp"
#include "ParameterType.hpp"
#include "ParameterValue.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"

chimera::EntryPointSystem::EntryPointSystem(ChimeraSystem* chimeraSystem)
{
    _hooks = new map_t_EntryPoint();
    _context = chimeraSystem->createContext(this);
    stateLoaded(chimeraSystem);
}

chimera::EntryPointSystem::~EntryPointSystem()
{
    map_t_EntryPoint::iterator it;
    while((it = _hooks->begin()) != _hooks->end())
    {
        removeEntryPoint(it->first);
    }
    delete _hooks;
}

void chimera::EntryPointSystem::addEntryPoint(const std::string& name, EntryPoint* ep)
{
    map_t_EntryPoint::const_iterator available = _hooks->find (name);

    if (available == _hooks->end())
    {
        _hooks->insert(std::make_pair(name, ep));
        itemAdded(ep, &name);
        ep->addListener(this);
        ep->loadEntryPoint();
    }
    else
    {
        //std::string s = "entry point `";
        //s += ep->getName();
        //s += "` already set";
        //LoggingSystem::Error(s);
#warning LOG (kekstoaster#1#): entry point `type` already set
    }
}

void chimera::EntryPointSystem::removeEntryPoint(const std::string& name)
{
    map_t_EntryPoint::const_iterator available = _hooks->find (name);

    if (available != _hooks->end())
    {
        available->second->unloadEntryPoint();

        map_t_Module::iterator it = available->second->_modules->begin();
        while((it = available->second->_modules->begin()) != available->second->_modules->end())
        {
            available->second->removeModule(it->first);
        }

        _hooks->erase(name);
        itemRemoved(available->second, &name);
        available->second->removeListener(this);
    }
}

const std::string chimera::EntryPointSystem::findEntryPoint(EntryPoint const * const entrypoint) const
{
    for(auto it = _hooks->begin(); it != _hooks->end(); it++)
    {
        if(it->second == entrypoint)
        {
            return it->first;
        }
    }
    return "";
}

chimera::EntryPoint* const chimera::EntryPointSystem::getEntryPoint(const std::string& name)
{
    map_t_EntryPoint::const_iterator available = _hooks->find (name);

    if (available != _hooks->end())
    {
        return available->second;
    }
    return nullptr;
}

chimera::EntryPoint::iterator chimera::EntryPointSystem::begin()
{
    return EntryPoint::iterator(_hooks->begin());
}

chimera::EntryPoint::iterator chimera::EntryPointSystem::end()
{
    return EntryPoint::iterator(_hooks->end());
}

void chimera::EntryPointSystem::notifyDelete(StateSynchrony* sender)
{
    for(auto it = _hooks->begin(); it != _hooks->end(); it++)
    {
        if(it->second == sender)
        {
            removeEntryPoint(it->first);
            break;
        }
    }
}

const std::string chimera::EntryPointSystem::getGUID() const
{
    return "EntryPointSystem";
}
