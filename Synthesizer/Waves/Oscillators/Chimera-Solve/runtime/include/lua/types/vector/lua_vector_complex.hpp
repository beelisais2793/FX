#ifndef LUA_VECTOR_COMPLEX_H
#define LUA_VECTOR_COMPLEX_H

namespace chimera {
    namespace runtime {
        namespace types {
            int luat_vector_complex_init(lua_State* const L);
            int luat_vector_complex_delete(lua_State* const L);
            int lua_vector_complex_new(lua_State* const L);
            int lua_vector_complex_get(lua_State* const L);
            int lua_vector_complex_set(lua_State* const L);
            int lua_vector_complex_norm(lua_State* const L);
        }
    }
}

#endif // LUA_VECTOR_COMPLEX_H
