#ifndef LUA_REGISTRY_H
#define LUA_REGISTRY_H

namespace chimera {
    int lua_Module_call(lua_State* L);

    int lua_Module_tostring(lua_State* L);

    int lua_Instance_tostring(lua_State* L);

    int lua_Instance_index(lua_State* L);

    int lua_Instance_forwardMethod(lua_State* L);

    int instance_pushValue(lua_State* const L);

    int instance_deleteValue(lua_State* const L);
}
#endif // LUA_REGISTRY_H
