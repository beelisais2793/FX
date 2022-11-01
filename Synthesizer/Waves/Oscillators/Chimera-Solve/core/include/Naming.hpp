#ifndef NAMING_HPP_INCLUDED
#define NAMING_HPP_INCLUDED

namespace chimera {
    namespace typenames
    {
        const char * const TYPE_NIL = "nil";
        const char * const TYPE_BOOLEAN = "boolean";
        const char * const TYPE_NUMBER = "real";
        const char * const TYPE_STRING = "string";
        const char * const TYPE_TABLE = "table";
        const char * const TYPE_FUNCTION = "function";
        const char * const TYPE_POINTER = "pointer";
        const char * const TYPE_USERFUNCTION = "LuaFunction";
        const char * const TYPE_LIBRARY = "Library";
        const char * const TYPE_MODULE = "Module";
        const char * const TYPE_ENTRYPOINT = "EntryPoint";
        const char * const TYPE_INSTANCE = "Instance";
    }
    namespace luanames
    {
        const char * const LUA_NAME_ENTRYPOINT = "EntryPoint";
        const char * const LUA_NAME_MODULE = "Module";
        const char * const LUA_NAME_INSTANCE = "Instance";
    }
    namespace registrynames
    {
        const char * const LUA_REGISTRY_CHIMERA_FUNCTIONS = "chimera:reg:luafunctions";
        const char * const LUA_REGISTRY_CHIMERA_REFERENCES = "chimera:reg:references";
        const char * const LUA_REGISTRY_CHIMERA_SYSTEMNAMES = "chimera:reg:systemnames";
        const char * const LUA_REGISTRY_CHIMERA_METATABLES = "chimera:reg:metatables";
    }
}

#endif // NAMING_HPP_INCLUDED
