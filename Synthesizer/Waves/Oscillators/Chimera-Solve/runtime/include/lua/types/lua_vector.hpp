#ifndef LUA_VECTOR_H
#define LUA_VECTOR_H

namespace chimera {
    namespace runtime {
        namespace types {
            int luat_vector_init(lua_State* const L);
            int lua_vector_iterator(lua_State* const L);
            int lua_vector_length(lua_State* const L);
            int lua_vector_tostring(lua_State* const L);
            int lua_vector_index(lua_State* const L);
            int lua_vector_newindex(lua_State* const L);
            int lua_vector_eq(lua_State* const L);
            bool lua_vector_newempty(lua_State* const L, int n);
            bool lua_vector_arith1(lua_State* const L, int op);
            bool lua_vector_arith2(lua_State* const L, int op);
            int lua_vector_add(lua_State* const L);
            int lua_vector_sub(lua_State* const L);
            int lua_vector_mul(lua_State* const L);
            int lua_vector_div(lua_State* const L);
            int lua_vector_mod(lua_State* const L);
            int lua_vector_pow(lua_State* const L);
            int lua_vector_unm(lua_State* const L);
            int lua_vector_idiv(lua_State* const L);
            int lua_vector_band(lua_State* const L);
            int lua_vector_bor(lua_State* const L);
            int lua_vector_bxor(lua_State* const L);
            int lua_vector_bnot(lua_State* const L);
            int lua_vector_shl(lua_State* const L);
            int lua_vector_shr(lua_State* const L);
            int lua_vector_get(lua_State* const L);
            int lua_vector_set(lua_State* const L);
            int lua_vector_all(lua_State* const L);
            int lua_vector_any(lua_State* const L);
            int lua_vector_foreach(lua_State* const L);
            int lua_vector_copy(lua_State* const L);
        }
    }
}

#endif // LUA_VECTOR_H
