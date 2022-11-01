#include <boost/numeric/ublas/vector.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
//#include <complex>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
//#include "RuntimeNames.hpp"
#include "StateSynchrony.hpp"
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
#include "lua/types/lua_vector.hpp"

namespace ublas = boost::numeric::ublas;

static size_t vector_pid = 0;

int chimera::runtime::types::luat_vector_init(lua_State* const L)
{
    int type = lua_tointeger(L, 2);
    lua_pop(L, 1);

    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    if(chimeraSystem->getTypeSystem()->getParameterBase(type) == 0 && vector_pid == 0) {
        vector_pid = type;
    }

    lua_pushcfunction (L, lua_vector_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction (L, lua_vector_length);
    lua_setfield(L, -2, "__len");
    lua_pushcfunction (L, lua_vector_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction (L, lua_vector_eq);
    lua_setfield(L, -2, "__eq");
    lua_pushcfunction (L, lua_vector_iterator);
    lua_setfield(L, -2, "__call");
    lua_pushcfunction (L, lua_vector_add);
    lua_setfield(L, -2, "__add");
    lua_pushcfunction (L, lua_vector_sub);
    lua_setfield(L, -2, "__sub");
    lua_pushcfunction (L, lua_vector_mul);
    lua_setfield(L, -2, "__mul");
    lua_pushcfunction (L, lua_vector_div);
    lua_setfield(L, -2, "__div");
    lua_pushcfunction (L, lua_vector_mod);
    lua_setfield(L, -2, "__mod");
    lua_pushcfunction (L, lua_vector_pow);
    lua_setfield(L, -2, "__pow");
    lua_pushcfunction (L, lua_vector_unm);
    lua_setfield(L, -2, "__unm");
    lua_pushcfunction (L, lua_vector_idiv);
    lua_setfield(L, -2, "__idiv");
    lua_pushcfunction (L, lua_vector_band);
    lua_setfield(L, -2, "__band");
    lua_pushcfunction (L, lua_vector_bor);
    lua_setfield(L, -2, "__bor");
    lua_pushcfunction (L, lua_vector_bxor);
    lua_setfield(L, -2, "__bxor");
    lua_pushcfunction (L, lua_vector_bnot);
    lua_setfield(L, -2, "__bnot");
    lua_pushcfunction (L, lua_vector_shl);
    lua_setfield(L, -2, "__shl");
    lua_pushcfunction (L, lua_vector_shr);
    lua_setfield(L, -2, "__shr");

    lua_pushcfunction (L, lua_vector_index);
    lua_setfield(L, -2, "__index");

    std::unordered_map<std::string, lua_CFunction>* vectorMethods = new std::unordered_map<std::string, lua_CFunction>();
    (*vectorMethods)["all"] = lua_vector_all;
    (*vectorMethods)["any"] = lua_vector_any;
    (*vectorMethods)["foreach"] = lua_vector_foreach;
    (*vectorMethods)["copy"] = lua_vector_copy;
    (*vectorMethods)["get"] = lua_vector_get;
    (*vectorMethods)["set"] = lua_vector_set;

    lua_pushlightuserdata(L, (void*)vectorMethods);
    lua_setfield(L, -2, "__methods");

    return 1;
}

int chimera::runtime::types::lua_vector_iterator(lua_State* const L)
{
    if(lua_gettop(L) >= 3)
    {
        struct chimera::simulation::T_VectorDef* v = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
        int i = 0;
        if(lua_isnil(L, 3)) {
            i = 0;
        } else if(lua_isnumber(L, 3)) {
            int isnum = 0;
            i = lua_tointegerx(L, 3, &isnum) + 1;
            if(!isnum)
            {
                return 0;
            }
        } else {
            return 0;
        }

        if(i < v->length)
        {
            lua_pushinteger(L, i);

            if(luaL_getmetafield(L, 1, "__get") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, 1);
                lua_pushinteger(L, i);
                lua_pcall(L, 2, 1, 0);
            }
            else
            {
#warning LOG (kekstoaster#1#): Vector has no associated get method
                //LoggingSystem::Error("Vector has no associated get method");
                return 0;
            }

            return 2;
        }
    }
    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

int chimera::runtime::types::lua_vector_length(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* v = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    lua_pushinteger(L, v->length);
    return 1;
}

int chimera::runtime::types::lua_vector_tostring(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* v = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));

    std::string text(chimera::simulation::Naming::Type_Vector);
    if(luaL_getmetafield(L, 1, "__type"))
    {
        ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
        int itemtype = chimeraSystem->getTypeSystem()->getParameterTag(lua_tointeger(L, -1));
        text += "<";
        text += chimeraSystem->getTypeSystem()->getParameterName(itemtype);
        text += ">";
    }
    text += "(";
    text += std::to_string(v->length);
    text += ")";
    lua_pushlstring(L, text.c_str(), text.size());
    return 1;
}

int chimera::runtime::types::lua_vector_index(lua_State* const L)
{
    switch(lua_type(L, 2))
    {
    case LUA_TSTRING:
        {
            struct chimera::simulation::T_VectorDef* v = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
            std::string item(lua_tostring(L, 2));
            if(item == "length")
            {
                lua_pushnumber(L, v->length);
                return 1;
            }
            else if(item == "readonly")
            {
                lua_pushboolean(L, v->readonly);
                return 1;
            }
            else
            {
                if(v->readonly && item == "set"){
#warning LOG (kekstoaster#1#): set method is not allowed for readonly vectors
                    //LoggingSystem::Error("set method is not allowed for readonly vectors");
                    return 0;
                }
                if(luaL_getmetafield(L, 1, "__methods"))
                {
                    std::unordered_map<std::string, lua_CFunction>* vectorMethods = (std::unordered_map<std::string, lua_CFunction>*)lua_touserdata(L, -1);
                    std::unordered_map<std::string, lua_CFunction>::const_iterator available = vectorMethods->find (item);
                    if(available != vectorMethods->end())
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
                struct chimera::simulation::T_VectorDef* v = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
                if(i >= 0 && i < v->length)
                {
                    if(luaL_getmetafield(L, 1, "__get") == LUA_TFUNCTION)
                    {
                        lua_pushvalue(L, 1);
                        lua_pushinteger(L, i);
                        lua_pcall(L, 2, 1, 0);
                        return 1;
                    }
                    else
                    {
#warning LOG (kekstoaster#1#): no get method defined
                        //LoggingSystem::Error("no get method defined");
                    }
                }
            }
            break;
        }
    }
#warning LOG (kekstoaster#1#): no valid index lookup for vector
    //LoggingSystem::Error("no valid index lookup for vector");
    return 0;
}

int chimera::runtime::types::lua_vector_newindex(lua_State* const L)
{
    struct chimera::simulation::T_VectorDef* m = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, 1));
    int i = lua_tointeger(L, 2);
    if(i >= 0 && i < m->length)
    {
        if(luaL_getmetafield(L, 1, "__set") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 1);
            lua_pushinteger(L, i);
            lua_pushvalue(L, 3);
            lua_pcall(L, 3, 0, 0);
        }
        else
        {
#warning LOG (kekstoaster#1#): no set method defined
            //LoggingSystem::Error("no set method defined");
        }
    }
    else
    {
#warning LOG (kekstoaster#1#): Index out of bounds in vector set
        //LoggingSystem::Error("Index out of bounds in vector set");
    }
    return 0;
}

int chimera::runtime::types::lua_vector_eq(lua_State* const L)
{
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    if(luaL_getmetafield(L, 1, "__type"))
    {
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != vector_pid)
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
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != vector_pid)
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

    struct chimera::simulation::T_VectorDef* v1 = *((struct chimera::simulation::T_VectorDef**)lua_touserdata(L, 1));
    struct chimera::simulation::T_VectorDef* v2 = *((struct chimera::simulation::T_VectorDef**)lua_touserdata(L, 2));

    if(v1->length != v2->length)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    for(int i = 0; i < v1->length; i++)
    {
        luaL_getmetafield(L, 1, "__get");
        lua_pushvalue(L, 1);
        lua_pushinteger(L, i);
        lua_pcall(L, 2, 1, 0);

        luaL_getmetafield(L, 2, "__get");
        lua_pushvalue(L, 2);
        lua_pushinteger(L, i);
        lua_pcall(L, 2, 1, 0);

        if(!lua_compare(L, -1, -2, LUA_OPEQ))
        {
            lua_pushboolean(L, false);
            return 1;
        }
        lua_pop(L, 2);
    }

    lua_pushboolean(L, true);
    return 1;
}

bool chimera::runtime::types::lua_vector_newempty(lua_State* const L, int n)
{
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    size_t innerType = chimeraSystem->getTypeSystem()->getParameterType(L, -1);
    size_t metaType = chimeraSystem->getTypeLookup()->findType(vector_pid, innerType);

    if(metaType != 0)
    {
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
        lua_rawget(L, LUA_REGISTRYINDEX);
        lua_rawgeti(L, -1, metaType);

        if(lua_getfield(L, -1, "__new") == LUA_TFUNCTION)
        {
            lua_pushnumber(L, n);
            lua_call(L, 1, 1);
            lua_insert(L, -3);
            lua_pop(L, 2);
            return true;
        }
        lua_pop(L, 2);
    }
    return false;
}

bool chimera::runtime::types::lua_vector_arith1(lua_State* const L, int op)
{
    struct chimera::simulation::T_VectorDef* v1 = *((struct chimera::simulation::T_VectorDef**)lua_touserdata(L, 1));
    //lua_pop(L, 1);

    for(int i = 0; i < v1->length; i++)
    {
        luaL_getmetafield(L, 1, "__get");
        lua_pushvalue(L, 1);
        lua_pushinteger(L, i);
        lua_pcall(L, 2, 1, 0);
        lua_arith(L, op);

        if(!i) // i == 0
        {
            if(lua_vector_newempty(L, v1->length))
            {
                lua_insert(L, 2);
            }
            else
            {
#warning LOG (kekstoaster#1#): result vector could not be created
                //LoggingSystem::Error("result vector could not be created");
                return false;
            }
        }

        if(luaL_getmetafield(L, 2, "__set") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 2);
            lua_pushinteger(L, i);
            lua_rotate(L, -4, -1);
            lua_pcall(L, 3, 0, 0);
        }
        else
        {
#warning LOG (kekstoaster#1#): result vector has no set function
            //LoggingSystem::Error("result vector has no set function");
            return false;
        }
    }
    return true;
}

bool chimera::runtime::types::lua_vector_arith2(lua_State* const L, int op)
{
    bool arg1 = false;
    bool arg2 = false;
    int length;

    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    if(luaL_getmetafield(L, 1, "__type"))
    {
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != vector_pid)
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
        if(chimeraSystem->getTypeSystem()->getParameterBase(lua_tointeger(L, -1)) != vector_pid)
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
#warning LOG (kekstoaster#1#): no valid vector supplied
        //LoggingSystem::Error("no valid vector supplied");
        return false;
    }

    struct chimera::simulation::T_VectorDef* v1 = nullptr;
    if(!arg1)
    {
        v1 = *((struct chimera::simulation::T_VectorDef**)lua_touserdata(L, 1));
        length = v1->length;
    }

    struct chimera::simulation::T_VectorDef* v2 = nullptr;
    if(!arg2)
    {
        v2 = *((struct chimera::simulation::T_VectorDef**)lua_touserdata(L, 2));
        length = v2->length;
    }

    if((!arg1 && !arg2) && (v1->length != v2->length))
    {
        return false;
    }

    for(int i = 0; i < length; i++)
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
            lua_pcall(L, 2, 1, 0);
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
            lua_pcall(L, 2, 1, 0);
        }

        lua_arith(L, op);

        if(!i) // i == 0
        {
            if(lua_vector_newempty(L, length))
            {
                lua_insert(L, 3);
            }
            else
            {
#warning LOG (kekstoaster#1#): result vector could not be created
                //LoggingSystem::Error("result vector could not be created");
                return false;
            }
        }

        if(luaL_getmetafield(L, 3, "__set") == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 3);
            lua_pushinteger(L, i);
            lua_rotate(L, -4, -1);
            lua_pcall(L, 3, 0, 0);
        }
        else
        {
#warning LOG (kekstoaster#1#): result vector could not be created
            //LoggingSystem::Error("result vector has no set function");
            return false;
        }
    }
    return true;
}

int chimera::runtime::types::lua_vector_add(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPADD))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_sub(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPSUB))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_mul(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPMUL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_div(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPDIV))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_mod(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPMOD))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_pow(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPPOW))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_unm(lua_State* const L)
{
    if(lua_vector_arith1(L, LUA_OPUNM))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_idiv(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPIDIV))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_band(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPBAND))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_bor(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPBOR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_bxor(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPBXOR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_bnot(lua_State* const L)
{
    if(lua_vector_arith1(L, LUA_OPBNOT))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_shl(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPSHL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_shr(lua_State* const L)
{
    if(lua_vector_arith2(L, LUA_OPSHR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int chimera::runtime::types::lua_vector_get(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        struct chimera::simulation::T_VectorDef* v = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, lua_upvalueindex(1)));
        int isnum;
        int i = lua_tointegerx(L, 1, &isnum);
        if(!isnum)
        {
#warning LOG (kekstoaster#1#): argument `1` is not a valid index.
            //LoggingSystem::Error("argument `1` is not a valid index.");
            return 0;
        }
        if(i >= 0 && i < v->length)
        {
            if(luaL_getmetafield(L, lua_upvalueindex(1), "__get") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, lua_upvalueindex(1));
                lua_pushinteger(L, i);
                lua_pcall(L, 2, 1, 0);
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
#warning LOG (kekstoaster#1#): Index out of bounds in vector get
            //LoggingSystem::Error("Index out of bounds in vector get");
        }
    }
    return 0;
}

int chimera::runtime::types::lua_vector_set(lua_State* const L)
{
    if(lua_gettop(L) == 2)
    {
        struct chimera::simulation::T_VectorDef* v = *((struct chimera::simulation::T_VectorDef **)lua_touserdata(L, lua_upvalueindex(1)));
        int isnum;
        int i = lua_tointegerx(L, 1, &isnum);
        if(!isnum)
        {
#warning LOG (kekstoaster#1#): argument `1` is not a valid index.
            //LoggingSystem::Error("argument `1` is not a valid index.");
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
        if(i >= 0 && i < v->length)
        {
            if(luaL_getmetafield(L, lua_upvalueindex(1), "__set") == LUA_TFUNCTION)
            {
                lua_pushvalue(L, lua_upvalueindex(1));
                lua_pushinteger(L, i);
                lua_pushvalue(L, 2);
                lua_pcall(L, 3, 0, 0);
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
#warning LOG (kekstoaster#1#): Index out of bounds in vector get
            //LoggingSystem::Error("Index out of bounds in vector get");
        }
    }
    return 0;
}

int chimera::runtime::types::lua_vector_all(lua_State* const L)
{

    return 0;
}

int chimera::runtime::types::lua_vector_any(lua_State* const L)
{

    return 0;
}

int chimera::runtime::types::lua_vector_foreach(lua_State* const L)
{

    return 0;
}

int chimera::runtime::types::lua_vector_copy(lua_State* const L)
{

    return 0;
}
