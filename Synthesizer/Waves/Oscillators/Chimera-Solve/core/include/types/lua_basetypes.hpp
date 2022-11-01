#ifndef LUA_BASETYPES_H
#define LUA_BASETYPES_H

typedef struct lua_State lua_State;

namespace chimera {
    int lua_UserData_gc(lua_State* L);
    int luat_UserData_push(lua_State* const L);
    int luat_nil_push(lua_State* const L);
    int luat_boolean_push(lua_State* const L);
    int luat_boolean_delete(lua_State* const L);
    int luat_real_push(lua_State* const L);
    int luat_real_delete(lua_State* const L);
    int luat_table_push(lua_State* const L);
    int luat_table_delete(lua_State* const L);
    int luat_string_push(lua_State* const L);
    int luat_string_delete(lua_State* const L);
    int luat_pointer_push(lua_State* const L);
    int luat_function_push(lua_State* const L);
    int luat_luafunction_call(lua_State* L);
    int luat_luafunction_init(lua_State* const L);
    int luat_luafunction_push(lua_State* const L);
    int luat_luafunction_delete(lua_State* const L);
}

#endif // LUA_BASETYPES_H
