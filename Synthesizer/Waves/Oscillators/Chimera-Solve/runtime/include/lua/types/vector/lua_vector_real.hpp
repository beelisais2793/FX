#ifndef LUA_VECTOR_REAL_H
#define LUA_VECTOR_REAL_H

namespace chimera {
    namespace runtime {
        namespace types {
            int luat_vector_real_init(lua_State* const L);
            int luat_vector_real_delete(lua_State* const L);
            int lua_vector_real_new(lua_State* const L);
            int lua_vector_real_get(lua_State* const L);
            int lua_vector_real_set(lua_State* const L);
            int lua_vector_real_norm(lua_State* const L);
        }
    }
}

#endif // LUA_VECTOR_REAL_H
