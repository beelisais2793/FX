#ifndef CHIMERACONTEXT_H
#define CHIMERACONTEXT_H

namespace chimera
{
    struct T_ParameterDef;

    class Module;
    class EntryPoint;
    class ParameterValue;
    class LuaFunctionWrapper;
    class StateSynchrony;
    class ParameterValueCollection;
    class ChimeraSystem;

    class ChimeraContext
    {
        friend class Module;
        friend class EntryPoint;
        friend class ParameterValueCollection;
        friend class ParameterTypeSystem;
        friend class EntryPointSystem;

        public:
            ChimeraContext(ChimeraSystem* chimeraSystem, StateSynchrony* referrer);
            virtual ~ChimeraContext();

            /**
             * Adds a new EntryPoint with the given name to the system. Creating an EntryPoint will
             * create a global variable in the lua context with the same name.
             *
             * @note Each EntryPoint should only be added once. Otherwise the system might get confused of where to add or remove new Modules.
             *
             * @param name The name to publish the new EntryPoint. This name needs to be unique.
             * @param ep The new EntryPoint to add.
             */
            void addEntryPoint(const std::string& name, EntryPoint* const ep) const;
            /**
             * Removes the EntryPoint from the system. If the name cannot be found nothing will be removed.
             *
             * @param name The name of the EntryPoint to remove.
             */
            void removeEntryPoint(const std::string& name) const;
            /**
             * Returns the EntryPoint instance for a given name.
             *
             * @param name The name of an EntryPoint to look for.
             *
             * @return Returns the pointer to an EntryPoint that was registered with the given name and nullptr if no EntryPoint was found.
             */
            EntryPoint* const getEntryPoint(const std::string& name) const;
            /**
             * Checks if the given EntryPoint was added and returns the name of it.
             *
             * @param module A pointer to an EntryPoint object to look up.
             *
             * @return Returns the name of the EntryPoint if it was found and an empty string otherwise.
             */
            const std::string findEntryPoint(EntryPoint const * const entrypoint) const;

            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag) const;
            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef) const;
            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag, const std::unordered_map<std::string, size_t>& attributes) const;
            size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, const std::unordered_map<std::string, size_t>& attributes) const;
            void removeParameter(const std::string& name);

            ParameterValue createValue(size_t type, void* value) const;
            LuaFunctionWrapper createFunction(fn_luafnwrapper fn) const;

            /**
             * Adds a dependency for a specific object. This prevents dependency from
             * being deleted through garbage collection until the dependency is released.
             *
             * @note it is not important if item also exists in the references list, but will still prevent the deletion.
             *
             * @param item An object that uses some other object that might be garbarge collected.
             * @param dependency An object obtained from the Lua context that is needed for item and must not be destroyed while item exists.
             */
            void addDependency(void* item, void* dependency) const;
            /**
             * Releases a previously added dependency relation.
             *
             * @note this will immediately destroy all items that were set as a dependency, if no items require it anymore and if it is also no longer needed within Lua.
             * @note This could cause a chain reaction if dependencies were also used as items that used dependencies.
             *
             * @param item An object that used some other object that might be garbarge collected.
             * @param dependency An object obtained from the Lua context that was needed for item and must not be destroyed while item existed.
             */
            void releaseDependency(void* item, void* dependency) const;
            /**
             * This will release all dependency relations that used item.
             *
             * @note this will immediately destroy all items that were set as a dependency, if no items require it anymore and if it is also no longer needed within Lua.
             * @note This could cause a chain reaction if dependencies were also used as items that used dependencies.
             *
             * @param item An object that used some other objects that might be garbarge collected.
             */
            void removeDependencyItem(void* item) const;

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
        protected:
            ChimeraSystem* _chimeraSystem;
            StateSynchrony* _referrer;
        private:
    };
}

#endif // CHIMERACONTEXT_H
