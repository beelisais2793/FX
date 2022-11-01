#ifndef LUA_MATRIX_COMMON_H
#define LUA_MATRIX_COMMON_H

namespace chimera {
    namespace runtime {
        namespace types {
            void push_matrix_library(lua_State* const L, size_t pid_matrix, size_t pid_matrixrow);
            int lua_matrix_new(lua_State* const L);
            int lua_matrix_eye(lua_State* const L);
            int lua_matrix_ones(lua_State* const L);
            int lua_matrix_zeros(lua_State* const L);
        }
    }
}

#endif // LUA_MATRIX_COMMON_H
