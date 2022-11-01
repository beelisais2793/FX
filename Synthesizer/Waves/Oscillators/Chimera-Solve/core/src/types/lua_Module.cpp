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
#include "types/lua_Module.hpp"
#include "ChimeraSystem.hpp"

void chimera::ChimeraSystem::pushModule(EntryPoint* ep, const std::string& name, Module* module)
{
    lua_State* L = this->getLuaState();
    int top = lua_gettop(L);
    std::string instanceTypeName("$");
    instanceTypeName += name;

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if(lua_rawgetp(L, -1, ep))
    {
        _typeSys->createReference(chimera::systemtypes::PID_MODULE, module);
        _typeSys->addDependency(this, module);

        //luaL_getmetafield(L, -1, "__loaded");

        int instType = _typeSys->registerParameter(instanceTypeName, module, {nullptr, instance_pushValue, instance_deleteValue, nullptr}, chimera::systemtypes::PID_INSTANCE, (size_t)ep);

        const Module **lmod = (const Module **)lua_newuserdata(L, sizeof(Module *));
        *lmod = module;
        int modPos = lua_absindex(L, -1);
        // module metatable
        lua_newtable(L);
        lua_pushcfunction (L, chimera::lua_ignore_newindex);
        lua_setfield(L, -2, "__newindex");
        lua_pushcfunction (L, chimera::lua_Module_call);
        lua_setfield(L, -2, "__call");
        lua_pushcfunction (L, chimera::lua_Module_tostring);
        lua_setfield(L, -2, "__tostring");
        lua_pushstring(L, chimera::luanames::LUA_NAME_MODULE);
        lua_setfield(L, -2, "__name");
        lua_pushinteger(L, chimera::systemtypes::PID_MODULE);
        lua_setfield(L, -2, "__type");
        lua_pushlightuserdata(L, ep);
        lua_setfield(L, -2, "__epptr");
        lua_pushlstring(L, name.c_str(), name.size());
        lua_setfield(L, -2, "__module");
        lua_pushinteger(L, instType);
        lua_setfield(L, -2, "__itype");
        lua_pushlstring(L, "__index", 7);
        lua_newtable(L);
        for(auto it = module->beginItems(); it != module->endItems(); it++)
        {
            lua_pushlstring(L, it->first.c_str(), it->first.size());
            _typeSys->pushValue(L, it->second.getType(), it->second.getValue());
            lua_rawset(L, -3);
        }

        lua_rawset(L, -3);
        lua_setmetatable(L, -2);
        lua_pushlightuserdata(L, (void*)module);
        lua_pushvalue(L, -2);
        lua_rawset(L, -5);

        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_rawget(L, LUA_REGISTRYINDEX);
        lua_rawgeti(L, -1, instType);

        lua_pushcfunction (L, chimera::lua_Instance_tostring);
        lua_setfield(L, -2, "__tostring");
        lua_pushcfunction (L, chimera::lua_ignore_newindex);
        lua_setfield(L, -2, "__newindex");
        lua_pushcfunction (L, chimera::lua_Instance_index);
        lua_setfield(L, -2, "__index");
        lua_pushstring(L, chimera::luanames::LUA_NAME_INSTANCE);
        lua_setfield(L, -2, "__name");
        lua_pushlightuserdata(L, ep);
        lua_setfield(L, -2, "__epptr");
        lua_pushlightuserdata(L, (void*)module);
        lua_setfield(L, -2, "__modptr");
        lua_pushlstring(L, name.c_str(), name.size());
        lua_setfield(L, -2, "__module");
        lua_pushcfunction(L, chimera::lua_UserData_gc);
        lua_setfield(L, -2, "__gc");
    }

    lua_settop(L, top);
}

void chimera::ChimeraSystem::popModule(EntryPoint* ep, Module* module)
{
    lua_State* L = this->getLuaState();
    const std::string name = ep->findModule(module);
    if(name.size() > 0)
    {
        int top = lua_gettop(L);
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
        lua_rawget(L, LUA_REGISTRYINDEX);
        if(lua_rawgetp(L, -1, ep))
        {
            _typeSys->removeParameter(module);
            //_typeSys->removeDependencyItem(module);

            if(lua_rawgetp(L, -2, module))
            {
                chimera::Module **lmod = (chimera::Module **)lua_touserdata(L, -1);
                *lmod = nullptr;
                lua_pushnil(L);
                lua_rawsetp(L, -4, module);

                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
                lua_gettable(L, LUA_REGISTRYINDEX);
                lua_rawgeti(L, -1, 0);
                lua_setmetatable(L, -3);
            }
        }
        lua_settop(L, top);

        _typeSys->releaseDependency(this, module);
    }
}

int chimera::lua_Module_call(lua_State* L)
{
    ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
    Module *mod = *((Module **)lua_touserdata(L, 1));
    if(chimeraSystem->getTypeSystem()->isReferenced(mod))
    {
        luaL_getmetafield(L, 1, "__itype");

        int type = lua_tointeger(L, -1);
        luaL_getmetafield(L, 1, "__epptr");
        const EntryPoint* lep = (const EntryPoint*)lua_touserdata(L, -1);
        lua_pop(L, 2);
        vec_t_LuaItem items;

        for(int i = 2; i <= lua_gettop(L); i++)
        {
            items.push_back(chimeraSystem->getTypeSystem()->getValue(L, i));
        }

        void* value = mod->getInstance(lep, items);
        if(value)
        {
            chimeraSystem->getTypeSystem()->pushValue(L, type, value);
            return 1;
        }
    }
    return 0;
}

int chimera::lua_Module_tostring(lua_State* L)
{
    const Module *lmod = *((const Module **)lua_touserdata(L, 1));

    std::string txt(chimera::luanames::LUA_NAME_MODULE);
    txt += "<";
    if(lmod)
    {
        luaL_getmetafield(L, 1, "__module");
        txt += lua_tostring(L, -1);
    }
    else
    {
        txt += "RefError";
    }
    txt += ">";

    lua_pushstring(L, txt.c_str());
    return 1;
}

int chimera::lua_Instance_tostring(lua_State* L)
{
    const void *inst = *((const void **)lua_touserdata(L, 1));

    std::string txt(chimera::luanames::LUA_NAME_INSTANCE);
    txt += "<";
    if(inst)
    {
        lua_getmetatable(L, -1);
        lua_getfield(L, -1, "__module");
        txt += lua_tostring(L, -1);
    }
    else
    {
        txt += "RefError";
    }
    txt += ">";

    lua_pushstring(L, txt.c_str());
    return 1;
}

int chimera::lua_Instance_forwardMethod(lua_State* L)
{
    ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
    luaL_getmetafield(L, lua_upvalueindex(1), "__modptr");
    Module* module = (Module *)lua_touserdata(L, -1);
    if(chimeraSystem->getTypeSystem()->isReferenced(module))
    {
        luaL_getmetafield(L, lua_upvalueindex(1), "__epptr");
        const EntryPoint* ep = (const EntryPoint*)lua_touserdata(L, -1);
        void* instance = *((void**)lua_touserdata(L, lua_upvalueindex(1)));
        fn_instancefnwrapper method = (fn_instancefnwrapper)lua_touserdata(L, lua_upvalueindex(2));
        lua_pop(L, 1);

        vec_t_LuaItem params;
        for(int i = 1; i <= lua_gettop(L); ++i)
        {
            params.push_back(chimeraSystem->getTypeSystem()->getValue(L, i));
        }
        lua_settop(L, 0);

        vec_t_LuaItem result = (*method)(ep, module, instance, params);

        for(auto it = result.begin(); it != result.end(); it++)
        {
            chimeraSystem->getTypeSystem()->pushValue(L, it->getType(), it->getValue());
        }

        return result.size();
    }
    return 0;
}

int chimera::lua_Instance_index(lua_State* L)
{
    ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
    luaL_getmetafield(L, 1, "__modptr");
    Module* module = (Module *)lua_touserdata(L, -1);
    if(chimeraSystem->getTypeSystem()->isReferenced(module))
    {
        std::string item(lua_tostring(L, 2));

        auto fnPtr = module->getMethod(item);
        if(fnPtr)
        {
            lua_pushvalue(L, 1);
            lua_pushlightuserdata(L, (void*)fnPtr);
            lua_pushcclosure(L, lua_Instance_forwardMethod, 2);
            return 1;
        }
    }
    return 0;
}

int chimera::instance_pushValue(lua_State* const L)
{
    void *value = (void *)lua_touserdata(L, 2);
    void **item = (void **)lua_newuserdata(L, sizeof(void *));
    *item = value;
    return 1;
}

int chimera::instance_deleteValue(lua_State* const L)
{
    ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
    void *value = (void *)lua_touserdata(L, 2);
    if(luaL_getmetafield(L, 1, "__modptr"))
    {
        Module* module = (Module *)lua_touserdata(L, -1);
        if(chimeraSystem->getTypeSystem()->isReferenced(module))
        {
            EntryPoint* ep = nullptr;
            if(luaL_getmetafield(L, 1, "__epptr"))
            {
                ep = (EntryPoint*)lua_touserdata(L, -1);
            }
            module->destroyInstance(ep, value);
            chimeraSystem->getTypeSystem()->removeDependencyItem(value);
        }
    }
    return 0;
}
