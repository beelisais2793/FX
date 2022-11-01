#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
//#include <string.h>
#include <vector>
#include <list>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "StateSynchrony.hpp"
#include "ParameterTypeSystem.hpp"
#include "ChimeraSystem.hpp"
#include "types/lua_static.hpp"

int chimera::lua_global_scope_newindex(lua_State* L)
{
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 2);
    lua_gettable(L, -2);
    if(lua_isnoneornil(L, -1))
    {
        lua_settop(L, 3);
        lua_rawset(L, -3);
    }

    lua_settop(L, 0);
    return 0;
}

int chimera::lua_ignore_newindex(lua_State* L)
{
    lua_settop(L, 0);
    return 0;
}

int chimera::lua_UserData_gc(lua_State* L)
{
    if(luaL_getmetafield(L, 1, "__type")) {
        int type = lua_tointeger(L, 2);
        void* value = *((void**)lua_touserdata(L, 1));

        ChimeraSystem* chimeraSystem = (ChimeraSystem*)(*((void**)lua_getextraspace(L)));
        if(value)
        {
            if(chimeraSystem->getTypeSystem()->isReferenced(value))
            {
                chimeraSystem->getTypeSystem()->releaseDependency(L, value);
            }
            else
            {
                chimeraSystem->getTypeSystem()->deleteValue(L, type, value);
            }
        }
    }
    return 0;
}
