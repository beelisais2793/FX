#ifndef LUA_STATIC_H
#define LUA_STATIC_H

namespace chimera {
    int lua_global_scope_newindex(lua_State* L);
    int lua_ignore_newindex(lua_State* L);
    int lua_UserData_gc(lua_State* L);
}

#endif // LUA_STATIC_H
