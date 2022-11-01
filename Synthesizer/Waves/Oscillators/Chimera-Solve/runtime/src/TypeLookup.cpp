#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
//#include <utility>
//#include <complex>
//#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "RuntimeNames.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "TypeLookup.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "lua/lua_math.hpp"
#include "lua/types/lua_complex.hpp"
#include "lua/types/lua_vector.hpp"
#include "lua/types/lua_matrixrow.hpp"
#include "lua/types/lua_matrix.hpp"
#include "lua/types/lua_Simulation.hpp"
#include "lua/lua_global.hpp"
#include "types/lua_basetypes.hpp"
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"

chimera::runtime::TypeLookup::TypeLookup(ParameterTypeSystem* typeSys)
{
    _typeSys = typeSys;
    _typeSys->addListener(this);
    _lookup = new std::unordered_map<size_t, std::unordered_map<size_t, size_t> >();
}

chimera::runtime::TypeLookup::~TypeLookup()
{
    delete _lookup;
}

size_t chimera::runtime::TypeLookup::findType(size_t baseType, size_t innerType) const
{
    auto baseExists = _lookup->find(baseType);
    if(baseExists != _lookup->end())
    {
        auto typeExists = baseExists->second.find(innerType);
        if(typeExists != baseExists->second.end())
        {
            return typeExists->second;
        }
    }
    return 0;
}

void chimera::runtime::TypeLookup::addTypeLookup(size_t baseType) const
{
    auto baseExists = _lookup->find(baseType);
    if(baseExists == _lookup->end())
    {
        (*_lookup)[baseType] = std::unordered_map<size_t, size_t>();
        //_lookup->insert (std::make_pair<size_t, std::unordered_map<size_t, size_t> >(baseType, std::unordered_map<size_t, size_t>()));
    }
}

void chimera::runtime::TypeLookup::notifyItemAdded(StateSynchrony* sender, void* item, void const * const data)
{
    if(sender == _typeSys)
    {
        ParameterType* p = (ParameterType*)item;
        auto baseExists = _lookup->find(p->getBase());
        if(baseExists != _lookup->end() && p->getTag() != 0)
        {
            baseExists->second[p->getTag()] = p->getID();
        }
    }
}

void chimera::runtime::TypeLookup::notifyItemRemoved(StateSynchrony* sender, void* item, void const * const data)
{
    if(sender == _typeSys)
    {
        ParameterType* p = (ParameterType*)item;
        auto baseExists = _lookup->find(p->getBase());
        if(baseExists != _lookup->end() && p->getTag() != 0)
        {
            baseExists->second.erase(p->getTag());
        }
    }
}

const std::string chimera::runtime::TypeLookup::getGUID() const
{
    return "TypeLookup";
}
