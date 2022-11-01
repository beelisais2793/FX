#ifndef PARAMETERTYPESYSTEM_H
#define PARAMETERTYPESYSTEM_H

typedef int (*lua_CFunction) (lua_State *L);

namespace chimera {
    class ChimeraSystem;
    class LuaFunctionWrapper;
    class ParameterReferenceString;
    class ParameterReferenceInteger;
    class ParameterType;
    class ParameterValue;

    /**
     * Managemant class to create and register all ParameterTypes that are used within the Lua context.
     * A ParameterTypeSystem requires an existing ChimeraSystem and is only used to seperate
     * the class of methods that deal with types and values and helps to keep the ChimeraSystem
     * class on a smaller size.
     */
    class ParameterTypeSystem:
        public StateSynchrony
    {
        friend class ChimeraSystem;
        friend class ChimeraContext;
        friend class LuaFunctionWrapper;
        friend class ParameterValue;
        friend class ParameterReferenceString;
        friend class ParameterReferenceInteger;

        friend int chimera::lua_UserData_gc(lua_State* L);
        friend int chimera::lua_Module_call(lua_State* L);
        friend int chimera::lua_Instance_forwardMethod(lua_State* L);
        friend int chimera::luat_luafunction_call(lua_State* L);
        friend int chimera::luat_table_push(lua_State* const L);
        friend std::ostream& operator<<(std::ostream& os, const ParameterValue& obj);

        public:
            virtual ~ParameterTypeSystem();
            /**
             * Gets the type ID of a type called name.
             *
             * @param name The name of a registered type.
             *
             * @return Returns the type ID if the name was found and 0 otherwise.
             */
            size_t getParameterID(const std::string& name) const;
            /**
             * Gets the name of a type that corresponds to the given ID.
             *
             * @param type The type ID to look up.
             *
             * @return Returns the name of the type if the ID is valid and the type still exists. Returns an empty string otherwise.
             */
            const std::string getParameterName(const size_t& type) const;
            /**
             * Checks if a type is preset for a given type ID. If the id is outside the rage
             * of the type array or if the type was deleted the result will be false.
             *
             * @param type The type id the check for.
             * @return true if the type exists, false otherwise.
             */
            const bool exists(const size_t& type) const;
            /**
             * Gets the base type of an existing type with given type ID.
             *
             * @param type The type ID to check for a parent type.
             *
             * @return Returns the base type for a given type or 0 if this type has no base type. Also returns 0 if the type ID is invalid.
             */
            size_t getParameterBase(const size_t& type) const;
            /**
             * Gets the tag value of an existing type with given type ID.
             *
             * @param type The type ID to check for a tag value.
             *
             * @return Returns the tag value for a given type or 0 if the type ID is invalid.
             */
            size_t getParameterTag(const size_t& type) const;
            /**
             * This function will attempt to cast value of a certain type to another type and
             * return the new object. If no such cast exists, or the cast cannot be done,
             * this method will return nullptr.
             *
             * @param fromType The type ID for the type of value
             * @param toType The new type ID that is attempted to cast to.
             * @param value The pointer to the object that should be casted.
             *
             * @return Returns a ParameterValue holding the type and a pointer to a new object created if the cast was successful and a ParameterValue of type 0 otherwise.
             */
            ParameterValue cast(size_t fromType, size_t toType, void * const value) const;
            /**
             * This function will attempt to cast value of a certain type to another type and
             * return the new object. If no such cast exists, or the cast cannot be done,
             * this method will return nullptr.
             *
             * @param toType The new type ID that is attempted to cast to.
             * @param param The object that should be casted to a new type.
             *
             * @return Returns a ParameterValue holding the type and a pointer to a new object created if the cast was successful and a ParameterValue of type 0 otherwise.
             */
            ParameterValue cast(size_t toType, const ParameterValue& param) const;
            /**
             * Checks whether the pointer to a given object exists in a hashmap
             * containing all references to objects that are shared between C++ and
             * Lua context.
             *
             * @param value The pointer to an object to check for existence.
             *
             * @return Returns if the pointer already exists in the reference list.
             */
            bool isReferenced(void* value) const;
            /**
             * Gets the value object at stack index of the given Lua state.
             * Retrieving the value from Lua will add all required values
             * that are related to this value to references and dependencies
             * to prevent garbage collection if still used in C++ context.
             *
             * @param L The Lua context to use.
             * @param index A valid stack index of the given Lua context.
             *
             * @return Returns the value object at stack index of the given Lua state.
             */
            ParameterValue getValue(lua_State* const L, const int& index);
            /**
             * Gets the type of a Lua object at the given stack index.
             * This will not affect the references or any other states.
             *
             * @param L The Lua context to use.
             * @param index A valid stack index of the given Lua context.
             *
             * @return Returns the type ID of the object at the stack index. If the type cannot be checked it will return 0.
             */
            size_t getParameterType(lua_State* const L, const int& index) const;
            /**
             * Pushes a Lua object onto the Lua stack that is represented by type and value.
             * If it is a native type like string or number, converts it to the Lua representation
             * otherwise pushes a userdata object with the meta table of type.
             *
             * @note At this point you give away the ownership of the object. The System will take care of the deletion of the object. If you also need the object in your own context, you will have to add value as an dependency to your object as item.
             *
             * @param L The Lua context to use.
             * @param type The type ID of value
             * @param value A pointer to the value object.
             *
             * @return Return true if the value could be pushed on the Lua stack and false otherwise. If the result is false, the stack will be in the same state as before the call.
             */
            bool pushValue(lua_State* const L, size_t type, void* value);
            /**
             * Adds a dependency for a specific object. This prevents dependency from
             * being deleted through garbage collection until the dependency is released.
             *
             * @note it is not important if item also exists in the references list, but will still prevent the deletion.
             *
             * @param item An object that uses some other object that might be garbarge collected.
             * @param dependency An object obtained from the Lua context that is needed for item and must not be destroyed while item exists.
             */
            void addDependency(void* item, void* dependency);
            /**
             * Releases a previously added dependency relation.
             *
             * @note this will immediately destroy all items that were set as a dependency, if no items require it anymore and if it is also no longer needed within Lua.
             * @note This could cause a chain reaction if dependencies were also used as items that used dependencies.
             *
             * @param item An object that used some other object that might be garbarge collected.
             * @param dependency An object obtained from the Lua context that was needed for item and must not be destroyed while item existed.
             */
            void releaseDependency(void* item, void* dependency);
            /**
             * This will release all dependency relations that used item.
             *
             * @note this will immediately destroy all items that were set as a dependency, if no items require it anymore and if it is also no longer needed within Lua.
             * @note This could cause a chain reaction if dependencies were also used as items that used dependencies.
             *
             * @param item An object that used some other objects that might be garbarge collected.
             */
            void removeDependencyItem(void* item);
            /**
             * Returns the name of this class.
             *
             * @return Returns the name of this class.
             */
            virtual const std::string getGUID() const override;
        protected:
            /**
             * Creates a new ParameterTypeSystem.
             * This class was used to split type and value functionality from
             * ChimeraSystem to keep classes at a smaller size.
             *
             * @param chimeraSystem The parent ChimeraSystem that created this subsystem
             */
            ParameterTypeSystem(ChimeraSystem* chimeraSystem);
            /**
             * Deletes an object that is represented by type and value.
             * This function should only be called if it was deleted from the Lua context
             * (meaning that its Lua garbage collector meta function was called for this object)
             * and if it is not referenced and needed anymore by any other object.
             *
             * @param L The Lua context to use.
             * @param type The type ID of value
             * @param value A pointer to the value object.
             */
            void deleteValue(lua_State* const L, size_t type, void* value) const;
            /**
             * Gets the type object for a given type ID.
             *
             * @param type The type ID.
             *
             * @return Returns the pointer to a Parameter object for a given ID. If the type was deleted of if the type ID is invalid returns nullptr.
             */
            ParameterType const * const getParameter(const size_t& type) const;
            /**
             * Creates a new type in the system with the given unique name.
             * A type always has an origin indicating which object created the type, in case the creator gets unloaded.
             * A new type needs function pointers to handle creating and destroying the type itself and creating and destroying values for this type.
             * If a base type is given, this will call all initilization functions of all base types and this type in their respective order.
             * A tag can be any value and is for user purpose only, the system makes no use of this value.
             * If needed types can be registered with attributes indicating certain functionality.
             *
             * @param name The unique name of the new type
             * @param origin An object pointer representing the scope of the type. Most likely the ChimeraSystem itself or the Module that created the type.
             * @param pdef A list of functions that are needed t manage the type and its values. Note that not all functions required.
             *
             * @return Returns the type ID of the new type or 0 if the type could not be created.
             */
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef);
            /**
             * Creates a new type in the system with the given unique name.
             * A type always has an origin indicating which object created the type, in case the creator gets unloaded.
             * A new type needs function pointers to handle creating and destroying the type itself and creating and destroying values for this type.
             * If a base type is given, this will call all initilization functions of all base types and this type in their respective order.
             * A tag can be any value and is for user purpose only, the system makes no use of this value.
             * If needed types can be registered with attributes indicating certain functionality.
             *
             * @param name The unique name of the new type
             * @param origin An object pointer representing the scope of the type. Most likely the ChimeraSystem itself or the Module that created the type.
             * @param pdef A list of functions that are needed t manage the type and its values. Note that not all functions required.
             * @param base The type ID of the base type to inhiret from.
             * @param tag An arbitrary value for user purpose.
             *
             * @return Returns the type ID of the new type or 0 if the type could not be created.
             */
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag);
            /**
             * Creates a new type in the system with the given unique name.
             * A type always has an origin indicating which object created the type, in case the creator gets unloaded.
             * A new type needs function pointers to handle creating and destroying the type itself and creating and destroying values for this type.
             * If a base type is given, this will call all initilization functions of all base types and this type in their respective order.
             * A tag can be any value and is for user purpose only, the system makes no use of this value.
             * If needed types can be registered with attributes indicating certain functionality.
             *
             * @param name The unique name of the new type
             * @param origin An object pointer representing the scope of the type. Most likely the ChimeraSystem itself or the Module that created the type.
             * @param pdef A list of functions that are needed t manage the type and its values. Note that not all functions required.
             * @param attributes A key-value pair map containing all available attribute names with their respective value.
             *
             * @return Returns the type ID of the new type or 0 if the type could not be created.
             */
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& attributes);
            /**
             * Creates a new type in the system with the given unique name.
             * A type always has an origin indicating which object created the type, in case the creator gets unloaded.
             * A new type needs function pointers to handle creating and destroying the type itself and creating and destroying values for this type.
             * If a base type is given, this will call all initilization functions of all base types and this type in their respective order.
             * A tag can be any value and is for user purpose only, the system makes no use of this value.
             * If needed types can be registered with attributes indicating certain functionality.
             *
             * @param name The unique name of the new type
             * @param origin An object pointer representing the scope of the type. Most likely the ChimeraSystem itself or the Module that created the type.
             * @param pdef A list of functions that are needed t manage the type and its values. Note that not all functions required.
             * @param base The type ID of the base type to inhiret from.
             * @param tag An arbitrary value for user purpose.
             * @param attributes A key-value pair map containing all available attribute names with their respective value.
             *
             * @return Returns the type ID of the new type or 0 if the type could not be created.
             */
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& attributes);
            /**
             * Removes a previously added type with the given name.
             *
             * @note Removing a parameter also releases the name. A new type with the same name can be added afterwards again.
             * @note When you remove a parameter, you also remove the definition on how to destroy existing objects. Therefore you should only remove types that do not have any instance values left in the system.
             *
             * @param name The name of the type.
             */
            void removeParameter(const std::string& name);
            /**
             * Removes a previously added type with the given type ID.
             *
             * @note Removing a parameter also releases the name. A new type with the same name can be added afterwards again.
             * @note When you remove a parameter, you also remove the definition on how to destroy existing objects. Therefore you should only remove types that do not have any instance values left in the system.
             *
             * @param type The type ID of the type.
             */
            void removeParameter(const size_t& type);
            /**
             * Removes all previously added types with the given origin.
             *
             * @note Removing a parameter also releases the name. A new type with the same name can be added afterwards again.
             * @note When you remove a parameter, you also remove the definition on how to destroy existing objects. Therefore you should only remove types that do not have any instance values left in the system.
             *
             * @param origin The origin pointer to check for.
             */
            void removeParameter(void const * const origin);
        private:
            /**
             * Creates a new type in the system with the given unique name.
             * A type always has an origin indicating which object created the type, in case the creator gets unloaded.
             * A new type needs function pointers to handle creating and destroying the type itself and creating and destroying values for this type.
             * If a base type is given, this will call all initilization functions of all base types and this type in their respective order.
             * A tag can be any value and is for user purpose only, the system makes no use of this value.
             * If needed types can be registered with attributes indicating certain functionality.
             *
             * @param name The unique name of the new type
             * @param origin An object pointer representing the scope of the type. Most likely the ChimeraSystem itself or the Module that created the type.
             * @param pdef A list of functions that are needed t manage the type and its values. Note that not all functions required.
             * @param base The type ID of the base type to inhiret from.
             * @param tag An arbitrary value for user purpose.
             * @param attributes A key-value pair map containing all available attribute names with their respective value.
             * @param notify A boolean value indicating if the creating of the type should be broadcasted to registered listeners.
             *
             * @return Returns the type ID of the new type or 0 if the type could not be created.
             */
            size_t registerParameter(const std::string& name, void const * const origin, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& attributes, bool notify);
            /**
             * Deletes all object in the references map that are of a certain type. This is needed if
             * a type is removed from the system.
             *
             * @note If you created objects in Lua functions that did nt register the object as a reference, and delete a type, this will leave objects in memeory that cannot be destroyed anymore automatically and will remain until the execution ends.
             *
             * @param type The type ID to check for all referenced items.
             */
            void deleteAllReferences(size_t type);
            /**
             * Checks if a given object still exists of if its reference counter droped to 0 and
             * it is not used as a dependency anymore.
             * This call is always save to do for any pointer and will only release the pointer
             * if it exists in the references list and is not needed anymore.
             *
             * @param value The value pointer to check for.
             */
            void disposeReference(void* value);
            //void destroyItemWithDependencies(void* value);
            /**
             * Inserts a reference object of value into the _references map
             * with references counter 0, if it does not exist.
             * This will only yield consistent behaviour if you call addDependency
             * after this call.
             *
             * @param type The type of the new item.
             * @param value The pointer to the value of the given type.
             */
            void createReference(size_t type, void* value);

            /// The last id given to a type. Type IDs are created sequentially and contain no gaps. A type that was deleted is still known to the system with an empty parameter definition.
            size_t _lastID;
            /// An ordered list of types. No elements will be deleted from this list, they will only be set to nullptr if a type is removed.
            std::vector<ParameterType*>* _typeList;
            /// A map of all pointers that are shared between Lua and C++ context. This list contains a reference counter for existing copies.
            std::unordered_map<void*, struct ParameterValueData* >* _references;
            /// A map containing all dependencies of a parent object.
            std::unordered_map<void*, std::unordered_set<void*> >* _dependencies;
            /// A map containing the inverted values of the _dependencies map. It contains all parent objects that require a depencency.
            std::unordered_map<void*, std::unordered_set<void*> >* _reverseDependencies;
    };
}

#endif // PARAMETERTYPESYSTEM_H
