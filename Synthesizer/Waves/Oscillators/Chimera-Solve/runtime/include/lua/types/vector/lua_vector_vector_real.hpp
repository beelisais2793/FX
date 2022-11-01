#ifndef LUA_VECTOR_VECTOR_REAL_H
#define LUA_VECTOR_VECTOR_REAL_H

namespace chimera {
    namespace runtime {
        namespace types {
            typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > vec_vec_real;

            int luat_vector_vector_real_init(lua_State* const L);
            int luat_vector_vector_real_delete(lua_State* const L);
            int lua_vector_vector_real_new(lua_State* const L);
            int lua_vector_vector_real_get(lua_State* const L);
        }
    }
}

#endif // LUA_VECTOR_VECTOR_REAL_H
