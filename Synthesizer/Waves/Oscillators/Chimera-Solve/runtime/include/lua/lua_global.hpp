#ifndef LUA_RUNTIME_STATIC_H
#define LUA_RUNTIME_STATIC_H

namespace chimera {
    namespace runtime {
        int lua_global_log(lua_State* L);
        int lua_global_print(lua_State* L);
        int lua_global_write(lua_State* L);
        int lua_global_writeline(lua_State* L);
        int lua_global_userevent(lua_State* L);
        int lua_global_error(lua_State* L);
        int lua_global_warning(lua_State* L);
        int lua_global_exit(lua_State* L);
        int lua_global_simulation(lua_State* L);
        int lua_argv_len(lua_State* L);

        int lua_EntryPoint_call(lua_State* L);
        int lua_EntryPoint_len(lua_State* L);
        int lua_EntryPoint_tostring(lua_State* L);

        int lua_module_index(lua_State* L);
        int lua_module_tostring(lua_State* L);

        //void showTable(lua_State* _L);
    }
}

#endif // LUA_RUNTIME_STATIC_H
