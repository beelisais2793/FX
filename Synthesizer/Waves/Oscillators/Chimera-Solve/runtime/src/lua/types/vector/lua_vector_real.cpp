#include <boost/numeric/ublas/vector.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include "lua.hpp"

#include "def.hpp"
#include "StateSynchrony.hpp"
#include "Naming.hpp"
#include "RuntimeNames.hpp"
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

#include "lua/types/lua_vector.hpp"
#include "lua/types/vector/lua_vector_real.hpp"

namespace ublas = boost::numeric::ublas;

static size_t vector_real_pid = 0;

int chimera::runtime::types::luat_vector_real_init(lua_State* const L)
{
    vector_real_pid = lua_tointeger(L, 2);
    lua_pop(L, 1);

    lua_pushcfunction (L, lua_vector_real_new);
    lua_setfield(L, -2, "__new");
    lua_pushcfunction (L, lua_vector_real_get);
    lua_setfield(L, -2, "__get");
    lua_pushcfunction (L, lua_vector_real_set);
    lua_setfield(L, -2, "__set");
    lua_getfield(L, -1, "__methods");
    std::unordered_map<std::string, lua_CFunction>* vectorMethods = (std::unordered_map<std::string, lua_CFunction>*)lua_touserdata(L, -1);
    (*vectorMethods)["norm"] = lua_vector_real_norm;
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::luat_vector_real_delete(lua_State* const L)
{
    void * const value = lua_touserdata(L, 2);
    const struct chimera::simulation::T_VectorDef* vd = (const struct chimera::simulation::T_VectorDef*)value;
    if(vd->deleteValue)
    {
        const ublas::vector<double>* v = (const ublas::vector<double>*)vd->value;
        delete v;
    }
    delete vd;
    return 0;
}

int chimera::runtime::types::lua_vector_real_new(lua_State* const L)
{
    int a = lua_tointeger(L, 1);
    ublas::vector<double>* v = new ublas::vector<double>(a);
    if(lua_gettop(L) > 1)
    {
        double val = lua_tonumber(L, 2);
        for(int i = 0; i < a; i++)
            (*v)(i) = val;
    }
    struct chimera::simulation::T_VectorDef** vd = (struct chimera::simulation::T_VectorDef**)lua_newuserdata(L, sizeof(struct chimera::simulation::T_VectorDef*));
    *vd = new struct chimera::simulation::T_VectorDef({a, false, true, v});

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, vector_real_pid);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::lua_vector_real_get(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    ublas::vector<double>* v = (ublas::vector<double>*)vd->value;
    lua_pushnumber(L, (*v)(a));
    return 1;
}

int chimera::runtime::types::lua_vector_real_set(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    double val = lua_tonumber(L, 3);
    ublas::vector<double>* v = (ublas::vector<double>*)vd->value;

    (*v)(a) = val;
    return 0;
}

int chimera::runtime::types::lua_vector_real_norm(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, lua_upvalueindex(1)));
    ublas::vector<double>* v = (ublas::vector<double>*)vd->value;
    int length = v->size();
    double sum = 0;
    for(int i = 0; i < length; i++)
        sum += (*v)(i) * (*v)(i);
    lua_pushnumber(L, sqrt(sum));
    return 1;
}
