#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "def.hpp"
#include "StateSynchrony.hpp"
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

chimera::ChimeraContext::ChimeraContext(ChimeraSystem* chimeraSystem, StateSynchrony* referrer):
    _chimeraSystem(chimeraSystem),
    _referrer(referrer)
{
    //ctor
}

chimera::ChimeraContext::~ChimeraContext()
{
    //dtor
}

// alle funktionen aus ChimeraSystem, ParameterType, Logging, Types... die nicht lua sind...
// ein bisschen besser sortieren nach visibility
// aus Zugriff auf ChimeraSystem soll Lua und vieles andere möglich sein
// aus ChimeraContext soll nur "sichere" Methoden bekommen
// überprüfen, ob LuaState richtig ist für Threads


void chimera::ChimeraContext::addEntryPoint(const std::string& name, EntryPoint* const ep) const
{
    _chimeraSystem->_epSys->addEntryPoint(name, ep);
}

void chimera::ChimeraContext::removeEntryPoint(const std::string& name) const
{
    _chimeraSystem->_epSys->removeEntryPoint(name);
}

chimera::EntryPoint* const chimera::ChimeraContext::getEntryPoint(const std::string& name) const
{
    return _chimeraSystem->_epSys->getEntryPoint(name);
}

const std::string chimera::ChimeraContext::findEntryPoint(chimera::EntryPoint const * const entrypoint) const
{
    return _chimeraSystem->_epSys->findEntryPoint(entrypoint);
}

size_t chimera::ChimeraContext::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag) const
{
    return _chimeraSystem->getTypeSystem()->registerParameter(name, this, pdef, base, tag);
}

size_t chimera::ChimeraContext::registerParameter(const std::string& name, const struct T_ParameterDef& pdef) const
{
    return _chimeraSystem->getTypeSystem()->registerParameter(name, this, pdef);
}

size_t chimera::ChimeraContext::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& attributes) const
{
    return _chimeraSystem->getTypeSystem()->registerParameter(name, this, pdef, base, tag, attributes);
}

size_t chimera::ChimeraContext::registerParameter(const std::string& name, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& attributes) const
{
    return _chimeraSystem->getTypeSystem()->registerParameter(name, this, pdef, attributes);
}

chimera::ParameterValue chimera::ChimeraContext::createValue(size_t type, void* value) const
{
    return ParameterValue(_chimeraSystem, type, value);
}

chimera::LuaFunctionWrapper chimera::ChimeraContext::createFunction(chimera::fn_luafnwrapper fn) const
{
    return LuaFunctionWrapper(_chimeraSystem, fn);
}

void chimera::ChimeraContext::addDependency(void* item, void* dependency) const
{
    _chimeraSystem->_typeSys->addDependency(item, dependency);
}

void chimera::ChimeraContext::releaseDependency(void* item, void* dependency) const
{
    _chimeraSystem->_typeSys->releaseDependency(item, dependency);
}

void chimera::ChimeraContext::removeDependencyItem(void* item) const
{
    _chimeraSystem->_typeSys->removeDependencyItem(item);
}

size_t chimera::ChimeraContext::getParameterID(const std::string& name) const
{
    return _chimeraSystem->_typeSys->getParameterID(name);
}

const std::string chimera::ChimeraContext::getParameterName(const size_t& type) const
{
    return _chimeraSystem->_typeSys->getParameterName(type);
}

const bool chimera::ChimeraContext::exists(const size_t& type) const
{
    return _chimeraSystem->_typeSys->exists(type);
}

size_t chimera::ChimeraContext::getParameterBase(const size_t& type) const
{
    return _chimeraSystem->_typeSys->getParameterBase(type);
}

size_t chimera::ChimeraContext::getParameterTag(const size_t& type) const
{
    return _chimeraSystem->_typeSys->getParameterTag(type);
}

chimera::ParameterValue chimera::ChimeraContext::cast(size_t toType, const ParameterValue& param) const
{
    return _chimeraSystem->_typeSys->cast(toType, param);
}

bool chimera::ChimeraContext::isReferenced(void* value) const
{
    return _chimeraSystem->_typeSys->isReferenced(value);
}
