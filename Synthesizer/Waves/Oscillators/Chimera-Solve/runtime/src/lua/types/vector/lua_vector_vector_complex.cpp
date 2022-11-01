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
#include "lua/types/vector/lua_vector_vector_complex.hpp"

namespace ublas = boost::numeric::ublas;

static size_t vector_vector_complex_pid = 0;
static size_t vector_complex_pid = 0;

int chimera::runtime::types::luat_vector_vector_complex_init(lua_State* const L)
{
    vector_vector_complex_pid = lua_tointeger(L, 2);
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
    lua_pop(L, 1);
    vector_complex_pid = chimeraSystem->getTypeSystem()->getParameterTag(vector_vector_complex_pid);

    lua_pushcfunction (L, lua_vector_vector_complex_get);
    lua_setfield(L, -2, "__get");
    lua_pushcfunction (L, lua_vector_vector_complex_new);
    lua_setfield(L, -2, "__new");
    //lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::luat_vector_vector_complex_delete(lua_State* const L)
{
    void * const value = lua_touserdata(L, 2);
    const struct chimera::simulation::T_VectorDef* vd = (const struct chimera::simulation::T_VectorDef*)value;
    if(vd->deleteValue)
    {
        vec_vec_complex* v = (vec_vec_complex*)vd->value;
        delete v;
    }
    delete vd;

    return 0;
}

int chimera::runtime::types::lua_vector_vector_complex_new(lua_State* const L)
{
    int a = lua_tointeger(L, 1);
    vec_vec_complex* v = new vec_vec_complex(a);
    if(lua_gettop(L) > 1)
    {
        void * const value = lua_touserdata(L, 2);
        const struct chimera::simulation::T_VectorDef* initVd = *((const struct chimera::simulation::T_VectorDef**)value);
        ublas::vector<std::complex<double> >* initVec = (ublas::vector<std::complex<double> >*)initVd->value;
        int b = initVd->length;

        for(int i = 0; i < a; i++) {
            (*v)[i].resize(b);
            for(int j = 0; j < b; j++) {
                (*v)[i][j] = (*initVec)[j];
            }
        }
    } else {
        for(int i = 0; i < a; i++) {
            (*v)[i].resize(1);
            (*v)[i][0] = 0.;
        }
    }

    struct chimera::simulation::T_VectorDef** vd = (struct chimera::simulation::T_VectorDef**)lua_newuserdata(L, sizeof(struct chimera::simulation::T_VectorDef*));
    *vd = new struct chimera::simulation::T_VectorDef({a, false, true, v});

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, vector_vector_complex_pid);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    return 1;
}

int chimera::runtime::types::lua_vector_vector_complex_get(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* vd = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    vec_vec_complex* v = (vec_vec_complex*)vd->value;

    struct chimera::simulation::T_VectorDef** vd2 = (struct chimera::simulation::T_VectorDef**)lua_newuserdata(L, sizeof(struct chimera::simulation::T_VectorDef*));
    int s = ((*v)(a)).size();
    //auto vvi = new ublas::vector<double>((*v)(a));
    auto vvi = &((*v)[a]);
    *vd2 = new struct chimera::simulation::T_VectorDef({s, vd->readonly, false, vvi});

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, vector_complex_pid);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    return 1;
}
