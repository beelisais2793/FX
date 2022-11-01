#include <iostream>
#include <unordered_map>
#include <vector>
#include "lua.hpp"

#include "def.hpp"
#include "ParameterType.hpp"

chimera::ParameterType::ParameterType():
    _id(-1),
    _name(""),
    _origin(nullptr),
    _initialize(nullptr),
    _pushValue(nullptr),
    _deleteValue(nullptr),
    _dispose(nullptr),
    _base(0),
    _tag(0)
{
    //ctor
}

chimera::ParameterType::ParameterType(const std::string& name, const int& id, void const * const origin, const struct chimera::T_ParameterDef& pdef):
    _id(id),
    _name(name),
    _origin(origin),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _dispose(pdef.dispose),
    _base(0),
    _tag(0)
{
    //ctor
}

chimera::ParameterType::ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, const size_t& base, const size_t& tag):
    _id(id),
    _name(name),
    _origin(origin),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _dispose(pdef.dispose),
    _base(base),
    _tag(tag)
{
    //ctor
}

chimera::ParameterType::ParameterType(const std::string& name, const int& id, void const * const origin, const struct chimera::T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& attributes):
    _id(id),
    _name(name),
    _origin(origin),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _dispose(pdef.dispose),
    _base(0),
    _tag(0),
    _attributes(attributes)
{
    //ctor
}

chimera::ParameterType::ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, const size_t& base, const size_t& tag, const std::unordered_map<std::string, size_t>& attributes):
    _id(id),
    _name(name),
    _origin(origin),
    _initialize(pdef.initialize),
    _pushValue(pdef.pushValue),
    _deleteValue(pdef.deleteValue),
    _dispose(pdef.dispose),
    _base(base),
    _tag(tag),
    _attributes(attributes)
{
    //ctor
}

chimera::ParameterType::~ParameterType()
{

}

size_t chimera::ParameterType::getID() const
{
    return _id;
}

const std::string chimera::ParameterType::getName() const
{
    return _name;
}

void const * const chimera::ParameterType::getOrigin() const
{
    return _origin;
}

size_t chimera::ParameterType::getTag() const
{
    return _tag;
}

size_t chimera::ParameterType::getBase() const
{
    return _base;
}

void chimera::ParameterType::initialize(lua_State * const L) const
{
    initialize(L, _id);
}

void chimera::ParameterType::initialize(lua_State * const L, const int& id) const
{
    if(_initialize)
    {
        // calls initialize function with 2 parameters
        lua_pushcfunction(L, _initialize);
        // 1. table that will be used as meta table. If first type in the hirarchy, table will be empty. for derived types this will contain meta data from base types.
        lua_insert(L, -2);
        // 2. type ID
        lua_pushinteger(L, id);
        // expected to return a table, either the same as parameter 1, or a new one, that will be used as meta table.
        lua_call(L, 2, 1);
    }
}

void chimera::ParameterType::pushValue(lua_State * const L, void * const value) const
{
    if(_pushValue)
    {
        // calls pushValue with 2 parameters
        lua_pushcfunction(L, _pushValue);
        // 1. parameter ID
        lua_pushinteger(L, _id);
        // 2. pointer to the object
        lua_pushlightuserdata(L, value);
        // returns the new userdata for the value pointer. This function does net need to assign the meta table, as it will be overwritten by the one created for the type.
        lua_call(L, 2, 1);
    }
}

void chimera::ParameterType::deleteValue(lua_State* const L, void * const value) const
{
    if(_deleteValue)
    {
        // calls deleteValue with 2 parameters
        lua_pushcfunction(L, _deleteValue);
        // 1. parameter ID
        lua_pushinteger(L, _id);
        // 2. pointer to the object
        lua_pushlightuserdata(L, value);
        // returns nothing, but has to delete the object at given pointer
        lua_call(L, 2, 0);
    }
}

size_t chimera::ParameterType::getAttribute(const std::string& attr) const
{
    if (_attributes.size() > 0)
    {
        auto attrValue = _attributes.find(attr);
        if (attrValue != _attributes.end())
        {
            return attrValue->second;
        }
    }
    return 0;
}

void chimera::ParameterType::addCast(const size_t& fromType, fn_typecast cast)
{
    _castFuntions[fromType] = cast;
}

void chimera::ParameterType::dispose(lua_State* const L) const
{
    if(_dispose)
    {
        // calls dispose function with 2 parameters
        lua_pushcfunction(L, _dispose);
        // 1. the pointer to this parameter type
        lua_pushlightuserdata(L, (void*)this);
        // 2. the tag value converted to a void pointer
        lua_pushlightuserdata(L, (void*)_tag);
        // Returns nothing but is responsible for clean up of the type
        lua_call(L, 2, 0);
    }
}

size_t chimera::ParameterType::getType(lua_State* const L, const int& index)
{
    switch(lua_type(L, index))
    {
        case LUA_TNONE:
        case LUA_TNIL:
        {
            return chimera::systemtypes::PID_NIL;
        }
        case LUA_TBOOLEAN:
        {
            return chimera::systemtypes::PID_BOOLEAN;
        }
        case LUA_TNUMBER:
        {
            return chimera::systemtypes::PID_NUMBER;
        }
        case LUA_TSTRING:
        {
            return chimera::systemtypes::PID_STRING;
        }
        case LUA_TTABLE:
        {
            return chimera::systemtypes::PID_TABLE;
        }
        case LUA_TFUNCTION:
        {
            return chimera::systemtypes::PID_FUNCTION;
        }
        case LUA_TUSERDATA:
        {
            if(luaL_getmetafield(L, index, "__type"))
            {
                int isnum, type;
                type = lua_tointegerx(L, -1, &isnum);
                lua_pop(L, 1);
                if(isnum)
                {
                    return type;
                }
            }
        }
    }

    return chimera::systemtypes::PID_NIL;
}
