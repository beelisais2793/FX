#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "def.hpp"
#include "StateSynchrony.hpp"
#include "ParameterValue.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"

chimera::Module::Module()
{
    _instanceMethods = new std::unordered_map<std::string, chimera::fn_instancefnwrapper>();
}

chimera::Module::~Module()
{
    delete _instanceMethods;
    // copy all context pointers, because the original list gets altered during unloadModule
    std::unordered_set<StateSynchrony*> loadStates = std::unordered_set<StateSynchrony*>(*_loadStates);
    for(auto it = loadStates.begin(); it != loadStates.end(); it++)
    {
        unloadModule(((EntryPoint*)*it));
    }
}

void chimera::Module::registerMethod(const std::string& name, chimera::fn_instancefnwrapper method)
{
    (*_instanceMethods)[name] = method;
}

chimera::fn_instancefnwrapper chimera::Module::getMethod(const std::string& name) const
{
    auto available = _instanceMethods->find(name);
    if(available != _instanceMethods->end())
    {
        return available->second;
    }
    else
    {
        return nullptr;
    }
}

void chimera::Module::init(EntryPoint const * const entryPoint)
{
    // virtual - do nothing
    return;
}

bool chimera::Module::load(EntryPoint const * const entryPoint, void const * const params)
{
    // virtual - do nothing
    return true;
}

void chimera::Module::unload(EntryPoint const * const entryPoint)
{
    // virtual - do nothing
    return;
}

void chimera::Module::loadModule(EntryPoint const * const entryPoint, const std::string& name, void const * const params)
{
    // only add new context
    if(!isLoaded((StateSynchrony*)entryPoint))
    {
        // if virtual function returns falls, cancel the loading process
        if(load(entryPoint, params)) {
            stateLoaded((StateSynchrony*)entryPoint, &name);
        }
    }
}

void chimera::Module::unloadModule(EntryPoint const * const entryPoint)
{
    // if context was loaded
    if(isLoaded((StateSynchrony*)entryPoint))
    {
        unload(entryPoint);
        stateUnloaded((StateSynchrony*)entryPoint);
    }
}

size_t chimera::Module::getAttribute(const std::string& attr) const
{
    // virtual - always return 0: Attribute not set.
    return 0;
}

std::ostream& operator<< (std::ostream &out, const chimera::Module &d)
{
    out << d.getGUID();
    return out;
}

chimera::Module::iterator::iterator (map_t_Module::iterator it):
    _it(it)
{

}

chimera::Module::iterator::~iterator()
{

}

chimera::Module* chimera::Module::iterator::operator*()
{
    return _it->second;
}

chimera::Module::iterator chimera::Module::iterator::operator++()
{
    chimera::Module::iterator it(++_it);
    return it;
}

chimera::Module::iterator chimera::Module::iterator::operator++(int)
{
    chimera::Module::iterator it(_it++);
    return it;
}

bool chimera::Module::iterator::operator==(const chimera::Module::iterator& other)
{
    return _it == other._it;
}

bool chimera::Module::iterator::operator!=(const chimera::Module::iterator& other)
{
    return _it != other._it;
}
