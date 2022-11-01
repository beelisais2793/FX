#ifndef LUA_COMPLEX_H
#define LUA_COMPLEX_H

namespace chimera {
    namespace runtime {
        namespace types {
            int luat_complex_init(lua_State* const L);
            int luat_complex_delete(lua_State* const L);
            void push_complex_library(lua_State* const L);
            int lua_complex_new(lua_State* const L);
            int lua_complex_newPolar(lua_State* const L);
            int lua_complex_real(lua_State* const L);
            int lua_complex_imag(lua_State* const L);
            int lua_complex_abs(lua_State* const L);
            int lua_complex_arg(lua_State* const L);
            int lua_complex_conj(lua_State* const L);
            int lua_complex_tostring(lua_State* const L);
            int lua_complex_index(lua_State* const L);
            int lua_complex_add(lua_State* const L);
            int lua_complex_sub(lua_State* const L);
            int lua_complex_mul(lua_State* const L);
            int lua_complex_div(lua_State* const L);
            int lua_complex_pow(lua_State* const L);
            int lua_complex_unm(lua_State* const L);
            int lua_complex_equal(lua_State* const L);
            int lua_complex_exp(lua_State* const L);
            int lua_complex_log(lua_State* const L);
            int lua_complex_log10(lua_State* const L);
            int lua_complex_sqrt(lua_State* const L);
            int lua_complex_sin(lua_State* const L);
            int lua_complex_cos(lua_State* const L);
            int lua_complex_tan(lua_State* const L);
            int lua_complex_asin(lua_State* const L);
            int lua_complex_acos(lua_State* const L);
            int lua_complex_atan(lua_State* const L);
            int lua_complex_sinh(lua_State* const L);
            int lua_complex_cosh(lua_State* const L);
            int lua_complex_tanh(lua_State* const L);
            int lua_complex_asinh(lua_State* const L);
            int lua_complex_acosh(lua_State* const L);
            int lua_complex_atanh(lua_State* const L);
        }
    }
}

#endif // LUA_COMPLEX_H
