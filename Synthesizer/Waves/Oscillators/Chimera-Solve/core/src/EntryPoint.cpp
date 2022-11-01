#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "def.hpp"
#include "ChimeraContext.hpp"
#include "StateSynchrony.hpp"
#include "ParameterValue.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "ParameterType.hpp"
#include "ChimeraSystem.hpp"

chimera::EntryPoint::EntryPoint()
{
    _modules = new map_t_Module();
}

chimera::EntryPoint::~EntryPoint()
{
    // unload entry point and all modules
    unloadEntryPoint();
    delete _modules;
}

bool chimera::EntryPoint::addModule(const std::string& name, Module * const module)
{
    if(name == "")
    {
        return false;
    }
    // virtual funtion to validate module
    bool result = checkModule(module);
    if(result)
    {
        _modules->insert(std::make_pair(name, module));
        itemAdded(module, &name);
        module->addListener(this);
    }
    return result;
}

bool chimera::EntryPoint::addLoadModule(const std::string& name, Module * const module)
{
    bool result = addModule(name, module);
    if(result && isLoaded((StateSynchrony*)this))
    {
        module->loadModule(this, name, getModuleLoadParams());
    }
    return result;
}

void chimera::EntryPoint::removeModule(const std::string& name)
{
    map_t_Module::iterator available = _modules->find (name);
    if(available != _modules->end())
    {
        available->second->unloadModule(this);
        available->second->removeListener(this);
        itemRemoved(available->second, &name);
        _modules->erase(name);
    }
}

const chimera::Module* chimera::EntryPoint::getModule(const std::string& name) const
{
    map_t_Module::const_iterator available = _modules->find(name);
    if (available != _modules->end())
    {
        return available->second;
    }
    else
    {
        return nullptr;
    }
}

const std::string chimera::EntryPoint::findModule(Module const * const module) const
{
    for(auto it = _modules->begin(); it != _modules->end(); it++)
    {
        if(it->second == module)
        {
            return it->first;
        }
    }
    return "";
}

bool chimera::EntryPoint::checkModule(chimera::Module const * const module) const
{
    // virtual - do nothing
    return true;
}

void chimera::EntryPoint::init()
{
    // virtual - do nothing
    return;
}

void chimera::EntryPoint::loadModule(const std::string& name)
{
    if(_context && isLoaded((StateSynchrony*)_context->_chimeraSystem->_epSys))
    {
        map_t_Module::const_iterator available = _modules->find(name);
        if (available != _modules->end())
        {
            available->second->loadModule(this, name, getModuleLoadParams());
        }
    }
}

void chimera::EntryPoint::unloadModule(const std::string& name)
{
    map_t_Module::const_iterator available = _modules->find(name);
    if (available != _modules->end())
    {
        available->second->unloadModule(this);
    }
}

void chimera::EntryPoint::load()
{
    // virtual - do nothing
}

void chimera::EntryPoint::unload()
{
    // virtual - do nothing
}

void const * const chimera::EntryPoint::getModuleLoadParams() const
{
    // virtual - do nothing
    return nullptr;
}

void chimera::EntryPoint::loadEntryPoint()
{
    if(_context && !isLoaded((StateSynchrony*)_context->_chimeraSystem->_epSys))
    {
        stateLoaded((StateSynchrony*)_context->_chimeraSystem->_epSys);
        load();
    }
}

void chimera::EntryPoint::unloadEntryPoint()
{
    if(_context && isLoaded((StateSynchrony*)_context->_chimeraSystem->_epSys))
    {
        for(auto it = _modules->begin(); it != _modules->end(); it++)
        {
            it->second->unloadModule(this);
        }

        unload();
        stateUnloaded((StateSynchrony*)_context->_chimeraSystem->_epSys);
    }
}

int chimera::EntryPoint::size() const
{
    return _modules->size();
}

chimera::Module::iterator chimera::EntryPoint::begin()
{
    return chimera::Module::iterator(_modules->begin());
}

chimera::Module::iterator chimera::EntryPoint::end()
{
    return chimera::Module::iterator(_modules->end());
}

std::ostream& operator<< (std::ostream &out, const chimera::EntryPoint &d)
{
    out << d.getGUID();
    return out;
}

void chimera::EntryPoint::notifyDelete(StateSynchrony* sender)
{
    for(auto it = _modules->begin(); it != _modules->end(); it++)
    {
        if(it->second == sender)
        {
            removeModule(it->first);
            break;
        }
    }
}

chimera::EntryPoint::iterator::iterator(map_t_EntryPoint::iterator it):
    _it(it)
{

}

chimera::EntryPoint::iterator::~iterator()
{

}

chimera::EntryPoint* chimera::EntryPoint::iterator::operator*()
{
    return _it->second;
}

chimera::EntryPoint::iterator chimera::EntryPoint::iterator::operator++()
{
    return EntryPoint::iterator(++_it);
}

chimera::EntryPoint::iterator chimera::EntryPoint::iterator::operator++(int)
{
    return EntryPoint::iterator(_it++);
}

bool chimera::EntryPoint::iterator::operator==(const chimera::EntryPoint::iterator& other)
{
    return _it == other._it;
}

bool chimera::EntryPoint::iterator::operator!=(const chimera::EntryPoint::iterator& other)
{
    return _it != other._it;
}
