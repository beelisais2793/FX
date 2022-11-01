#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lua.hpp"

#include "def.hpp"
#include "StateSynchrony.hpp"
#include "Naming.hpp"
#include "types/lua_basetypes.hpp"
//#include "interfaces/ILogger.hpp"
//#include "LoggingSystem.hpp"
#include "ParameterType.hpp"
#include "ParameterValue.hpp"
#include "ParameterTypeSystem.hpp"
#include "types/LuaFunctionWrapper.hpp"
//#include "ParameterValueCollection.hpp"
//#include "Module.hpp"
//#include "EntryPoint.hpp"
//#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ChimeraContext.hpp"

chimera::ParameterTypeSystem::ParameterTypeSystem(ChimeraSystem* chimeraSystem):
    _lastID(7),
    _typeList(new std::vector<chimera::ParameterType*>(8)),
    _references(new std::unordered_map<void*, struct chimera::ParameterValueData* >()),
    _dependencies(new std::unordered_map<void*, std::unordered_set<void*> >()),
    _reverseDependencies(new std::unordered_map<void*, std::unordered_set<void*> >())
{
    (*_typeList)[chimera::systemtypes::PID_NIL] = new ParameterType(chimera::typenames::TYPE_NIL, chimera::systemtypes::PID_NIL, this, {nullptr, chimera::luat_nil_push, nullptr, nullptr});
    (*_typeList)[chimera::systemtypes::PID_NUMBER] = new ParameterType(chimera::typenames::TYPE_NUMBER, chimera::systemtypes::PID_NUMBER, this, {nullptr, chimera::luat_real_push, chimera::luat_real_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_BOOLEAN] = new ParameterType(chimera::typenames::TYPE_BOOLEAN, chimera::systemtypes::PID_BOOLEAN, this, {nullptr, chimera::luat_boolean_push, chimera::luat_boolean_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_STRING] = new ParameterType(chimera::typenames::TYPE_STRING, chimera::systemtypes::PID_STRING, this, {nullptr, chimera::luat_string_push, chimera::luat_string_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_TABLE] = new ParameterType(chimera::typenames::TYPE_TABLE, chimera::systemtypes::PID_TABLE, this, {nullptr, chimera::luat_table_push, chimera::luat_table_delete, nullptr});
    (*_typeList)[chimera::systemtypes::PID_POINTER] = new ParameterType(chimera::typenames::TYPE_POINTER, chimera::systemtypes::PID_POINTER, this, {nullptr, chimera::luat_pointer_push, nullptr, nullptr});
    (*_typeList)[chimera::systemtypes::PID_FUNCTION] = new ParameterType(chimera::typenames::TYPE_FUNCTION, chimera::systemtypes::PID_FUNCTION, this, {nullptr, chimera::luat_function_push, nullptr, nullptr});
    (*_typeList)[chimera::systemtypes::PID_LIBRARAY] = new ParameterType(chimera::typenames::TYPE_LIBRARY, chimera::systemtypes::PID_LIBRARAY, this, {nullptr, nullptr, nullptr, nullptr});
    registerParameter(chimera::typenames::TYPE_USERFUNCTION, this, {chimera::luat_luafunction_init, chimera::luat_function_push, chimera::luat_luafunction_delete, nullptr});
    registerParameter(chimera::typenames::TYPE_ENTRYPOINT, this, {nullptr, nullptr, nullptr, nullptr});
    registerParameter(chimera::typenames::TYPE_MODULE, this, {nullptr, nullptr, nullptr, nullptr});
    registerParameter(chimera::typenames::TYPE_INSTANCE, this, {nullptr, nullptr, nullptr, nullptr});
    _context = chimeraSystem->createContext(this);
    stateLoaded(chimeraSystem);
}

chimera::ParameterTypeSystem::~ParameterTypeSystem()
{
    for(int i = chimera::systemtypes::PID_INSTANCE + 1; i <= _lastID; i++)
    {
        removeParameter(i);
    }
    _references->clear();
    delete _typeList;
    delete _references;
    delete _dependencies;
    delete _reverseDependencies;
}

size_t chimera::ParameterTypeSystem::getParameterID(const std::string& name) const
{
    for(auto it = _typeList->begin(); it != _typeList->end(); it++)
    {
        if((*it) != nullptr && (*it)->getName() == name)
        {
            return (*it)->getID();
        }
    }
    return 0;
}

const std::string chimera::ParameterTypeSystem::getParameterName(const size_t& type) const
{
    if(type >= 0 && type < _typeList->size())
    {
        if((*_typeList)[type] != nullptr)
        {
            return (*_typeList)[type]->getName();
        }
    }
    return "";
}

const bool chimera::ParameterTypeSystem::exists(const size_t& type) const
{
    if(type >= 0 && type < _typeList->size())
    {
        const ParameterType* p = (*_typeList)[type];
        if(p != nullptr) {
            return true;
        }
    }
    return false;
}

size_t chimera::ParameterTypeSystem::getParameterBase(const size_t& type) const
{
    if(type >= 0 && type < _typeList->size())
    {
        if((*_typeList)[type] != nullptr)
        {
            return (*_typeList)[type]->getBase();
        }
    }
    return 0;
}

size_t chimera::ParameterTypeSystem::getParameterTag(const size_t& type) const
{
    if(type >= 0 && type < _typeList->size())
    {
        if((*_typeList)[type] != nullptr)
        {
            return (*_typeList)[type]->getTag();
        }
    }
    return 0;
}

chimera::ParameterValue chimera::ParameterTypeSystem::cast(size_t toType, const ParameterValue& param) const
{
    return cast(param.getType(), toType, param.getValue());
}

chimera::ParameterValue chimera::ParameterTypeSystem::cast(size_t fromType, size_t toType, void * const value) const
{
    ParameterType const * const to = getParameter(toType);
    if(to != nullptr)
    {
        fn_typecast fn = nullptr;
        auto cast = to->_castFuntions.find(fromType);
        if(cast != to->_castFuntions.end())
        {
            fn = cast->second;
        }
        else
        {
            cast = to->_castFuntions.find(0);
            if(cast != to->_castFuntions.end())
            {
                fn = cast->second;
            }
        }

        if(fn)
        {
            void* result = (*fn)(fromType, value);
            if(result != nullptr)
            {
                return ParameterValue(_context->_chimeraSystem, toType, result);
            }
        }
    }

    return ParameterValue(_context->_chimeraSystem, 0, nullptr);
}

bool chimera::ParameterTypeSystem::isReferenced(void* ptr) const
{
    auto it = _references->find(ptr);
    if(it != _references->end())
    {
        return it->second->type > 0;
    }
    return false;
}

chimera::ParameterValue chimera::ParameterTypeSystem::getValue(lua_State* const L, const int& index)
{
    switch(lua_type(L, index))
    {
        case LUA_TNONE:
        case LUA_TNIL:
        {
            return ParameterValue(chimera::systemtypes::PID_NIL, nullptr);
        }
        case LUA_TNUMBER:
        {
            return ParameterValue(_context->_chimeraSystem, chimera::systemtypes::PID_NUMBER, new double(lua_tonumber(L, index)));
        }
        case LUA_TBOOLEAN:
        {
            return ParameterValue(_context->_chimeraSystem, chimera::systemtypes::PID_BOOLEAN, new bool(lua_toboolean(L, index)));
        }
        case LUA_TSTRING:
        {
            return ParameterValue(_context->_chimeraSystem, chimera::systemtypes::PID_STRING, new std::string(lua_tostring(L, index)));
        }
        case LUA_TTABLE:
        {
            map_t_LuaItem* table = new map_t_LuaItem();
            int absIndex = lua_absindex(L, index);
            lua_pushnil(L);
            while (lua_next(L, absIndex) != 0)
            {
                switch(lua_type(L, -2))
                {
                    case LUA_TNUMBER:
                    {
                        int isnum = 0;
                        int intKey = lua_tointegerx(L, -2, &isnum);
                        if(isnum)
                        {
                            std::string key = std::to_string(intKey);
                            table->insert(std::make_pair(key, getValue(L, -1)));
                            //(*table)[key] = value;
                        }
                        break;
                    }
                    case LUA_TSTRING:
                    {
                        std::string key(lua_tostring(L, -2));
                        table->insert(std::make_pair(key, getValue(L, -1)));
                        //(*table)[key] = value;
                        break;
                    }
                }

                lua_pop(L, 1);
            }

            return ParameterValue(_context->_chimeraSystem, chimera::systemtypes::PID_TABLE, table);
        }
        case LUA_TFUNCTION:
        {
            int f_ind = lua_absindex(L, index);
            lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
            lua_rawget(L, LUA_REGISTRYINDEX);
            void* w = (void*)(new chimera::LuaFunctionWrapper(_context->_chimeraSystem));
            lua_pushvalue(L, f_ind);
            lua_rawsetp(L, -2, w);
            lua_pop(L, 1);

            return ParameterValue(_context->_chimeraSystem, chimera::systemtypes::PID_LUAFUNC, w);
        }
        case LUA_TUSERDATA:
        {
            int f_ind = lua_absindex(L, index);
            if(luaL_getmetafield(L, f_ind, "__type"))
            {
                int isnum, type;
                type = lua_tointegerx(L, -1, &isnum);
                lua_pop(L, 1);
                if(isnum)
                {
                    if(type > 0 && (unsigned)type >= chimera::systemtypes::PID_MODULE && (*_typeList)[type] != nullptr)
                    {
                        void* value = *((void**)lua_touserdata(L, f_ind));

                        ParameterValue pv(_context->_chimeraSystem, type, value);
                        addDependency(_context->_chimeraSystem->getLuaState(), value);

                        return pv;
                    }
                    else
                    {
                        return ParameterValue(type, nullptr);
                    }
                }
            }
            break;
        }
    case LUA_TTHREAD:
        {

            break;
        }
    case LUA_TLIGHTUSERDATA:
        {
            return ParameterValue(_context->_chimeraSystem, chimera::systemtypes::PID_POINTER, lua_touserdata(L, index));
            break;
        }
    }

    return ParameterValue(chimera::systemtypes::PID_NIL, nullptr);
}

size_t chimera::ParameterTypeSystem::getParameterType(lua_State* const L, const int& index) const
{
    return ParameterType::getType(L, index);
}

bool chimera::ParameterTypeSystem::pushValue(lua_State* const L, size_t type, void* value)
{
    switch(type)
    {
        case chimera::systemtypes::PID_NIL:
        case chimera::systemtypes::PID_NUMBER:
        case chimera::systemtypes::PID_BOOLEAN:
        case chimera::systemtypes::PID_STRING:
        case chimera::systemtypes::PID_TABLE:
        case chimera::systemtypes::PID_FUNCTION:
        case chimera::systemtypes::PID_POINTER:
        {
            (*_typeList)[type]->pushValue(L, value);
            break;
        }
        case chimera::systemtypes::PID_LUAFUNC:
        {
            LuaFunctionWrapper* fw = (LuaFunctionWrapper*)value;
            if(fw->intern())
            {
                (*_typeList)[type]->pushValue(L, (void*)fw->_origin);

                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_rawgeti(L, -1, type);
                lua_setmetatable(L, -2);
                lua_pop(L, 1);
                addDependency(fw->_chimeraSystem->getLuaState(), (void*)fw->_origin);
            }
            else
            {
                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_FUNCTIONS);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_pushlightuserdata(L, (void*)fw->_origin);
                lua_rawget(L, -2);
                lua_remove(L, -2);
            }

            break;
        }
        default:
        {
            if(type > 0 && type < _typeList->size())
            {
                if(type < chimera::systemtypes::PID_INSTANCE)
                {
                    lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_REFERENCES);
                    lua_rawget(L, LUA_REGISTRYINDEX);
                    if(lua_rawgetp(L, -1, value))
                    {
                        lua_remove(L, -2);
                        return true;
                    }
                    lua_pop(L, 1);
                }
                createReference(type, value);
                addDependency(_context->_chimeraSystem->getLuaState(), value);
                (*_typeList)[type]->pushValue(L, value);
                lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_METATABLES);
                lua_rawget(L, LUA_REGISTRYINDEX);
                lua_rawgeti(L, -1, type);
                lua_setmetatable(L, -3);
                lua_pop(L, 1);
            }
            else
            {
                return false;
            }
            break;
        }
    }
    return true;
}

void chimera::ParameterTypeSystem::addDependency(void* item, void* dependency)
{
    {
        if (!_dependencies->count(item))
        {
            _dependencies->insert(std::make_pair(item, std::unordered_set<void*>()));
        }

        auto listItem = _dependencies->find(item);
        if(listItem != _dependencies->end())
        {
            if(!listItem->second.count(dependency))
            {
                listItem->second.insert(dependency);
            }
        }
    }

    {
        if (!_reverseDependencies->count(dependency))
        {
            _reverseDependencies->insert(std::make_pair(dependency, std::unordered_set<void*>()));
        }

        auto listItem = _reverseDependencies->find(dependency);
        if(listItem != _reverseDependencies->end())
        {
            if(!listItem->second.count(item))
            {
                listItem->second.insert(item);
            }
        }
    }
}

void chimera::ParameterTypeSystem::releaseDependency(void* item, void* dependency)
{
    auto listItem = _dependencies->find(item);
    auto listDep = _reverseDependencies->find(dependency);
    if (listItem != _dependencies->end() && listDep != _reverseDependencies->end())
    {
        listItem->second.erase(dependency);
        listDep->second.erase(item);

        if (listDep->second.size() == 0)
        {
            _reverseDependencies->erase(listDep);
            removeDependencyItem(dependency);
            disposeReference(dependency);
        }

        if (listItem->second.size() == 0)
        {
            _dependencies->erase(listItem);
        }
    }
}

void chimera::ParameterTypeSystem::removeDependencyItem(void* item)
{
    //static void* dependency;
    std::unordered_set<void*> itemCache;

    auto depList = _dependencies->find(item);
    if(depList != _dependencies->end())
    {
        for(auto it = depList->second.begin(); it != depList->second.end(); it++)
        {
            auto itemList = _reverseDependencies->find(*it);
            if (itemList != _reverseDependencies->end())
            {
                itemList->second.erase(item);
                if (itemList->second.size() == 0)
                {
                    _reverseDependencies->erase(itemList);
                    // delete reference
                    itemCache.insert(*it);
                }
            }
        }
        _dependencies->erase(depList);

        for(auto it = itemCache.begin(); it != itemCache.end(); it++)
        {
            removeDependencyItem(*it);
            disposeReference(*it);
        }
    }
}

void chimera::ParameterTypeSystem::deleteValue(lua_State* const L, size_t type, void* value) const
{
    if(type >= 0 && type < _typeList->size())
    {
        (*_typeList)[type]->deleteValue(L, value);
    }
}

chimera::ParameterType const * const chimera::ParameterTypeSystem::getParameter(const size_t& type) const
{
    if(type >= 0 && type < _typeList->size())
    {
        if((*_typeList)[type] != nullptr)
        {
            return (*_typeList)[type];
        }
    }
    return nullptr;
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct chimera::T_ParameterDef& pdef)
{
    return registerParameter(name, origin, pdef, 0, 0, std::unordered_map<std::string, size_t>(), true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag)
{
    return registerParameter(name, origin, pdef, base, tag, std::unordered_map<std::string, size_t>(), true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct chimera::T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& attributes)
{
    return registerParameter(name, origin, pdef, 0, 0, attributes, true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& attributes)
{
    return registerParameter(name, origin, pdef, base, tag, attributes, true);
}

size_t chimera::ParameterTypeSystem::registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& attributes, bool notify)
{
    for(auto it = _typeList->begin(); it != _typeList->end(); it++)
    {
        if((*it) != nullptr && (*it)->getName() == name)
        {
            return 0;
        }
    }
    ++_lastID;
    base = base < _lastID ? base : 0;
    base = base > chimera::systemtypes::PID_MODULE ? base : 0;
    ParameterType* p = new ParameterType(name, _lastID, origin, pdef, base, tag, attributes);
    _typeList->push_back(p);
    if(notify)
    {
        itemAdded(p, nullptr);
    }
    return _lastID;
}

void chimera::ParameterTypeSystem::removeParameter(const std::string& name)
{
    removeParameter(getParameterID(name));
}

void chimera::ParameterTypeSystem::removeParameter(const size_t& type)
{
    if(type > chimera::systemtypes::PID_INSTANCE && type < _typeList->size())
    {
        ParameterType* p = (*_typeList)[type];
        if(p != nullptr)
        {
            itemRemoved(p, nullptr);
            deleteAllReferences(type);
            p->dispose(_context->_chimeraSystem->getLuaState());
            delete p;
            (*_typeList)[type] = nullptr;
        }
    }
}

void chimera::ParameterTypeSystem::removeParameter(void const * const origin)
{
    ParameterType* p = nullptr;
    for(size_t id = chimera::systemtypes::PID_INSTANCE + 1; id < _typeList->size(); id++)
    {
        p = (*_typeList)[id];
        if(p != nullptr && p->_origin == origin)
        {
            deleteAllReferences(id);
            itemRemoved(p, nullptr);
            delete p;
            (*_typeList)[id] = nullptr;
        }
    }
}

void chimera::ParameterTypeSystem::deleteAllReferences(size_t type)
{
    std::unordered_set<void*> deleteValues;
    for(auto it = _references->begin(); it != _references->end(); it++)
    {
        if(it->second->type == type)
        {
            deleteValues.insert(it->first);
            deleteValue(_context->_chimeraSystem->getLuaState(), it->second->type, it->second->value);
            it->second->type = 0;
        }
    }

    for(auto it = deleteValues.begin(); it != deleteValues.end(); it++)
    {
        _references->erase(*it);
    }
}

void chimera::ParameterTypeSystem::createReference(size_t type, void* value)
{
    auto refValue = _references->find(value);

    if(refValue == _references->end())
    {
        struct ParameterValueData* data = new ParameterValueData({
            _context->_chimeraSystem,
            type,
            value,
            0
        });
        _references->insert(
            std::make_pair(value, data)
        );
    }
}

void chimera::ParameterTypeSystem::disposeReference(void* value)
{
    auto refItem = _references->find(value);
    if(refItem != _references->end())
    {
        if(refItem->second->references == 0 && !_reverseDependencies->count(value))
        {
            removeDependencyItem(value);
            deleteValue(_context->_chimeraSystem->getLuaState(), refItem->second->type, refItem->second->value);
            _references->erase(value);
        }
    }
}

const std::string chimera::ParameterTypeSystem::getGUID() const
{
    return "ParameterTypeSystem";
}
