#ifndef MODULE_H
#define MODULE_H

namespace chimera
{
    class EntryPoint;
    class ChimeraContext;
    class ChimeraSystem;

    /**
     * Base class for extensions. A module is linked to a certain EntryPoint
     * and can create the required instance object of the EntryPoint.
     * An individual Module instance is expected to be created only once and
     * only be added to a single EntryPoint.
     * However a module can provide multiple instance objects and is responsible
     * for destroying them  when they are not used anymore. An instance class
     * is usually derived from a base class the EntryPoint demands.
     * Please see EntryPoint documentation of what Object to create.
     * When a module is added to an EntryPoint its `init` funtion will be called
     * only once after the ChimeraContext is available. The `load` and `unload`
     * functions can be called several times during execution whenever these
     * events occur.
     */
    class Module:
        public ParameterValueCollection
    {
        friend class EntryPoint;
        friend class EntryPointSystem;
        friend class ChimeraSystem;
        friend std::ostream& operator<< (std::ostream &out, const Module &d);

        public:
            class iterator;
            /**
             * Creates a new Module.
             */
            Module();
            virtual ~Module();

            /**
             * Checks wether a method with the given name exists and returns
             * the function pointer if it exists. This function will be called
             * if the module method is called in the Lua script on a instance
             * object created by this module.
             *
             * @param name The method name the method was published by
             *
             * @return Returns the function pointer if the function exists and `nullptr` otherwise
             */
            chimera::fn_instancefnwrapper getMethod(const std::string& name) const;
            /**
             * Create a new object instance with the given parameters.
             * This function will be called directly from Lua with the parameters
             * that were supplied with the function call by a user.
             * It is strongly encurraged to check all parameters strictly for sanity.
             *
             * @param parameters A vector of `ParameterValue` items.
             *
             * @return Returns an instance that is required for this EntryPoint. Please check the EntryPoint documentation for more details.
             */
            virtual void* getInstance(EntryPoint const * const entrypoint, vec_t_LuaItem& parameters) const = 0;
            /**
             * Destroys an instance object that was previously created with
             * a call to `getInstance`.
             *
             * @note This is usually just a `delete` statement.
             *
             * @param instance An instance object created by `getInstance`.
             */
            virtual void destroyInstance(EntryPoint const * const entrypoint, void * const instance) const = 0;
            /**
             * Returns the value for a given attribute. If the attribute is
             * unknown or not implemented it should simply return `0`.
             * The attributes will be looked up for module instance objects
             * returned by `getInstance`.
             *
             * @param attr The name of the attribute.
             *
             * @return If present returns a value different to `0`. For simple flags it should just return `1`.
             */
            virtual size_t getAttribute(const std::string& attr) const;
            /**
             * Set meta information about the module. This should usually be done only by the
             * module loader, not by the module itself.
             *
             * @param key The key to set a value for
             * @param value The new value to assoziate with key
             */
            void setMetaField(const std::string& key, const std::string& value) const;
            /**
             * Retrieve a previously stored value for a give meta key.
             *
             * @param key The key to look up in the meta hash map.
             * @return Returns the value assoziated with key or an empty string if the key does not exist.
             */
            const std::string getMetaField(const std::string& key) const;
        protected:
            /**
             * Adds a new method to module instance objects. These methods will be accessible
             * directly through the Lua script by the given name.
             *
             * @param name The name to register the method with.
             * @param method Implementation of the method.
             *
             * @note A called function will always be called with its EntryPoint and Module objects that were used to create the instance object with getInstance.
             */
            void registerMethod(const std::string& name, chimera::fn_instancefnwrapper method);

            /**
             * This function can be used in derived Modules if a setup process is necessary.
             * You should always prefer this function over the constructor because a ChimeraContext
             * is available at this point. The init function will be called exactly once for each
             * EntryPoint it is added to.
             *
             * @param entryPoint The EntryPoint that this module was added to.
             */
            virtual void init(EntryPoint const * const entryPoint);
            /**
             * This function can be used in derived Modules to allocate all resources that are
             * needed for the Module. Additionally all checks should be done at this point.
             * If the Module cannot be used this function should return false. This will prevent
             * the system from integrating it into the Lua script context. It will be called for
             * every EntryPoint the Module is added to.
             *
             * @param entryPoint The EntryPoint that this module was added to.
             * @param params A pointer to a data structure holding additional data. Please refer to the EntryPoint documentation to find what object you will recieve.
             * @return indicates if loading was successful. It should return true if the module can be used from the Lua context and the runtime will continue the loading process. If it returns false the rest of the loading process will be cancled.
             */
            virtual bool load(EntryPoint const * const entryPoint, void const * const params);
            /**
             * This function can be used in derived Modules to dispose all allocated
             * resources that were needed for the Module. It will be called for
             * every EntryPoint the Module was added to.
             *
             * @param entryPoint The EntryPoint that this module was added to.
             */
            virtual void unload(EntryPoint const * const entryPoint);
        private:
            /**
             * Internal load function. This function will attempt to load the module taking the given
             * EntryPoint as context. If the context was already loaded this function does nothing.
             * While loading the virtual `load` function is called and the loading is canceled
             * if that function returns false.
             *
             * @param entryPoint The EntryPoint that this module was added to.
             * @param name The name that was used to load the module.
             * @param params A pointer to a data structure holding additional data. This data will be passed to the `load` function.
             */
            void loadModule(EntryPoint const * const entryPoint, const std::string& name, void const * const params);
            /**
             * Internal unload function. This function will attempt to unload the module taking the given
             * EntryPoint as context. If the context was not loaded this function does nothing.
             *
             * @param entryPoint The EntryPoint that this module was added to.
             */
            void unloadModule(EntryPoint const * const entryPoint);

            /// A map to store function pointers added by registerMethod
            std::unordered_map<std::string, chimera::fn_instancefnwrapper>* _instanceMethods;
            /// A simple key-value pair map to store additional data about the module.
            std::unordered_map<std::string, std::string>* _meta;

    };

    /**
     * Iterator for EntryPoints to list all added Modules
     */
    class Module::iterator
    {
        public:
            /**
             * Create a new iterator. This needs an existing map to reuse the iterator.
             * Transforms the key-value iterator to simple unref iterator.
             *
             * @param it The map iterator containg the list to go though.
             */
            iterator (map_t_Module::iterator it);
            virtual ~iterator();
            /**
             * Get the Module object
             *
             * @return Returns the current module in the list.
             */
            Module* operator*();
            /**
             * Steps to the next Module in the list
             */
            Module::iterator operator++(); // prefix
            /**
             * Steps to the next Module in the list
             */
            Module::iterator operator++(int); // postfix
            /**
             * Checks if two iterator objects are equal
             */
            bool operator==(const Module::iterator& other);
            /**
             * Checks if two iterator objects are not equal
             */
            bool operator!=(const Module::iterator& other);
        private:
            /// iterator object of a key-value pair map
            map_t_Module::iterator _it;
    };
}

#endif // MODULE_H
