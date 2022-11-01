#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

namespace chimera
{
    class ParameterValue;
    class EntryPoint;
    class Module;

    namespace systemtypes
    {
        const size_t PID_NIL = 0;
        const size_t PID_NUMBER = 1;
        const size_t PID_BOOLEAN = 2;
        const size_t PID_STRING = 3;
        const size_t PID_TABLE = 4;
        const size_t PID_FUNCTION = 5;
        const size_t PID_POINTER = 6;
        const size_t PID_LIBRARAY = 7;
        const size_t PID_LUAFUNC = 8;
        const size_t PID_ENTRYPOINT = 9;
        const size_t PID_MODULE = 10;
        const size_t PID_INSTANCE = 11;
    }

    typedef std::unordered_map<std::string, size_t> map_t_size;
    typedef std::unordered_map<std::string, chimera::EntryPoint*> map_t_EntryPoint;
    typedef std::unordered_map<std::string, chimera::Module*> map_t_Module;

    typedef std::vector<ParameterValue> vec_t_LuaItem;
    typedef std::unordered_map<std::string, ParameterValue> map_t_LuaItem;
    typedef void* (*fn_typecast)(size_t type, void * const value);
    typedef vec_t_LuaItem (*fn_luafnwrapper)(const vec_t_LuaItem& params);
    typedef vec_t_LuaItem (*fn_instancefnwrapper)(chimera::EntryPoint const * const entrypoint, chimera::Module const * const, void* instance, const vec_t_LuaItem& params);
}

#endif // COMMON_DEFINITIONS_H

#ifdef lua_h
#ifndef COMMON_DEFINITIONS_LUA
#define COMMON_DEFINITIONS_LUA
namespace chimera {
    typedef std::unordered_map<std::string, fn_luafnwrapper> map_t_LuaMethods;
    typedef std::unordered_map<std::string, fn_instancefnwrapper> map_t_InstanceMethods;
}


void DUMP_STACK(lua_State* L);
void DUMP_TABLE(lua_State* L, int i);

#endif

#endif


