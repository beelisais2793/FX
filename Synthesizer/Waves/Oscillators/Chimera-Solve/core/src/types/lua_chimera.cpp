#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "StateSynchrony.hpp"
#include "types/lua_basetypes.hpp"
#include "types/lua_static.hpp"
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
#include "types/lua_chimera.hpp"

void chimera::ChimeraSystem::init()
{
    lua_State* L = _baseState = _currentState = luaL_newstate();

    //lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_INSTANCE);
    //lua_pushlightuserdata(L, (void *)this);
    //lua_settable(L, LUA_REGISTRYINDEX);
    void** chimera = (void**)lua_getextraspace(L);
    *chimera = (void*)this;

    // create table to store all system functions
    // and keywords that cannot be overridden
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_newtable(L);
    lua_newtable(L);
    lua_pushstring(L, "nil");
    lua_setfield(L, -2, "__name");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "__type");
    lua_rawseti(L, -2, 0);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
    lua_newtable(L);
    lua_newtable(L);
    lua_pushliteral(L, "__mode");
    lua_pushliteral(L, "v");
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushglobaltable(L);
    // meta table for global with __index and __newindex
    lua_newtable(L);

    // set system names table as __index, so they cannot be set by script
    lua_pushstring(L, "__index");
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_rawset(L, -3);  // set table as __index in global metatable

    // set __newindex in global metatable
    lua_pushstring(L, "__newindex");
    lua_pushcfunction (L, lua_global_scope_newindex);
    lua_rawset(L, -3);

    lua_setmetatable(L, -2);  // set meta table for global table
    lua_pop(L, 1);

    for(auto it = _typeSys->_typeList->begin(); it != _typeSys->_typeList->end(); it++)
    {
        if((*it)->getID() > chimera::systemtypes::PID_LIBRARAY)
        {
            pushType(*it); // create Meta for Parameter
        }
    }
}

void chimera::ChimeraSystem::pushType(ParameterType const * const p)
{
    lua_State* L = _baseState;
    // Create new meta table for new type
    if(p != nullptr)
    {
        int type = p->getID();
        std::string s(p->getName());
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_gettable(L, LUA_REGISTRYINDEX);
        // new meta table
        lua_newtable(L);
        // pops table above and returns / pushes a single table onto stack
        // thus chainLoad will leave stack count unchanged
        // executes all init functions of type and base types
        chainLoad(p, p->getID());
        lua_pushcfunction(L, chimera::lua_UserData_gc);
        lua_setfield(L, -2, "__gc");
        lua_pushstring(L, p->getName().c_str());
        lua_setfield(L, -2, "__name");
        lua_pushinteger(L, p->getID());
        lua_setfield(L, -2, "__type");
        lua_rawseti(L, -2, p->getID());
        lua_pop(L, 1);
    }
}

void chimera::ChimeraSystem::popType(const ParameterType& type)
{
    lua_State* L = _baseState;
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_gettable(L, LUA_REGISTRYINDEX);
    if(lua_rawgeti(L, -1, type.getID()))
    {
        // empty meta table
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_rawset(L, -3);
            lua_pushnil(L);
        }

        lua_pushinteger(L, chimera::systemtypes::PID_NIL);
        lua_setfield(L, -2, "__type");
    }
    lua_pushnil(L);
    lua_rawseti(L, -3, type.getID());
    lua_pop(L, 2);
}

lua_State* chimera::ChimeraSystem::getLuaState()
{
    return _baseState;
}

lua_State* chimera::ChimeraSystem::getCurrentState()
{
    return _currentState;
}

void chimera::ChimeraSystem::setCurrentState(lua_State* next)
{
    _currentState = next;
}

void chimera::ChimeraSystem::chainLoad(ParameterType const * const p, const int& id) const
{
    lua_State* L = _baseState;
    // TODO lua_gettop speichern
    int base = p->getBase();
    if(base)
    {
        ParameterType const * const b = _typeSys->getParameter(base);
        chainLoad(b, id);
    }
    p->initialize(L, id);
    // if a initialize call leaves any object but a table
    // replace it with table again, so it can be used as a
    // meta table

    // TODO teste, ob gettop gleich oben, dann nicht popen, sondern nur tabelle pushen
    if(lua_type(L, -1) != LUA_TTABLE)
    {
        lua_pop(L, 1);
        lua_newtable(L);
    }
}

void chimera::ChimeraSystem::pushLibrary(const std::string& name, lua_CFunction createMetaTable)
{
    lua_State* L = _baseState;
    lua_pushcfunction(L, createMetaTable);
    lua_call(L, 0, 1);
    if(lua_type(L, -1) == LUA_TTABLE)
    {
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushstring(L, name.c_str());
        void** lib = (void**)lua_newuserdata(L, sizeof(void*));
        *lib = nullptr;
        lua_pushvalue(L, -4);

        lua_pushcfunction (L, chimera::lua_ignore_newindex);
        lua_setfield(L, -2, "__newindex");
        lua_pushstring(L, chimera::typenames::TYPE_LIBRARY);
        lua_setfield(L, -2, "__name");
        lua_pushinteger(L, chimera::systemtypes::PID_LIBRARAY);
        lua_setfield(L, -2, "__type");
        lua_setmetatable(L, -2);
        lua_rawset(L, -3);
        lua_pop(L, 2);
    } else {
        lua_pop(L, 1);
    }
}
