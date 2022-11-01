#ifndef LUA_VECTOR_VECTOR_COMPLEX_H
#define LUA_VECTOR_VECTOR_COMPLEX_H

namespace chimera {
    namespace runtime {
        namespace types {
            typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > > vec_vec_complex;

            int luat_vector_vector_complex_init(lua_State* const L);
            int luat_vector_vector_complex_delete(lua_State* const L);
            int lua_vector_vector_complex_new(lua_State* const L);
            int lua_vector_vector_complex_get(lua_State* const L);
        }
    }
}

#endif // LUA_VECTOR_VECTOR_COMPLEX_H
