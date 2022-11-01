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
#include "RuntimeNames.hpp"
#include "StateSynchrony.hpp"
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
#include "lua/types/lua_matrix.hpp"
#include "lua/types/matrix/lua_matrix_real.hpp"

namespace ublas = boost::numeric::ublas;

static size_t matrix_real_pid = 0;

int chimera::runtime::types::luat_matrix_real_init(lua_State* const L)
{
    matrix_real_pid = lua_tointeger(L, 2);
    lua_pop(L, 1);

    lua_pushcfunction (L, lua_matrix_real_new);
    lua_setfield(L, -2, "__new");
    lua_pushcfunction (L, lua_matrix_real_get);
    lua_setfield(L, -2, "__get");
    lua_pushcfunction (L, lua_matrix_real_set);
    lua_setfield(L, -2, "__set");
    lua_getfield(L, -1, "__methods");
    std::unordered_map<std::string, lua_CFunction>* matrixMethods = (std::unordered_map<std::string, lua_CFunction>*)lua_touserdata(L, -1);
    (*matrixMethods)["det"] = lua_matrix_real_det;
    (*matrixMethods)["inv"] = lua_matrix_real_inv;
    (*matrixMethods)["dot"] = lua_matrix_real_dot;
    (*matrixMethods)["trace"] = lua_matrix_real_trace;
    (*matrixMethods)["eigen"] = lua_matrix_real_eigen;
    (*matrixMethods)["solve"] = lua_matrix_real_solve;
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::luat_matrix_real_delete(lua_State* const L)
{
    void * const value = lua_touserdata(L, 2);
    const struct chimera::simulation::T_MatrixDef* md = (const struct chimera::simulation::T_MatrixDef*)value;
    if(md->deleteValue)
    {
        const ublas::matrix<double>* m = (const ublas::matrix<double>*)md->value;
        delete m;
    }
    delete md;
    return 0;
}

int chimera::runtime::types::lua_matrix_real_new(lua_State* const L)
{
    int a = lua_tointeger(L, 1);
    int b = lua_tointeger(L, 2);
    ublas::matrix<double>* m = new ublas::matrix<double>(a, b);
    if(lua_gettop(L) > 2)
    {
        double v = lua_tonumber(L, 3);
        for(int i = 0; i < a; i++)
            for(int j = 0; j < b; j++)
                (*m)(i, j) = v;
    }
    struct chimera::simulation::T_MatrixDef** md = (struct chimera::simulation::T_MatrixDef**)lua_newuserdata(L, sizeof(struct chimera::simulation::T_MatrixDef*));
    *md = new struct chimera::simulation::T_MatrixDef({a, b, false, true, m});

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, matrix_real_pid);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::lua_matrix_real_get(lua_State* const L)
{
    struct chimera::simulation::T_MatrixDef* md = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    int b = lua_tointeger(L, 3);
    ublas::matrix<double>* m = (ublas::matrix<double>*)md->value;
    lua_pushnumber(L, (*m)(a, b));
    return 1;
}

int chimera::runtime::types::lua_matrix_real_set(lua_State* const L)
{
    struct chimera::simulation::T_MatrixDef* md = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    int b = lua_tointeger(L, 3);
    double v = lua_tonumber(L, 4);
    ublas::matrix<double>* m = (ublas::matrix<double>*)md->value;

    (*m)(a, b) = v;
    return 0;
}

int chimera::runtime::types::lua_matrix_real_det(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_real_inv(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_real_dot(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_real_trace(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_real_eigen(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_real_solve(lua_State* const L)
{
    return 0;
}
