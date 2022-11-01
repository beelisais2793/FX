#include <boost/numeric/ublas/vector.hpp>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <complex>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "RuntimeNames.hpp"
#include "ExtensionNaming.hpp"
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
#include "ChimeraContext.hpp"
#include "types/lua_static.hpp"
#include "lua/lua_math.hpp"
#include "lua/types/lua_complex.hpp"
#include "lua/types/lua_vector.hpp"
#include "lua/types/lua_matrixrow.hpp"
#include "lua/types/lua_matrix.hpp"
#include "lua/types/vector/lua_vector_common.hpp"
#include "lua/types/vector/lua_vector_real.hpp"
#include "lua/types/vector/lua_vector_complex.hpp"
#include "lua/types/vector/lua_vector_vector_real.hpp"
#include "lua/types/vector/lua_vector_vector_complex.hpp"
#include "lua/types/matrix/lua_matrix_common.hpp"
#include "lua/types/matrix/lua_matrix_real.hpp"
#include "lua/types/lua_Simulation.hpp"
#include "lua/lua_global.hpp"
#include "types/lua_basetypes.hpp"
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"

#include "EntryPoints/SystemDynamicEntryPoint.hpp"
#include "EntryPoints/InitializerEntryPoint.hpp"
#include "EntryPoints/IntegratorEntryPoint.hpp"
#include "EntryPoints/OutputEntryPoint.hpp"
#include "EntryPoints/RandomEntryPoint.hpp"
#include "EntryPoints/ObserverEntryPoint.hpp"
#include "EntryPoints/UtilEntryPoint.hpp"

int lua_show_global(lua_State* L)
{
    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_gettable(L, LUA_REGISTRYINDEX);
    int tabIdx = lua_absindex(L, -1);
    lua_pushnil(L);  /* first key */
    while (lua_next(L, tabIdx) != 0) {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        std::cout << ">> " << lua_tostring(L, -2) << std::endl;
        lua_pop(L, 1);
    }
    std::cout << "#############" << std::endl;
    return 0;
}

void DUMP_STACK(lua_State* L)
{
    int top = lua_gettop(L);
    std::cout << "############# STACK: " << top << std::endl;
    for (int i = 1; i <= top; i++)
    {
        std::cout << "[" << i << "] (" << luaL_typename(L, i) << ")  " << luaL_tolstring(L, i, NULL) << std::endl;
        lua_pop(L, 1);
    }
    std::cout << "#############" << std::endl;
}

void DUMP_TABLE(lua_State* L, int index)
{
    int tabIdx = lua_absindex(L, index);
    lua_len(L, index);
    std::cout << "############# Table[" << lua_tonumber(L, -1) << "]" << std::endl;
    lua_pop(L, 1);
    lua_pushnil(L);  /* first key */
    while (lua_next(L, tabIdx) != 0) {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        switch(lua_type(L, -2))
        {
            case LUA_TNUMBER:
                std::cout << "[" << lua_tonumber(L, -2) << "]"; //  (" << luaL_typename(L, -1) << ")  " << luaL_tolstring(L, -1, NULL) << std::endl;
                break;
            case LUA_TLIGHTUSERDATA:
                std::cout << "[" << lua_touserdata(L, -2) << "]";
                break;
            default:
                std::cout << "[" << lua_tostring(L, -2) << "]"; //  (" << luaL_typename(L, -1) << ")  " << luaL_tolstring(L, -1, NULL) << std::endl;
                break;
        }
        std::cout << " - " << luaL_tolstring(L, -1, NULL) << std::endl;
        lua_pop(L, 2);
    }
    std::cout << "#############" << std::endl;
}

chimera::runtime::ChimeraRuntime::ChimeraRuntime(const std::string& filename, std::vector<char*>& argv, ModuleLoader* loader)
{
    _loader = loader;
    _filename = new std::string(filename);
    _lookup = new TypeLookup(getTypeSystem());
    lua_State* L = getLuaState();

    _systemDynamic = new chimera::runtime::SystemDynamicEntryPoint();
    _initializer = new chimera::runtime::InitializerEntryPoint();
    _integrator = new chimera::runtime::IntegratorEntryPoint(_initializer);
    _output = new chimera::runtime::OutputEntryPoint();
    _random = new chimera::runtime::RandomEntryPoint();
    _observer = new chimera::runtime::ObserverEntryPoint();
    _util = new chimera::runtime::UtilEntryPoint();

    getEntryPointSystem()->addEntryPoint(chimera::simulation::Naming::EntryPoint_dynamics, _systemDynamic);
    getEntryPointSystem()->addEntryPoint(chimera::simulation::Naming::EntryPoint_initilizer, _initializer);
    getEntryPointSystem()->addEntryPoint(chimera::simulation::Naming::EntryPoint_integrator, _integrator);
    getEntryPointSystem()->addEntryPoint(chimera::simulation::Naming::EntryPoint_output, _output);
    getEntryPointSystem()->addEntryPoint(chimera::simulation::Naming::EntryPoint_random, _random);
    getEntryPointSystem()->addEntryPoint(chimera::simulation::Naming::EntryPoint_observer, _observer);
    getEntryPointSystem()->addEntryPoint(chimera::simulation::Naming::EntryPoint_util, _util);

    // TODO (kekstoaster#1#): replace string with const
    _pid_vector = getContext()->registerParameter(chimera::simulation::Naming::Type_Vector, {types::luat_vector_init, nullptr, nullptr, nullptr});
    _pid_matrix = getContext()->registerParameter(chimera::simulation::Naming::Type_Matrix, {types::luat_matrix_init, nullptr, nullptr, nullptr});
    _pid_matrixrow = getContext()->registerParameter(chimera::simulation::Naming::Type_MatrixRow, {types::luat_matrixrow_init, nullptr, types::luat_matrixrow_delete, nullptr});

    _lookup->addTypeLookup(_pid_vector);
    _lookup->addTypeLookup(_pid_matrix);
    _lookup->addTypeLookup(_pid_matrixrow);

    lua_pushstring(L, Naming::Lua_reg_argv);
    lua_createtable(L, argv.size(), 0);
    for(unsigned int i = 1; i < argv.size(); i++) {
        lua_pushstring(L, argv[i]);
        lua_rawseti (L, -2, i);
    }
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_SYSTEMNAMES);
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, Naming::Lua_global_log);
    lua_pushcfunction (L, lua_global_log);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_print);
    lua_pushcfunction (L, lua_global_print);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_write);
    lua_pushcfunction (L, lua_global_write);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_writeline);
    lua_pushcfunction (L, lua_global_writeline);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_error);
    lua_pushcfunction (L, lua_global_error);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_warning);
    lua_pushcfunction (L, lua_global_warning);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_exit);
    lua_pushcfunction (L, lua_global_exit);
    lua_rawset(L, -3);


    //////////////////////////////////////////////
    lua_pushstring(L, "showglobal");          ////
    lua_pushcfunction (L, lua_show_global);   ////
    lua_rawset(L, -3);                        ////
    //////////////////////////////////////////////


    lua_pushstring(L, Naming::Lua_global_simulation);
    lua_pushcfunction (L, lua_global_simulation);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_complex);
    types::push_complex_library(L);
    lua_rawset(L, -3);

    //lua_pushstring(L, Naming::Lua_global_random);
    //push_random_library(L);
    //lua_rawset(L, -3);

    _pid_complex = getContext()->registerParameter(chimera::simulation::Naming::Type_Complex, {types::luat_complex_init, chimera::luat_UserData_push, types::luat_complex_delete, nullptr});
    lua_pushstring(L, Naming::Lua_global_math);
    push_math_library(L, _pid_complex);
    lua_rawset(L, -3);

    lua_pushstring(L, "J");
    std::complex<double>** c = (std::complex<double>**)lua_newuserdata(L, sizeof(std::complex<double>*));
    *c = new std::complex<double>(0.0, 1.0);

    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, _pid_complex);
    lua_setmetatable(L, -3);
    lua_pop(L, 1);

    lua_rawset(L, -3);

    size_t vecRealId = registerVector(chimera::systemtypes::PID_NUMBER, {types::luat_vector_real_init, chimera::luat_UserData_push, types::luat_vector_real_delete, nullptr});
    size_t vecCompId = registerVector(_pid_complex, {types::luat_vector_complex_init, chimera::luat_UserData_push, types::luat_vector_complex_delete, nullptr});
    registerVector(vecRealId, {types::luat_vector_vector_real_init, chimera::luat_UserData_push, types::luat_vector_vector_real_delete, nullptr});
    registerVector(vecCompId, {types::luat_vector_vector_complex_init, chimera::luat_UserData_push, types::luat_vector_vector_complex_delete, nullptr});

    registerMatrix(chimera::systemtypes::PID_NUMBER, {types::luat_matrix_real_init, chimera::luat_UserData_push, types::luat_matrix_real_delete, nullptr});

    lua_pushstring(L, Naming::Lua_global_vector);
    types::push_vector_library(L, _pid_vector);
    lua_rawset(L, -3);

    lua_pushstring(L, Naming::Lua_global_matrix);
    types::push_matrix_library(L, _pid_matrix, _pid_matrixrow);
    lua_rawset(L, -3);

    // add argv table, which has all elements as
    // meta table with __index, so elements cannot be changed
    lua_pushstring(L, Naming::Lua_global_argv);
    lua_newtable(L);  // value for argv
    lua_newtable(L);  // meta table for argv

    // set __index in metatable
    lua_pushstring(L, "__index");
    lua_pushstring(L, Naming::Lua_reg_argv);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_rawset(L, -3);

    // set __newindex in metatable
    lua_pushstring(L, "__newindex");
    lua_pushcfunction (L, lua_ignore_newindex);
    lua_rawset(L, -3);

    // set __len in metatable
    lua_pushstring(L, "__len");
    lua_pushcfunction (L, lua_argv_len);
    lua_rawset(L, -3);

    lua_setmetatable(L, -2);  // set meta table for argv
    lua_rawset(L, -3);  // set argv

    std::unordered_map<std::string, size_t> simulationAttributes;
    simulationAttributes[chimera::simulation::Naming::Attribute_Observable] = 1;

    getContext()->registerParameter(chimera::simulation::Naming::Type_Simulation, {nullptr, nullptr, luat_Simulation_delete, nullptr}, simulationAttributes);

    lua_settop(L, 0);
}

chimera::runtime::ChimeraRuntime::~ChimeraRuntime()
{
    _loader->unloadAll();
    delete _systemDynamic;
    delete _integrator;
    delete _output;
    delete _random;
    delete _initializer;
    delete _observer;
    delete _util;
}

chimera::runtime::TypeLookup const * const chimera::runtime::ChimeraRuntime::getTypeLookup() const
{
    return _lookup;
}

chimera::Module* chimera::runtime::ChimeraRuntime::openModule(chimera::EntryPoint const * const entrypoint, const std::string& name)
{
    return _loader->load(name);
}

void chimera::runtime::ChimeraRuntime::closeModule(chimera::Module* module)
{
    _loader->unload(module);
}

void chimera::runtime::ChimeraRuntime::notifyDelete(chimera::StateSynchrony* sender)
{
    return;
}

size_t chimera::runtime::ChimeraRuntime::registerVector(const size_t& subtype, const struct T_ParameterDef& pdef)
{
    if(getTypeSystem()->exists(subtype))
    {
        size_t exists = getTypeLookup()->findType(_pid_vector, subtype);
        if(exists == 0)
        {
            std::string name(chimera::simulation::Naming::Type_Vector);
            name += "#";
            name += getTypeSystem()->getParameterName(subtype);
            return getContext()->registerParameter(name, pdef, _pid_vector, subtype);
        }
#warning LOG (kekstoaster#1#): Vector type already exists
    }
#warning LOG (kekstoaster#1#): invalid subtype for Vector
    return 0;
}

size_t chimera::runtime::ChimeraRuntime::registerMatrix(const size_t& subtype, const struct T_ParameterDef& pdef)
{
    if(getTypeSystem()->exists(subtype))
    {
        size_t exists = getTypeLookup()->findType(_pid_matrix, subtype);
        if(exists == 0) {
            std::string nameMatrix(chimera::simulation::Naming::Type_Matrix);
            nameMatrix += "#";
            nameMatrix += getTypeSystem()->getParameterName(subtype);

            std::string nameMatrixrow(chimera::simulation::Naming::Type_MatrixRow);
            nameMatrixrow += "#";
            nameMatrixrow += getTypeSystem()->getParameterName(subtype);

            int mId = getContext()->registerParameter(nameMatrix, pdef, _pid_matrix, subtype);
            getContext()->registerParameter(nameMatrixrow, {nullptr, nullptr, types::luat_matrixrow_delete, nullptr}, _pid_matrixrow, subtype);
            return mId;
        }
#warning LOG (kekstoaster#1#): Matrix type already exists
    }
#warning LOG (kekstoaster#1#): invalid subtype for Matrix
    return 0;
}
