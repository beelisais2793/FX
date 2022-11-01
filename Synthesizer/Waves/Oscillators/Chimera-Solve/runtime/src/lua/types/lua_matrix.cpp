#include <boost/numeric/ublas/matrix.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <complex>
#include "lua.hpp"

#include "def.hpp"
#include "StateSynchrony.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
//#include "RuntimeNames.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "TypeLookup.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"
#include "lua/types/lua_matrix.hpp"

namespace ublas = boost::numeric::ublas;

static size_t matrix_pid = 0;

int chimera::runtime::types::luat_matrix_init(lua_State* const L)
{
    int type = lua_tointeger(L, 2);
    lua_pop(L, 1);

    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    if(chimeraSystem->getTypeSystem()->getParameterBase(type) == 0 && matrix_pid == 0) {
        matrix_pid = type;
    }

    lua_pushcfunction (L, lua_matrix_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction (L, lua_matrix_length);
    lua_setfield(L, -2, "__len");
    lua_pushcfunction (L, lua_matrix_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction (L, lua_matrix_eq);
    lua_setfield(L, -2, "__eq");
    lua_pushcfunction (L, lua_matrix_iterator);
    lua_setfield(L, -2, "__call");
    lua_pushcfunction (L, lua_matrix_add);
    lua_setfield(L, -2, "__add");
    lua_pushcfunction (L, lua_matrix_sub);
    lua_setfield(L, -2, "__sub");
    lua_pushcfunction (L, lua_matrix_mul);
    lua_setfield(L, -2, "__mul");
    lua_pushcfunction (L, lua_matrix_div);
    lua_setfield(L, -2, "__div");
    lua_pushcfunction (L, lua_matrix_mod);
    lua_setfield(L, -2, "__mod");
    lua_pushcfunction (L, lua_matrix_pow);
    lua_setfield(L, -2, "__pow");
    lua_pushcfunction (L, lua_matrix_unm);
    lua_setfield(L, -2, "__unm");
    lua_pushcfunction (L, lua_matrix_idiv);
    lua_setfield(L, -2, "__idiv");
    lua_pushcfunction (L, lua_matrix_band);
    lua_setfield(L, -2, "__band");
    lua_pushcfunction (L, lua_matrix_bor);
    lua_setfield(L, -2, "__bor");
    lua_pushcfunction (L, lua_matrix_bxor);
    lua_setfield(L, -2, "__bxor");
    lua_pushcfunction (L, lua_matrix_bnot);
    lua_setfield(L, -2, "__bnot");
    lua_pushcfunction (L, lua_matrix_shl);
    lua_setfield(L, -2, "__shl");
    lua_pushcfunction (L, lua_matrix_shr);
    lua_setfield(L, -2, "__shr");

    lua_pushcfunction (L, lua_matrix_index);
    lua_setfield(L, -2, "__index");
    std::unordered_map<std::string, lua_CFunction>* matrixMethods = new std::unordered_map<std::string, lua_CFunction>();
    (*matrixMethods)["transpose"] = lua_matrix_transpose;
    (*matrixMethods)["all"] = lua_matrix_all;
    (*matrixMethods)["any"] = lua_matrix_any;
    (*matrixMethods)["foreach"] = lua_matrix_foreach;
    (*matrixMethods)["copy"] = lua_matrix_copy;
    (*matrixMethods)["get"] = lua_matrix_get;
    (*matrixMethods)["set"] = lua_matrix_set;

    lua_pushlightuserdata(L, (void*)matrixMethods);
    lua_setfield(L, -2, "__methods");

    return 1;
}

int chimera::runtime::types::lua_matrix_iterator(lua_State* const L)
{
    if(lua_gettop(L) >= 3)
    {
        struct chimera::simulation::T_MatrixDef* m = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, 1));
        int i = 0;
        if(lua_isnil(L, 3)) {
            i = 0;
        } else if(lua_isinteger(L, 3)) {
            i = lua_tointeger(L, 3) + 1;
        } else {
            return 0;
        }

        if(i < m->rows)
        {
            if(luaL_getmetafield(L, 1, "__type"))
            {
                ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
                int itemtype = chimeraSystem->getTypeSystem()->getParameterTag(lua_tointeger(L, -1));
                lua_pushinteger(L, i);
                struct chimera::simulation::T_MatrixRowDef** mrow = (struct chimera::simulation::T_MatrixRowDef **)lua_newuserdata(L, sizeof(struct chimera::simulation::T_MatrixRowDef*));
                *mrow = new struct chimera::simulation::T_MatrixRowDef({i, m->cols, m->readonly, m->value});
                static size_t matrixrow_pid = chimeraSystem->getTypeSystem()->getParameterID(chimera::simulation::Naming::Type_MatrixRow);
                size_t metaType = chimeraSystem->getTypeLookup()->findType(matrixrow_pid, itemtype);

                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_rawgeti(L, -1, metaType);
                lua_setmetatable(L, -3);
                lua_pop(L, 1);

                //luaL_setmetatable(L, ((matrixrowMetaName + "#") + ParameterTypeSystem::getParameterName(itemtype)).c_str());
                return 2;
            }
        }
    }
    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

int chimera::runtime::types::lua_matrix_length(lua_State* const L)
{
    struct chimera::simulation::T_MatrixDef* m = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, 1));
    lua_pushinteger(L, m->rows);
    return 1;
}

int chimera::runtime::types::lua_matrix_tostring(lua_State* const L)
{
    struct chimera::simulation::T_MatrixDef* m = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, 1));

    std::string text(chimera::simulation::Naming::Type_Matrix);
    if(luaL_getmetafield(L, 1, "__type"))
    {
        ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
        int itemtype = chimeraSystem->getTypeSystem()->getParameterTag(lua_tointeger(L, -1));
        text += "<";
        text += chimeraSystem->getTypeSystem()->getParameterName(itemtype);
        text += ">";
    }
    text += "(";
    text += std::to_string(m->rows);
    text += ",";
    text += std::to_string(m->cols);
    text += ")";
    lua_pushlstring(L, text.c_str(), text.size());
    return 1;
}

int chimera::runtime::types::lua_matrix_index(lua_State* const L)
{
    switch(lua_type(L, 2))
    {
    case LUA_TSTRING:
        {
            struct chimera::simulation::T_MatrixDef* m = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, 1));
            std::string item(lua_tostring(L, 2));
            if(item == "size")
            {
                lua_pushnumber(L, m->rows);
                lua_pushnumber(L, m->cols);
                return 2;
            }
            else if(item == "readonly")
            {
                lua_pushboolean(L, m->readonly);
                return 1;
            }
            else
            {
                if(m->readonly && item == "set"){
#warning LOG (kekstoaster#1#): set method is not allowed for readonly matrices
                    //LoggingSystem::Error("set method is not allowed for readonly matrices");
                    return 0;
                }
                if(luaL_getmetafield(L, 1, "__methods"))
                {
                    std::unordered_map<std::string, lua_CFunction>* matrixMethods = (std::unordered_map<std::string, lua_CFunction>*)lua_touserdata(L, -1);
                    std::unordered_map<std::string, lua_CFunction>::const_iterator available = matrixMethods->find (item);
                    if(available != matrixMethods->end())
                    {
                        lua_pushvalue(L, 1);
                        lua_pushcclosure(L, available->second, 1);
                        return 1;
                    }
                }
            }
        }
    case LUA_TNUMBER:
        {
            int isNum = 0;
            int i = lua_tointegerx(L, 2, &isNum);
            if(isNum)
            {
                struct chimera::simulation::T_MatrixDef* m = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, 1));
                if(i >= 0 && i < m->rows)
                {
                    if(luaL_getmetafield(L, 1, "__type"))
                    {
                        ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
                        int itemtype = chimeraSystem->getTypeSystem()->getParameterTag(lua_tointeger(L, -1));
                        struct chimera::simulation::T_MatrixRowDef** mrow = (struct chimera::simulation::T_MatrixRowDef **)lua_newuserdata(L, sizeof(struct chimera::simulation::T_MatrixRowDef*));
                        *mrow = new struct chimera::simulation::T_MatrixRowDef({i, m->cols, m->readonly, m->value});

                        static size_t matrixrow_pid = chimeraSystem->getTypeSystem()->getParameterID(chimera::simulation::Naming::Type_MatrixRow);
                        size_t metaType = chimeraSystem->getTypeLookup()->findType(matrixrow_pid, itemtype);

                        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
                        lua_rawget(L, LUA_REGISTRYINDEX);
                        lua_rawgeti(L, -1, metaType);
                        lua_setmetatable(L, -3);
                        lua_pop(L, 1);
                        return 1;
                    }
                }
            }
            break;
        }
    }
#warning LOG (kekstoaster#1#): no valid index lookup for matrix
    //LoggingSystem::Error("no valid index lookup for matrix");
    return 0;
}

int chimera::runtime::types::lua_matrix_newindex(lua_State* const L)
{
    // ignore
    lua_settop(L, 0);
    return 0;
}

int chimera::runtime::types::lua_matrix_eq(lua_State* const L)
{
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    if(luaL_getmetafield(L, 1, "__type"))
    {
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != matrix_pid)
        {
            lua_pushboolean(L, false);
            return 1;
        }
        lua_pop(L, 1);
    }
    else
    {
        lua_pushboolean(L, false);
        return 1;
    }

    if(luaL_getmetafield(L, 2, "__type"))
    {
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != matrix_pid)
        {
            lua_pushboolean(L, false);
            return 1;
        }
        lua_pop(L, 1);
    }
    else
    {
        lua_pushboolean(L, false);
        return 1;
    }

    struct chimera::simulation::T_MatrixDef* m1 = *((struct chimera::simulation::T_MatrixDef**)lua_touserdata(L, 1));
    struct chimera::simulation::T_MatrixDef* m2 = *((struct chimera::simulation::T_MatrixDef**)lua_touserdata(L, 2));

    if(m1->rows != m2->rows || m1->cols != m2->cols)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    for(int i = 0; i < m1->rows; i++)
    {
        for(int j = 0; j < m1->cols; j++)
        {
            luaL_getmetafield(L, 1, "__get");
            lua_pushvalue(L, 1);
            lua_pushinteger(L, i);
            lua_pushinteger(L, j);
            lua_pcall(L, 3, 1, 0);

            luaL_getmetafield(L, 2, "__get");
            lua_pushvalue(L, 2);
            lua_pushinteger(L, i);
            lua_pushinteger(L, j);
            lua_pcall(L, 3, 1, 0);

            if(!lua_compare(L, -1, -2, LUA_OPEQ))
            {
                lua_pushboolean(L, false);
                return 1;
            }
            lua_pop(L, 2);
        }
    }

    lua_pushboolean(L, true);
    return 1;
}

bool chimera::runtime::types::lua_matrix_newempty(lua_State* const L, int m, int n)
{
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    size_t innerType = chimeraSystem->getTypeSystem()->getParameterType(L, -1);
    size_t metaType = chimeraSystem->getTypeLookup()->findType(matrix_pid, innerType);

    if(metaType != 0)
    {
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_rawget(L, LUA_REGISTRYINDEX);
        lua_rawgeti(L, -1, metaType);

        if(lua_getfield(L, -1, "__new") == LUA_TFUNCTION)
        {
            lua_pushinteger(L, m);
            lua_pushinteger(L, n);
            lua_call(L, 2, 1);
            lua_insert(L, -3);
            lua_pop(L, 2);
            return true;
        }

        lua_pop(L, 2);
    }
    return false;
}

bool chimera::runtime::types::lua_matrix_arith1(lua_State* const L, int op)
{
    struct chimera::simulation::T_MatrixDef* m1 = *((struct chimera::simulation::T_MatrixDef**)lua_touserdata(L, 1));
    lua_pop(L, 1);

    for(int i = 0; i < m1->rows; i++)
    {
        for(int j = 0; j < m1->cols; j++)
        {
            luaL_getmetafield(L, 1, "__get");
            lua_pushvalue(L, 1);
            lua_pushinteger(L, i);
            lua_pushinteger(L, j);
            lua_pcall(L, 3, 1, 0);
            lua_arith(L, op);

            if(!(i || j)) // i == 0 && j == 0
            {
                if(lua_matrix_newempty(L, m1->rows, m1->cols))
                {
                    lua_insert(L, 2);
                }
                else
                {
#warning LOG (kekstoaster#1#): esult matrix could not be created
                    //LoggingSystem::Error("result matrix could not be created");
                    return false;
                }
            }

            if(luaL_getmetafield(L, 2, "__set") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, 2);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_rotate(L, -5, -1);
                lua_pcall(L, 4, 0, 0);
            }
            else
            {
#warning LOG (kekstoaster#1#): result matrix has no set function
                //LoggingSystem::Error("result matrix has no set function");
                return false;
            }
        }
    }
    return true;
}

bool chimera::runtime::types::lua_matrix_arith2(lua_State* const L, int op)
{
    bool arg1 = false;
    bool arg2 = false;
    int r, c;

    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    if(luaL_getmetafield(L, 1, "__type"))
    {
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != matrix_pid)
        {
            arg1 = true;
        }
        lua_pop(L, 1);
    }
    else
    {
        arg1 = true;
    }

    if(luaL_getmetafield(L, 2, "__type"))
    {
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != matrix_pid)
        {
            arg2 = true;
        }
        lua_pop(L, 1);
    }
    else
    {
        arg2 = true;
    }

    if(arg1 && arg2)
    {
#warning LOG (kekstoaster#1#): no valid matrix supplied
        //LoggingSystem::Error("no valid matrix supplied");
        return false;
    }

    struct chimera::simulation::T_MatrixDef* m1 = nullptr;
    if(!arg1)
    {
        m1 = *((struct chimera::simulation::T_MatrixDef**)lua_touserdata(L, 1));
        r = m1->rows;
        c = m1->cols;
    }

    struct chimera::simulation::T_MatrixDef* m2 = nullptr;
    if(!arg2)
    {
        m2 = *((struct chimera::simulation::T_MatrixDef**)lua_touserdata(L, 2));
        r = m2->rows;
        c = m2->cols;
    }

    if((!arg1 && !arg2) && (m1->rows != m2->rows || m1->cols != m2->cols))
    {
        return false;
    }

    for(int i = 0; i < r; i++)
    {
        for(int j = 0; j < c; j++)
        {
            if(arg1)
            {
                lua_pushvalue(L, 1);
            }
            else
            {
                luaL_getmetafield(L, 1, "__get");
                lua_pushvalue(L, 1);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_pcall(L, 3, 1, 0);
            }

            if(arg2)
            {
                lua_pushvalue(L, 2);
            }
            else
            {
                luaL_getmetafield(L, 2, "__get");
                lua_pushvalue(L, 2);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_pcall(L, 3, 1, 0);
            }

            lua_arith(L, op);

            if(!(i || j)) // i == 0 && j == 0
            {
                if(lua_matrix_newempty(L, r, c))
                {
                    lua_insert(L, 3);
                }
                else
                {
#warning LOG (kekstoaster#1#): result matrix could not be created
                    //LoggingSystem::Error("result matrix could not be created");
                    return false;
                }
            }

            if(luaL_getmetafield(L, 3, "__set") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, 3);
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_rotate(L, -5, -1);
                lua_pcall(L, 4, 0, 0);
            }
            else
            {
#warning LOG (kekstoaster#1#): result matrix has no set function
                //LoggingSystem::Error("result matrix has no set function");
                return false;
            }
        }
    }
    return true;
}

int chimera::runtime::types::lua_matrix_add(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPADD))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_sub(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPSUB))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_mul(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPMUL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_div(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPDIV))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_mod(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPMOD))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_pow(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPPOW))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_unm(lua_State* const L)
{
    if(lua_matrix_arith1(L, LUA_OPUNM))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_idiv(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPIDIV))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_band(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPBAND))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_bor(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPBOR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_bxor(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPBXOR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_bnot(lua_State* const L)
{
    if(lua_matrix_arith1(L, LUA_OPBNOT))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_shl(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPSHL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_shr(lua_State* const L)
{
    if(lua_matrix_arith2(L, LUA_OPSHR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_matrix_get(lua_State* const L)
{
    if(lua_gettop(L) == 2)
    {
        struct chimera::simulation::T_MatrixDef* m = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, lua_upvalueindex(1)));
        int isnum;
        int i = lua_tointegerx(L, 1, &isnum);
        if(!isnum)
        {
#warning LOG (kekstoaster#1#): argument `1` is not a valid index.
            //LoggingSystem::Error("argument `1` is not a valid index.");
        }
        int j = lua_tointegerx(L, 2, &isnum);
        if(!isnum)
        {
#warning LOG (kekstoaster#1#): argument `2` is not a valid index.
            //LoggingSystem::Error("argument `2` is not a valid index.");
        }
        if(i >= 0 && i < m->rows && j >= 0 && j < m->cols)
        {
            if(luaL_getmetafield(L, lua_upvalueindex(1), "__get") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, lua_upvalueindex(1));
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_pcall(L, 3, 1, 0);
                return 1;
            }
            else
            {
#warning LOG (kekstoaster#1#): no get method defined
                //LoggingSystem::Error("no get method defined");
            }
        }
        else
        {
#warning LOG (kekstoaster#1#): Index out of bounds in matrix get
            //LoggingSystem::Error("Index out of bounds in matrix get");
        }
    }
    return 0;
}

int chimera::runtime::types::lua_matrix_set(lua_State* const L)
{
    if(lua_gettop(L) == 3)
    {
        struct chimera::simulation::T_MatrixDef* m = *((struct chimera::simulation::T_MatrixDef **)lua_touserdata(L, lua_upvalueindex(1)));
        int isnum;
        int i = lua_tointegerx(L, 1, &isnum);
        if(!isnum)
        {
#warning LOG (kekstoaster#1#): argument `1` is not a valid index.
            //LoggingSystem::Error("argument `1` is not a valid index.");
            return 0;
        }
        int j = lua_tointegerx(L, 2, &isnum);
        if(!isnum)
        {
#warning LOG (kekstoaster#1#): argument `2` is not a valid index.
            //LoggingSystem::Error("argument `2` is not a valid index.");
            return 0;
        }
        if(luaL_getmetafield(L, lua_upvalueindex(1), "__type"))
        {
            int type = lua_tointeger(L, -1);
            ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

            if(chimeraSystem->getTypeSystem()->getParameterType(L, 3) != chimeraSystem->getTypeSystem()->getParameterTag(type))
            {
#warning LOG (kekstoaster#1#): Invalid type argument `3`.
                //LoggingSystem::Error("Invalid type argument `3`.");
                return 0;
            }
        }
        else
        {
            return 0;
        }
        if(i >= 0 && i < m->rows && j >= 0 && j < m->cols)
        {
            if(luaL_getmetafield(L, lua_upvalueindex(1), "__set") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, lua_upvalueindex(1));
                lua_pushinteger(L, i);
                lua_pushinteger(L, j);
                lua_pushvalue(L, 3);
                lua_pcall(L, 4, 0, 0);
                return 1;
            }
            else
            {
#warning LOG (kekstoaster#1#): no set method defined
                //LoggingSystem::Error("no set method defined");
            }
        }
        else
        {
#warning LOG (kekstoaster#1#): Index out of bounds in matrix get
            //LoggingSystem::Error("Index out of bounds in matrix get");
        }
    }
    return 0;
}

int chimera::runtime::types::lua_matrix_transpose(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_all(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_any(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_foreach(lua_State* const L)
{
    return 0;
}

int chimera::runtime::types::lua_matrix_copy(lua_State* const L)
{
    return 0;
}
