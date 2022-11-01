#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
//#include <complex>
#include "lua.hpp"

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
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"
#include "types/lua_static.hpp"
#include "lua/types/matrix/lua_matrix_common.hpp"

static int meta_pid_matrix = 0;
static int meta_pid_matrixrow = 0;

void chimera::runtime::types::push_matrix_library(lua_State* const L, size_t pid_matrix, size_t pid_matrixrow)
{
    meta_pid_matrix = pid_matrix;
    meta_pid_matrixrow = pid_matrixrow;
    void** lib = (void**)lua_newuserdata(L, sizeof(void*));
    *lib = nullptr;
    lua_newtable(L);
    lua_pushcfunction (L, lua_ignore_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushstring (L, "Library");
    lua_setfield(L, -2, "__name");
    lua_pushinteger (L, chimera::systemtypes::PID_LIBRARAY);
    lua_setfield(L, -2, "__type");
    lua_pushcfunction (L, lua_matrix_new);
    lua_setfield(L, -2, "__call");
    lua_pushstring(L, "__index");
    lua_newtable(L);
    lua_pushcfunction (L, lua_matrix_eye);
    lua_setfield(L, -2, "eye");
    lua_pushcfunction (L, lua_matrix_ones);
    lua_setfield(L, -2, "ones");
    lua_pushcfunction (L, lua_matrix_zeros);
    lua_setfield(L, -2, "zeros");
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);
}

int chimera::runtime::types::lua_matrix_new(lua_State* const L)
{
    if(lua_gettop(L) != 4)
    {
#warning LOG (kekstoaster#1#): matrix.new requires 3 arguments
        //LoggingSystem::Error("matrix.new requires 3 arguments");
        return 0;
    }
    if(!lua_isinteger(L, 2))
    {
#warning LOG (kekstoaster#1#): argument `1` to matrix.new is not an integer number
        //LoggingSystem::Error("argument `1` to matrix.new is not an integer number");
        return 0;
    }
    if(!lua_isinteger(L, 3))
    {
#warning LOG (kekstoaster#1#): argument `2` to matrix.new is not an integer number
        //LoggingSystem::Error("argument `2` to matrix.new is not an integer number");
        return 0;
    }
    int a = lua_tointeger(L, 2);
    int b = lua_tointeger(L, 3);

    if(a <= 0 || b <= 0)
    {
#warning LOG (kekstoaster#1#): attempt to create invalid matrix in matrix.new (dimension <= 0)
        //LoggingSystem::Error("attempt to create invalid matrix in matrix.new (dimension <= 0)");
        return 0;
    }

    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    size_t innerType = chimeraSystem->getTypeSystem()->getParameterType(L, 4);
    size_t metaType = chimeraSystem->getTypeLookup()->findType(meta_pid_matrix, innerType);

    if(metaType != 0)
    {
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_rawget(L, LUA_REGISTRYINDEX);
        lua_rawgeti(L, -1, metaType);

        if(lua_getfield(L, -1, "__new") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 2);
            lua_pushvalue(L, 3);
            lua_pushvalue(L, 4);
            lua_call(L, 3, 1);
            return 1;
        }
    }
#warning LOG (kekstoaster#1#): Matrix type is not supported
    //LoggingSystem::Error("Matrix type is not supported");
    return 0;
}

int chimera::runtime::types::lua_matrix_eye(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_ones(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_zeros(lua_State* const L)
{
    return 0;
}
