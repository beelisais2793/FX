#ifndef LUA_MATRIX_H
#define LUA_MATRIX_H

namespace chimera {
    namespace runtime {
        namespace types {
            int luat_matrix_init(lua_State* const L);
            int luat_matrixrow_init(lua_State* const L);
            int luat_matrix_delete(lua_State* const L);
            void push_matrix_library(lua_State* const L);
            bool lua_matrix_newempty(lua_State* const L, int m, int n);
            int lua_matrix_get(lua_State* const L);
            int lua_matrix_set(lua_State* const L);
            int lua_matrix_iterator(lua_State* const L);
            int lua_matrix_index(lua_State* const L);
            int lua_matrix_newindex(lua_State* const L);
            int lua_matrix_length(lua_State* const L);
            int lua_matrix_tostring(lua_State* const L);
            int lua_matrix_eq(lua_State* const L);
            int lua_matrix_iterator(lua_State* const L);
            bool lua_matrix_arith1(lua_State* const L, int op);
            bool lua_matrix_arith2(lua_State* const L, int op);
            int lua_matrix_add(lua_State* const L);
            int lua_matrix_sub(lua_State* const L);
            int lua_matrix_mul(lua_State* const L);
            int lua_matrix_div(lua_State* const L);
            int lua_matrix_mod(lua_State* const L);
            int lua_matrix_pow(lua_State* const L);
            int lua_matrix_unm(lua_State* const L);
            int lua_matrix_idiv(lua_State* const L);
            int lua_matrix_band(lua_State* const L);
            int lua_matrix_bor(lua_State* const L);
            int lua_matrix_bxor(lua_State* const L);
            int lua_matrix_bnot(lua_State* const L);
            int lua_matrix_shl(lua_State* const L);
            int lua_matrix_shr(lua_State* const L);
            int lua_matrix_transpose(lua_State* const L);
            int lua_matrix_all(lua_State* const L);
            int lua_matrix_any(lua_State* const L);
            int lua_matrix_foreach(lua_State* const L);
            int lua_matrix_copy(lua_State* const L);
        }
    }
}

#endif // LUA_MATRIX_H
