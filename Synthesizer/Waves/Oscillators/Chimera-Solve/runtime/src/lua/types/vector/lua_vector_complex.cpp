#include <boost/numeric/ublas/vector.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <complex>
#include <cmath>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
//#include "RuntimeNames.hpp"
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
#include "lua/types/lua_vector.hpp"
#include "lua/types/vector/lua_vector_complex.hpp"

namespace ublas = boost::numeric::ublas;

static size_t vector_complex_pid = 0;
static size_t complex_pid = 0;

int chimera::runtime::types::luat_vector_complex_init(lua_State* const L)
{
    vector_complex_pid = lua_tointeger(L, 2);
    ChimeraRuntime* chSys = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
    lua_pop(L, 1);
    //complex_pid = chSys->getTypeSystem()->getParameterID(chimera::simulation::Naming::Type_Complex);
    complex_pid = chSys->getTypeSystem()->getParameterTag(vector_complex_pid);

    lua_pushcfunction (L, lua_vector_complex_new);
    lua_setfield(L, -2, "__new");
    lua_pushcfunction (L, lua_vector_complex_get);
    lua_setfield(L, -2, "__get");
    lua_pushcfunction (L, lua_vector_complex_set);
    lua_setfield(L, -2, "__set");
    lua_getfield(L, -1, "__methods");
    std::unordered_map<std::string, lua_CFunction>* vectorMethods = (std::unordered_map<std::string, lua_CFunction>*)lua_touserdata(L, -1);
    (*vectorMethods)["norm"] = lua_vector_complex_norm;
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::luat_vector_complex_delete(lua_State* const L)
{
    void * const value = lua_touserdata(L, 2);
    const struct chimera::simulation::T_VectorDef* vd = (const struct chimera::simulation::T_VectorDef*)value;
    if(vd->deleteValue)
    {
        const ublas::vector<std::complex<double> >* v = (const ublas::vector<std::complex<double> >*)vd->value;
        delete v;
    }
    delete vd;

    return 0;
}

int chimera::runtime::types::lua_vector_complex_new(lua_State* const L)
{
    int a = lua_tointeger(L, 1);
    ublas::vector<std::complex<double> >* v = new ublas::vector<std::complex<double> >(a);
    if(lua_gettop(L) > 1)
    {
        std::complex<double>* val = *((std::complex<double>**)lua_touserdata(L, 2));
        for(int i = 0; i < a; i++)
            (*v)(i) = std::complex<double>(*val);
    }
    struct chimera::simulation::T_VectorDef** vd = (struct chimera::simulation::T_VectorDef**)lua_newuserdata(L, sizeof(struct chimera::simulation::T_VectorDef*));
    *vd = new struct chimera::simulation::T_VectorDef({a, false, true, v});

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, vector_complex_pid);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::lua_vector_complex_get(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    ublas::vector<std::complex<double> >* v = (ublas::vector<std::complex<double> >*)vd->value;

    std::complex<double>** val = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
    *val = new std::complex<double>((*v)(a));

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, complex_pid);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::lua_vector_complex_set(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    ublas::vector<std::complex<double> >* v = (ublas::vector<std::complex<double> >*)vd->value;
    int a = lua_tointeger(L, 2);

    if(lua_isnumber(L, 3))
    {
        double val = lua_tonumber(L, 3);
        (*v)(a) = val;
        return 0;
    }

    if(luaL_getmetafield(L, 3, "__type") && lua_tointeger(L, -1) == complex_pid)
    {
        std::complex<double>* val = *((std::complex<double>**)lua_touserdata(L, 3));

        (*v)(a) = std::complex<double>(*val);
        return 0;
    }

    return 0;
}

int chimera::runtime::types::lua_vector_complex_norm(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, lua_upvalueindex(1)));
    ublas::vector<std::complex<double> >* v = (ublas::vector<std::complex<double> >*)vd->value;
    int length = v->size();
    double sum = 0;
    double vi = 0;
    for(int i = 0; i < length; i++)
    {
        vi = std::abs((*v)(i));
        sum += vi * vi;
    }

    lua_pushnumber(L, sqrt(sum));
    return 1;
}
