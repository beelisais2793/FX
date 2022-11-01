#ifndef PARAMETERTYPE_H
#define PARAMETERTYPE_H

typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State *L);

namespace chimera
{
    class ChimeraSystem;
    /**
     * Struct to store all function pointers required for a type, to reduce parameter count
     */
    struct T_ParameterDef
    {
        /// Initialize the ParameterType (creating meta table, etc) this call is chained with all base types assoziated with this type.
        lua_CFunction initialize;
        /// Create a new reference value and push a userdata on the lua stack. This can be seen as a constructor in object oriented prgramming.
        lua_CFunction pushValue;
        /// Destroy the reference; called if the lua garbage collection releases the object, there are no more references created with ParameterValue objects and if all dependencies are released. This can be seen as a destructor in object oriented programming.
        lua_CFunction deleteValue;
        /// Dispose the type itself; called when the type gets unloaded. This is needed if a context object is created and its reference passed to the type with the tag parameter.
        lua_CFunction dispose;
    };

    /**
     * Stores all data required to identfy an Object for the use in Lua.
     * Every type has a unique ID and a unique name. IDs will be assigned
     * automatically by the system; names however will always be the same
     * and should be used to look up a type.
     * Parameters can have a base class. If they do, all initialize functions
     * of all base types will be called with the new ID in revers order.
     *
     * @note ParameterTypes cannot be created explicitly but only through registerParameter of the used ParameterTypeSystem. This assures that the ID will be unique and not be reused if a type is removed.
     */
    class ParameterType final
    {
        friend class ParameterTypeSystem;
        friend class ChimeraSystem;

        public:
            virtual ~ParameterType();
            /**
             * Get the unique ID of this type
             *
             * @return Returns the Parameter ID.
             */
            size_t getID() const;
            /**
             * Get the assigned name of the type. This name is used to look up types.
             *
             * @return Returns the unique name of the type.
             */
            const std::string getName() const;
            /**
             * Get the base type if this is a derived type. If this class was not derived
             * from another type it willl return 0.
             *
             * @return Returns the type ID of the base class
             */
            size_t getBase() const;
            /**
             * Get the tag value for the type. A tag is an arbitrary value that can be assigned
             * to a type and can be any positive integer. It is up to the user of the type
             * to make proper use of it, as it can be another type ID, a pointer to a data
             * object, function pointer, etc...
             *
             * @note This can be used to create similar behavior as generic or template data types with a single generic parameter in object oriented programming.
             *
             * @return Returns the tag value.
             */
            size_t getTag() const;
            /**
             * Returns the value of any assigned attribute. Although very similar to tag value,
             * using attributes requires an explicit name. Unlike tag values, attributes are used
             * internally as well. Therefore check the documentation for a specific attribute
             * as it might be interpreted by the system.
             *
             * @note Attributes were introduced as an equivalent to interfaces in object oriented programming and the existing interfaces indicate a C++ base class the object can be cased to.
             *
             * @param attr An attribute name to check for.
             *
             * @return Returns the value of the attribute if it was assigned and 0 otherwise.
             */
            size_t getAttribute(const std::string& attr) const;
            /**
             * Adds an explicit cast to this type. Cast functions should always either return
             * a new independent object of the new type or add the origial object as a
             * dependency.
             *
             * @param fromType The type ID of the original object
             * @param cast A function pointer to a cast function. The function takes type and value as its parameters and returns a pointer to the newly casted object.
             */
            void addCast(const size_t& fromType, fn_typecast cast);
        protected:
        private:
            /**
             * Creates an invalid parameter type.
             */
            ParameterType();
            /**
             * Creates a new ParameterType.
             *
             * @param name The unique name of the type.
             * @param id The unique type ID of the type.
             * @param origin If a type is created by loaded modules, origin must be set to the object that can be unloaded. This links types to dynamic code.
             * @param pdef A struct containing all required methods. Note that you should always add at least both deleteValue and pushValue.
             */
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef);
            /**
             * Creates a new ParameterType.
             *
             * @param name The unique name of the type.
             * @param id The unique type ID of the type.
             * @param origin If a type is created by loaded modules, origin must be set to the object that can be unloaded. This links types to dynamic code.
             * @param pdef A struct containing all required methods. Note that you should always add at least both deleteValue and pushValue.
             * @param base The type ID of base type. Note that this ID must always be less that the current ID.
             * @param tag An arbitrary data field that can be used to store any unsigned integer or pointer.
             */
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, const size_t& base, const size_t& tag);
            /**
             * Creates a new ParameterType.
             *
             * @param name The unique name of the type.
             * @param id The unique type ID of the type.
             * @param origin If a type is created by loaded modules, origin must be set to the object that can be unloaded. This links types to dynamic code.
             * @param pdef A struct containing all required methods. Note that you should always add at least both deleteValue and pushValue.
             * @param attributes A key-value map containing all implemented attributes with their corresponding value.
             */
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& attributes);
            /**
             * Creates a new ParameterType.
             *
             * @param name The unique name of the type.
             * @param id The unique type ID of the type.
             * @param origin If a type is created by loaded modules, origin must be set to the object that can be unloaded. This links types to dynamic code.
             * @param pdef A struct containing all required methods. Note that you should always add at least both deleteValue and pushValue.
             * @param base The type ID of base type. Note that this ID must always be less that the current ID.
             * @param tag An arbitrary data field that can be used to store any unsigned integer or pointer.
             * @param attributes A key-value map containing all implemented attributes with their corresponding value.
             */
            ParameterType(const std::string& name, const int& id, void const * const origin, const struct T_ParameterDef& pdef, const size_t& base, const size_t& tag, const std::unordered_map<std::string, size_t>& attributes);
            /**
             * Calls the initialize function with the current type ID if the function exists.
             * The function is called as a seperate Lua function call and has its own execution stack.
             *
             * @param L The Lua context to use.
             */
            void initialize(lua_State * const L) const;
            /**
             * Calls the initialize function with the given type ID if the function exists.
             * The function is called as a seperate Lua function call and has its own execution stack.
             *
             * @note The type ID does not need to be the assigned ID. If a class is derived from a type this function will be called with the new type ID.
             *
             * @param L The Lua context to use.
             * @param id The type ID of the new type to initialize.
             */
            void initialize(lua_State * const L, const int& id) const;
            /**
             * Calls the pushValue function if the function exists.
             * It should always return 1 (the value to push).
             * This will push the given value onto the lua stack and assign the proper meta table.
             * The function is called as a seperate Lua function call and has its own execution stack.
             *
             * @param L The Lua context to use.
             * @param value The pointer to the object. It is up to the function to cast it to the correct class if needed.
             */
            void pushValue(lua_State * const L, void * const value) const;
            /**
             * Calls the deleteValue function if the function exists.
             * Releases the reference and all used resources of an object.
             *
             * @param L The Lua context to use.
             * @param value The pointer to the object. It is up to the function to cast it to the correct class if needed.
             */
            void deleteValue(lua_State* const L, void * const value) const;
            /**
             * Gets the type of a Lua object at the given stack index.
             *
             * @param L The Lua context to use.
             * @param index A valid stack index of the given Lua context.
             *
             * @return Returns the type ID of the object at the stack index. If the type cannot be checked it will return 0.
             */
            static size_t getType(lua_State* const L, const int& index);
            /**
             * Calls the dispose function if the function exists.
             * This function is responsible to release all resouces held by the
             * ParameterType itself.
             *
             * @param L The Lua context to use.
             */
            void dispose(lua_State* const L) const;
            /**
             * Get the origin of the type. If the type was not created though a module, origin will be nullptr.
             *
             * @return Returns the pointer to the origin object that created this type.
             */
            void const * const getOrigin() const;
            /// The unique type ID.
            const size_t _id;
            /// The unique type name.
            const std::string _name;
            /// A pointer to the origin object that created this type.
            void const * const _origin;
            /// A function pointer used to initialize the type.
            const lua_CFunction _initialize;
            /// A function pointer used to push object onto the Lua stack.
            const lua_CFunction _pushValue;
            /// A function pointer used to delete objects that are no longer used by the system.
            const lua_CFunction _deleteValue;
            /// A function pointer used to release all resources used by the type itself.
            const lua_CFunction _dispose;
            /// The type ID of the base type.
            const size_t _base;
            /// An arbitrary tag value.
            const size_t _tag;
            /// A map of possible cast functions that will cast other objects of some type to this type.
            std::unordered_map<size_t, fn_typecast> _castFuntions;
            /// A key-value map listing all attributes with their assigned value.
            std::unordered_map<std::string, size_t> _attributes;
    };
}

#endif // PARAMETERTYPE_H
