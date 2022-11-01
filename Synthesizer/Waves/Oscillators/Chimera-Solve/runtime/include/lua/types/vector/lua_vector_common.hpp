#ifndef LUA_VECTOR_COMMON_H
#define LUA_VECTOR_COMMON_H

namespace chimera {
    namespace runtime {
        namespace types {
            void push_vector_library(lua_State* const L, size_t pid_vector);
            int lua_vector_new(lua_State* const L);
        }
    }
}

#endif // LUA_VECTOR_COMMON_H
