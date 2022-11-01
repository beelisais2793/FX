#include <boost/numeric/ublas/matrix.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
//#include <complex>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
//#include "RuntimeNames.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "TypeLookup.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"

#include "lua/types/lua_matrixrow.hpp"

static size_t matrixrow_pid = 0;

int chimera::runtime::types::luat_matrixrow_init(lua_State* const L)
{
    int type = lua_tointeger(L, 2);
    lua_pop(L, 1);

    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    if(chimeraSystem->getTypeSystem()->getParameterBase(type) == 0 && matrixrow_pid == 0) {
        matrixrow_pid = type;
    }

    std::string tn(chimeraSystem->getTypeSystem()->getParameterName(type));
    std::string tnb(chimeraSystem->getTypeSystem()->getParameterName(chimeraSystem->getTypeSystem()->getParameterBase(type)));
    std::string tnt(chimeraSystem->getTypeSystem()->getParameterName(chimeraSystem->getTypeSystem()->getParameterTag(type)));
    lua_pushcfunction (L, lua_matrixrow_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction (L, lua_matrixrow_len);
    lua_setfield(L, -2, "__len");
    lua_pushcfunction (L, lua_matrixrow_iterator);
    lua_setfield(L, -2, "__call");
    lua_pushcfunction (L, lua_matrixrow_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction (L, lua_matrixrow_index);
    lua_setfield(L, -2, "__index");

    static size_t matrix_pid = chimeraSystem->getTypeSystem()->getParameterID(chimera::simulation::Naming::Type_Matrix);
    size_t tag = chimeraSystem->getTypeSystem()->getParameterTag(type);
    size_t metaType = chimeraSystem->getTypeLookup()->findType(matrix_pid, chimeraSystem->getTypeSystem()->getParameterTag(type));

    if(metaType != 0)
    {
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_rawget(L, LUA_REGISTRYINDEX);
        lua_rawgeti(L, -1, metaType);
        //lua_remove(L, -2);

        if(lua_getfield(L, -1, "__get"))
        {
            lua_setfield(L, 1, "__get");
        }
        else
        {
            lua_pop(L, 1);
        }

        if(lua_getfield(L, -1, "__set"))
        {
            lua_setfield(L, 1, "__set");
        }
        else
        {
            lua_pop(L, 1);
        }
        lua_pop(L, 2);
    }
    return 1;
}

int chimera::runtime::types::luat_matrixrow_delete(lua_State* const L)
{
    struct chimera::simulation::T_MatrixRowDef const * const value = (const struct chimera::simulation::T_MatrixRowDef*)lua_touserdata(L, 2);
    delete value;
    return 0;
}

int chimera::runtime::types::lua_matrixrow_newindex(lua_State* const L)
{
    struct chimera::simulation::T_MatrixRowDef* m = *((struct chimera::simulation::T_MatrixRowDef **)lua_touserdata(L, 1));
    int i = lua_tointeger(L, 2);
    if(i >= 0 && i < m->length)
    {
        if(luaL_getmetafield(L, 1, "__set") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 1);
            lua_pushinteger(L, m->row);
            lua_pushinteger(L, i);
            lua_pushvalue(L, 3);
            lua_pcall(L, 4, 0, 0);
        }
        else
        {
#warning LOG (kekstoaster#1#): no set method defined
            //LoggingSystem::Error("no set method defined");
        }
    }
    else
    {
#warning LOG (kekstoaster#1#): Index out of bounds in matrix set
        //LoggingSystem::Error("Index out of bounds in matrix set");
    }
    return 0;
}

int chimera::runtime::types::lua_matrixrow_iterator(lua_State* const L)
{
    if(lua_gettop(L) >= 3)
    {
        struct chimera::simulation::T_MatrixRowDef* m = *((struct chimera::simulation::T_MatrixRowDef **)lua_touserdata(L, 1));
        int i = 0;
        if(lua_isnil(L, 3)) {
            i = 0;
        } else if(lua_isinteger(L, 3)) {
            int isnum = 0;
            i = lua_tointegerx(L, 3, &isnum) + 1;
            if(!isnum)
            {
                return 0;
            }
        } else {
            return 0;
        }

        if(i < m->length)
        {
            lua_pushinteger(L, i);

            if(luaL_getmetafield(L, 1, "__get") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, 1);
                lua_pushinteger(L, m->row);
                lua_pushinteger(L, i);
                lua_pcall(L, 3, 1, 0);
            }
            else
            {
#warning LOG (kekstoaster#1#): Matrix has no associated get method
                //LoggingSystem::Error("Matrix has no associated get method");
                return 0;
            }

            return 2;
        }
    }
    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

int chimera::runtime::types::lua_matrixrow_len(lua_State* const L)
{
    struct chimera::simulation::T_MatrixRowDef* m = *((struct chimera::simulation::T_MatrixRowDef **)lua_touserdata(L, 1));
    lua_pushinteger(L, m->length);
    return 1;
}

int chimera::runtime::types::lua_matrixrow_tostring(lua_State* const L)
{
    struct chimera::simulation::T_MatrixRowDef* m = *((struct chimera::simulation::T_MatrixRowDef **)lua_touserdata(L, 1));

    std::string text(chimera::simulation::Naming::Type_MatrixRow);
    if(luaL_getmetafield(L, 1, "__type"))
    {
        ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
        int itemtype = chimeraSystem->getTypeSystem()->getParameterTag(lua_tointeger(L, -1));
        text += "<";
        text += chimeraSystem->getTypeSystem()->getParameterName(itemtype);
        text += ">";
    }
    text += "(";
    text += std::to_string(m->length);
    text += ")";
    lua_pushlstring(L, text.c_str(), text.size());
    return 1;
}

int chimera::runtime::types::lua_matrixrow_index(lua_State* const L)
{
    struct chimera::simulation::T_MatrixRowDef* m = *((struct chimera::simulation::T_MatrixRowDef **)lua_touserdata(L, 1));
    int isnum = 0;
    int i = lua_tointegerx(L, 2, &isnum);
    if(!isnum)
    {
#warning LOG (kekstoaster#1#): no valid index for matrix
        return 0;
    }

    if(i >= 0 && i < m->length)
    {
        if(luaL_getmetafield(L, 1, "__get") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 1);
            lua_pushinteger(L, m->row);
            lua_pushinteger(L, i);
            lua_pcall(L, 3, 1, 0);
            return 1;
        }
        else
        {
#warning LOG (kekstoaster#1#): no get method defined
            //LoggingSystem::Error("no get method defined");
        }
    }
    else
    {
#warning LOG (kekstoaster#1#): Index out of bounds in matrix get
        //LoggingSystem::Error("Index out of bounds in matrix get");
    }
    return 0;
}
