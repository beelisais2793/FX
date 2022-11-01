#ifndef LUA_MATRIXROW_H
#define LUA_MATRIXROW_H

namespace chimera {
    namespace runtime {
        namespace types {
            int luat_matrixrow_init(lua_State* const L);
            int luat_matrixrow_delete(lua_State* const L);
            int lua_matrixrow_newindex(lua_State* const L);
            int lua_matrixrow_iterator(lua_State* const L);
            int lua_matrixrow_len(lua_State* const L);
            int lua_matrixrow_tostring(lua_State* const L);
            int lua_matrixrow_index(lua_State* const L);
        }
    }
}

#endif // LUA_MATRIXROW_H
