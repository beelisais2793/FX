#ifndef LUA_SIMULATION_H
#define LUA_SIMULATION_H

namespace chimera {
    namespace runtime {
        int luat_Simulation_delete(lua_State* const L);
        int lua_Simulation_tostring(lua_State* L);
/*
        int lua_Simulation_add_event(lua_State* L);
        int lua_Simulation_register_eventlistener(lua_State* L);
        int lua_Simulation_index(lua_State* L);
        int lua_Simulation_start(lua_State* L);
        int lua_Simulation_stop(lua_State* L);
        int lua_Simulation_step(lua_State* L);
        int lua_Simulation_run(lua_State* L);
        int lua_Simulation_current(lua_State* L);
*/
    }
}

#endif // LUA_SIMULATION_H
