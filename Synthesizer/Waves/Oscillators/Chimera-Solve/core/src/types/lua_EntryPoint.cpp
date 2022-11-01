#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "types/lua_static.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "types/lua_EntryPoint.hpp"
#include "ChimeraSystem.hpp"

void chimera::ChimeraSystem::pushEntryPoint(const std::string& name, EntryPoint* ep)
{
    lua_State* L = this->getLuaState();
    int top = lua_gettop(L);
    ep->init();

    // save entrypoint also in references
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    // get system names
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    // if new EntryPoint conflicts with existing name
    lua_pushstring(L, name.c_str());
    if(!lua_rawget(L, -2))
    {
        // pop nil
        lua_pop(L, 1);
        // push new EntryPoint
        lua_pushstring(L, name.c_str());
        EntryPoint **lep = (EntryPoint **)lua_newuserdata(L, sizeof(EntryPoint *));
        *lep = ep;

        // metatable
        lua_newtable(L);
        lua_pushcfunction (L, chimera::lua_ignore_newindex);
        lua_setfield(L, -2, "__newindex");
        lua_pushcfunction (L, chimera::lua_EntryPoint_call);
        lua_setfield(L, -2, "__call");
        lua_pushcfunction (L, chimera::lua_EntryPoint_len);
        lua_setfield(L, -2, "__len");
        lua_pushcfunction (L, chimera::lua_EntryPoint_tostring);
        lua_setfield(L, -2, "__tostring");
        lua_pushstring(L, chimera::luanames::LUA_NAME_ENTRYPOINT);
        lua_setfield(L, -2, "__name");
        lua_pushinteger(L, chimera::systemtypes::PID_ENTRYPOINT);
        lua_setfield(L, -2, "__type");
        lua_pushlstring(L, name.c_str(), name.size());
        lua_setfield(L, -2, "__ep");
        lua_pushlightuserdata(L, ep);
        lua_setfield(L, -2, "__ptr");

        // index table with all associated Lua values inEntryPoint
        lua_pushstring(L, "__index");
        lua_newtable(L);

        for(auto it = ep->beginItems(); it != ep->endItems(); it++)
        {
            lua_pushlstring(L, it->first.c_str(), it->first.size());
            _typeSys->pushValue(L, it->second.getType(), it->second.getValue());
            lua_rawset(L, -3);
        }
        lua_rawset(L, -3);

        // set individual meta table
        lua_setmetatable(L, -2);
        // set entrypoint also in references
        lua_pushvalue(L, -1);
        lua_rawsetp(L, -5, ep);
        // name value pair in system names
        lua_rawset(L, -3);

        _typeSys->createReference(chimera::systemtypes::PID_ENTRYPOINT, ep);
        _typeSys->addDependency(this, ep);
    }
    lua_settop(L, top);
}

void chimera::ChimeraSystem::popEntryPoint(EntryPoint* ep)
{
    lua_State* L = this->getLuaState();
    const std::string name = _epSys->findEntryPoint(ep);
    if(name.size() > 0)
    {
        int top = lua_gettop(L);
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
        lua_rawget(L, LUA_REGISTRYINDEX);
        // if EntryPoint exists
        if(lua_rawgetp(L, -1, ep))
        {
            _typeSys->removeParameter(ep);
            lua_pushnil(L);
            lua_rawsetp(L, -3, ep);

            lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
            lua_rawget(L, LUA_REGISTRYINDEX);
            lua_pushstring(L, name.c_str());
            lua_pushnil(L);
            lua_rawset(L, -3);

            chimera::EntryPoint **lep = (chimera::EntryPoint**)lua_touserdata(L, -2);
            *lep = nullptr;

            lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
            lua_gettable(L, LUA_REGISTRYINDEX);
            lua_rawgeti(L, -1, 0);
            lua_setmetatable(L, -4);

            _typeSys->releaseDependency(this, ep);
        }
        lua_settop(L, top);
    }
}

int chimera::lua_EntryPoint_call(lua_State* L)
{
    if(lua_gettop(L) == 2)
    {
        if(lua_isstring(L, 2))
        {
            ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
            chimera::EntryPoint* ep = *((chimera::EntryPoint**)lua_touserdata(L, 1));
            const chimera::Module* module;
            const char *name = lua_tostring(L, 2);
            module = ep->getModule(name);

            if(module == nullptr)
            {
                module = chimeraSystem->includeModule(ep, name);
            }

            if(module)
            {
                ep->loadModule(name);

                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                if(lua_rawgetp(L, -1, module))
                {
                    return 1;
                }
            }
        }
        else
        {
            //LoggingSystem::Error("Cannot load module. Supplied value is not a string.");
        }
    }
    else
    {
        //LoggingSystem::Error("Cannot load module. Too many arguments.");
    }

    return 0;
}

int chimera::lua_EntryPoint_len(lua_State* L)
{
    ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
    EntryPoint* ep = *((EntryPoint**)lua_touserdata(L, 1));
    if(chimeraSystem->getTypeSystem()->isReferenced(ep))
    {
        lua_pushinteger(L, ep->size());
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::lua_EntryPoint_tostring(lua_State* L)
{
    const EntryPoint *ep = *((const EntryPoint **)lua_touserdata(L, -1));
    if(ep)
    {
        lua_pushstring(L, ep->getGUID().c_str());
    }
    else
    {
        std::string txt(chimera::luanames::LUA_NAME_ENTRYPOINT);
        txt += "<RefError>";
        lua_pushstring(L, txt.c_str());
    }
    return 1;
}
