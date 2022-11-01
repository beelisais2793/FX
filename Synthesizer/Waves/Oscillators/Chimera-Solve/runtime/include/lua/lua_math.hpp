#ifndef LUA_MATH_H
#define LUA_MATH_H

namespace chimera {
    namespace runtime {
        void push_math_library(lua_State* const L, const size_t& complexTypeId);
        int lua_math_abs(lua_State* const L);
        int lua_math_exp(lua_State* const L);
        int lua_math_log(lua_State* const L);
        int lua_math_log10(lua_State* const L);
        int lua_math_pow(lua_State* const L);
        int lua_math_sqrt(lua_State* const L);
        int lua_math_sin(lua_State* const L);
        int lua_math_cos(lua_State* const L);
        int lua_math_tan(lua_State* const L);
        int lua_math_asin(lua_State* const L);
        int lua_math_acos(lua_State* const L);
        int lua_math_atan(lua_State* const L);
        int lua_math_sinh(lua_State* const L);
        int lua_math_cosh(lua_State* const L);
        int lua_math_tanh(lua_State* const L);
        int lua_math_asinh(lua_State* const L);
        int lua_math_acosh(lua_State* const L);
        int lua_math_atanh(lua_State* const L);
    }
}

#endif // LUA_MATH_H
