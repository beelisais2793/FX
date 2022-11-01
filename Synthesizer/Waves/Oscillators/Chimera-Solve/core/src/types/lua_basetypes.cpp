#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "StateSynchrony.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ChimeraSystem.hpp"
#include "types/lua_basetypes.hpp"

int chimera::luat_UserData_push(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    const void **item = (const void **)lua_newuserdata(L, sizeof(void *));
    *item = value;
    return 1;
}

int chimera::luat_nil_push(lua_State* const L)
{
    lua_pushnil(L);
    return 1;
}

int chimera::luat_boolean_push(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    lua_pushboolean(L, *((bool*)value));
    return 1;
}

int chimera::luat_boolean_delete(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    delete ((bool*)value);
    return 0;
}

int chimera::luat_real_push(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    lua_pushnumber(L, *((double*)value));
    return 1;
}

int chimera::luat_real_delete(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    delete ((double*)value);
    return 0;
}

int chimera::luat_table_push(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));

    map_t_LuaItem* table = (map_t_LuaItem*)value;

    lua_createtable(L, 0, table->size());
    for(auto item : *table)
    {
        lua_pushstring(L, item.first.c_str());
        chimeraSystem->getTypeSystem()->pushValue(L, item.second.getType(), item.second.getValue());
        lua_rawset(L, -3);
    }
    return 1;
}

int chimera::luat_table_delete(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    delete ((map_t_LuaItem*)value);
    return 0;
}

int chimera::luat_string_push(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    std::string* s = (std::string*)value;
    lua_pushlstring(L, s->c_str(), s->size());
    return 1;
}

int chimera::luat_string_delete(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    delete ((std::string*)value);
    return 0;
}

int chimera::luat_pointer_push(lua_State* const L)
{
    return 1;
}

int chimera::luat_function_push(lua_State* const L)
{
    const void *value = (const void *)lua_touserdata(L, 2);
    lua_pushcfunction(L, (lua_CFunction)value);
    return 1;
}

int chimera::luat_luafunction_call(lua_State* L)
{
    ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
    LuaFunctionWrapper* obj = *((LuaFunctionWrapper**)lua_touserdata(L, 1));
    vec_t_LuaItem items;
    for(int i = 2; i <= lua_gettop(L); i++)
    {
        items.push_back(chimeraSystem->getTypeSystem()->getValue(L, i));
    }

    vec_t_LuaItem result = (*obj)(items);
    for(auto it : result)
    {
        chimeraSystem->getTypeSystem()->pushValue(L, it.getType(), it.getValue());
    }
    return result.size();
}

int chimera::luat_luafunction_init(lua_State* const L)
{
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_pop(L, 1);
    lua_pushcfunction(L, luat_luafunction_call);
    lua_setfield(L, -2, "__call");
    return 1;
}

int chimera::luat_luafunction_push(lua_State* const L)
{
    const LuaFunctionWrapper *value = (const LuaFunctionWrapper *)lua_touserdata(L, 2);
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pushlightuserdata(L, (void*)value->_origin);

    //LuaFunctionWrapper* obj = (LuaFunctionWrapper*)value;
    if(!lua_rawget(L, -2))
    {
        lua_pop(L, 1);
        LuaFunctionWrapper **item = (LuaFunctionWrapper**)lua_newuserdata(L, sizeof(LuaFunctionWrapper *));
        *item = (LuaFunctionWrapper*)value;
        lua_pushlightuserdata(L, (void*)value->_origin);
        lua_pushvalue(L, -2);
        lua_rawset(L, -4);
    }
    return 1;
}

int chimera::luat_luafunction_delete(lua_State* const L)
{
    LuaFunctionWrapper* fw = (LuaFunctionWrapper*)lua_touserdata(L, 2);
    if(fw->intern()) {
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
        lua_rawget(L, LUA_REGISTRYINDEX);
        lua_pushlightuserdata(L, (void*)fw->_origin);
        lua_pushnil(L);
        lua_rawset(L, -3);
    }
    delete fw;
    return 0;
}
