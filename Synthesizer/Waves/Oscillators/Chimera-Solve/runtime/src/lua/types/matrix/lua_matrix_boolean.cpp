/*
#include <boost/numeric/ublas/matrix.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "LoggingSystem.hpp"

#include "lua/types/lua_matrix.hpp"
#include "lua/types/matrix/lua_matrix_boolean.hpp"

namespace ublas = boost::numeric::ublas;

const std::string matrixBooleanMetaName = (std::string("meta:") + std::string(Naming::Type_Matrix) + "#" + std::string(Naming::Type_boolean));
/*
void luat_matrix_real_init(lua_State* const L, const int& type)
{
    lua_pushcfunction (L, lua_matrix_real_new);
    lua_setfield(L, -2, "__new");
    lua_pushcfunction (L, lua_matrix_real_get);
    lua_setfield(L, -2, "__get");
    lua_pushcfunction (L, lua_matrix_real_set);
    lua_setfield(L, -2, "__set");
    lua_getfield(L, -1, "__methods");
    map_t_LuaMethods* matrixMethods = (map_t_LuaMethods*)lua_touserdata(L, -1);
    (*matrixMethods)["det"] = lua_matrix_real_det;
    (*matrixMethods)["inv"] = lua_matrix_real_inv;
    (*matrixMethods)["dot"] = lua_matrix_real_dot;
    (*matrixMethods)["trace"] = lua_matrix_real_trace;
    (*matrixMethods)["eigen"] = lua_matrix_real_eigen;
    (*matrixMethods)["solve"] = lua_matrix_real_solve;
}

void luat_matrix_real_delete(lua_State* const L, void * const value, const int& type)
{
    const struct T_MatrixDef* md = (const struct T_MatrixDef*)value;
    const ublas::matrix<double>* mat = (const ublas::matrix<double>*)md->value;
    delete mat;
    delete md;
}

int lua_matrix_real_new(lua_State* const L)
{
    int a = lua_tointeger(L, 1);
    int b = lua_tointeger(L, 2);
    ublas::matrix<double>* m = new ublas::matrix<double>(a, b);
    if(lua_gettop(L) > 2)
    {
        double v = lua_tonumber(L, 3);
        for(int i = 0; i < a; i++)
            for(int j = 0; j < b; j++)
                (*m)(i, j) = v;
    }
    struct T_MatrixDef** md = (struct T_MatrixDef**)lua_newuserdata(L, sizeof(struct T_MatrixDef*));
    *md = new struct T_MatrixDef({a, b, false, true, m});
    luaL_setmetatable(L, matrixRealMetaName.c_str());
    return 1;
}

int lua_matrix_real_get(lua_State* const L)
{
    struct T_MatrixDef* md = *((struct T_MatrixDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    int b = lua_tointeger(L, 3);
    ublas::matrix<double>* m = (ublas::matrix<double>*)md->value;
    lua_pushnumber(L, (*m)(a, b));
    return 1;
}

int lua_matrix_real_set(lua_State* const L)
{
    struct T_MatrixDef* md = *((struct T_MatrixDef **)lua_touserdata(L, 1));
    int a = lua_tointeger(L, 2);
    int b = lua_tointeger(L, 3);
    double v = lua_tonumber(L, 4);
    ublas::matrix<double>* m = (ublas::matrix<double>*)md->value;

    (*m)(a, b) = v;
    return 0;
}

int lua_matrix_real_det(lua_State* const L)
{

}

int lua_matrix_real_inv(lua_State* const L)
{

}

int lua_matrix_real_dot(lua_State* const L)
{

}

int lua_matrix_real_trace(lua_State* const L)
{

}

int lua_matrix_real_eigen(lua_State* const L)
{

}

int lua_matrix_real_solve(lua_State* const L)
{

}

*/
