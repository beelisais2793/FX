#ifndef LUA_TEMPORALSIMULATION_H
#define LUA_TEMPORALSIMULATION_H

namespace chimera {
    namespace runtime {
        //int lua_TemporalSimulation_add_event(lua_State* L);
        //int lua_TemporalSimulation_register_eventlistener(lua_State* L);
        int lua_TemporalSimulation_tostring(lua_State* L);
        //int lua_TemporalSimulation_index(lua_State* L);
        int lua_TemporalSimulation_start(lua_State* L);
        int lua_TemporalSimulation_stop(lua_State* L);
        int lua_TemporalSimulation_step(lua_State* L);
        int lua_TemporalSimulation_run(lua_State* L);
        int lua_TemporalSimulation_current(lua_State* L);
        int lua_TemporalSimulation_on(lua_State* L);
        int lua_TemporalSimulation_onStep(lua_State* L);
    }
}

#endif // LUA_TEMPORALSIMULATION_H
