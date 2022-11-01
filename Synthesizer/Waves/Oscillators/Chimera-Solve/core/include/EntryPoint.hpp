#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

namespace chimera {
    class Module;
    class ChimeraSystem;

    /**
     * Base class for extension types. It provides the necessary checks and resources
     * required to load a certain type of Modules. Every EntryPoint is expected to be
     * created exactly once. It contains a list of all Modules that were added and is
     * responsible for loading and unloading these Modules.
     * When an EntryPoint is added its `init` funtion will be called
     * only once after the ChimeraContext is available. The `load` and `unload`
     * functions can be called several times during execution whenever these
     * events occur.
     */
    class EntryPoint:
        public ParameterValueCollection
    {
        friend class ChimeraSystem;
        friend class EntryPointSystem;
        friend std::ostream& operator<< (std::ostream &out, const EntryPoint &d);

        public:
            class iterator;

            /**
             * Creates a new EntryPoint.
             */
            EntryPoint();
            virtual ~EntryPoint();
            /**
             * Adds a new Module with the given name if it is a valid Module for this EntryPoint.
             *
             * @param name String identifier to register the Module.
             * @param module The new Module instance that will be tried to be added. If all checks succeed adds the new Module.
             *
             * @return Returns true if all checks were successful and the Module was added. Returns false otherwise.
             *
             * @note The name is usually obtained by the user using the string to load the Module within their script. If the ChimeraSystem is able to load it, the string and the Module pointer will be passed
             */
            bool addModule(const std::string& name, Module * const module);
            /**
             * Adds and loads a Module to this EntryPoint.
             *
             * @param name String identifier to register the Module.
             * @param module The new Module instance that will be tried to be added. If all checks succeed adds the new Module.
             *
             * @return Returns true if all checks were successful and the Module was added. Returns false otherwise.
             *
             * @note This is only an auxiliary function that simply calles addModule and loadModule.
             */
            bool addLoadModule(const std::string& name, Module * const module);
            /**
             * Removes a module that was previously added  by addModule or addLoadModule.
             *
             * @param name The name of the module that will be removed. If no Module exists with the given name, this function will do nothing.
             */
            void removeModule(const std::string& name);
            /**
             * Loads a Module that was previously added. If the Module is already loaded into this EntryPoint, the function will do nothing.
             *
             * @param name The name that was used to register a module
             *
             * @note When a Module is added to an EntryPoint through Lua, it will be loaded automatically. This function only needs to be called if you added Modules manually or if a module was unloaded via unloadModule.
             */
            void loadModule(const std::string& name);
            /**
             * Unloads a Module that was previously added and loaded. If the Module is currently not loaded into this EntryPoint, the function will do nothing.
             *
             * @param name The name that was used to register a module
             */
            void unloadModule(const std::string& name);
            /**
             * Gets the pointer to a Module that was added with the given name
             *
             * @param name The name of a registered Module.
             *
             * @return Returns the pointer to a Module if name exists and nullptr otherwise.
             */
            const Module* getModule(const std::string& name) const;
            /**
             * Checks if the given Module was added to this EntryPoint and returns the name of the Module.
             *
             * @param module A pointer to a Module object to look up.
             *
             * @return Returns the name of the Module if it was found and an empty string otherwise.
             */
            const std::string findModule(Module const * const module) const;
            /**
             * This function can be used in derived EntryPoints if a setup process is necessary.
             * Returns a pointer to an arbitrary data structur that will be passed to the load
             * function of every Module that is loaded into this EntryPoint.
             *
             * @note The setup of that data structur should happen in the load method of an EntryPoint and should be freed during the unload method.
             *
             * @return Returns a pointer to a data structure used to set up Modules.
             */
            virtual void const * const getModuleLoadParams() const;
            /**
             * Provides an iterator object used to walk though all Modules
             * that were added to the EntryPoint
             *
             * @return Returns an iterator pointing to the first Module found in the EntryPoint.
             */
            Module::iterator begin();
            /**
             * Provides an iterator object pointing to the end of the Module list
             *
             * @return Returns a readonly iterator pointing one past the end of the list of Modules.
             */
            Module::iterator end();
            /**
             * Returns the number of added Modules.
             *
             * @return Returns the number of added Modules.
             */
            int size() const;
        protected:
            /**
             * This function can be used in derived EntryPoints if a setup process is necessary.
             * You should always prefer this function over the constructor because a ChimeraContext
             * is available at this point. The init function will be called exactly once for each
             * EntryPoint that is added to the system.
             */
            virtual void init();
            /**
             * This function can be used in derived EntryPoints to allocate all resources that are
             * needed.
             */
            virtual void load();
            /**
             * This function can be used in derived EntryPoints and should free all resouces
             * that were allocated during the load method.
             */
            virtual void unload();
            /**
             * This function can be used in derived EntryPoints if a setup process is necessary.
             * A pointer to a new Module is passed and this method should return true if the
             * Module is valid and can be added to the EntryPoint.
             *
             * @param module A pointer to a newly created Module with the attempt to add it to this EntryPoint.
             *
             * @return Returns true if the Module can be added and false otherwise.
             */
            virtual bool checkModule(Module const * const module) const;
        private:
            /**
             * Internal load function. This function will load the EntryPoint into the
             * running ChimeraSystem.
             * While loading the virtual `load` function is called to add set up
             * capability for derived EntryPoints.
             */
            void loadEntryPoint();
            /**
             * Internal unload function. This function will unload the EntryPoint from the
             * running ChimeraSystem.
             * While unloading the virtual `unload` function is called to add tear down
             * capability for derived EntryPoints.
             */
            void unloadEntryPoint();
            /**
             * Notifies the EntryPoint about all deletions of Modules.
             * The destructor of a registered Module will call this function
             * using sender to pass its pointer so it can be removed from the Module list.
             *
             * @param sender A pointer to a Module object that was destroyed.
             */
            virtual void notifyDelete(StateSynchrony* sender);

            /// A map saving the names and pointers to all added Modules.
            map_t_Module *_modules;
    };

    /**
     * Iterator for EntryPointSystem to list all added EntryPoints
     */
    class EntryPoint::iterator
    {
        public:
            /**
             * Create a new iterator. This needs an existing map to reuse the iterator.
             * Transforms the key-value iterator to simple unref iterator.
             *
             * @param it The map iterator containg the list to go though.
             */
            iterator (map_t_EntryPoint::iterator it);
            virtual ~iterator();
            /**
             * Get the EntryPoint object
             *
             * @return Returns the current EntryPoint in the list.
             */
            EntryPoint* operator*();
            /**
             * Steps to the next EntryPoint in the list
             */
            EntryPoint::iterator operator++(); // prefix
            /**
             * Steps to the next EntryPoint in the list
             */
            EntryPoint::iterator operator++(int); // postfix
            /**
             * Checks if two iterator objects are equal
             */
            bool operator==(const EntryPoint::iterator& other);
            /**
             * Checks if two iterator objects are not equal
             */
            bool operator!=(const EntryPoint::iterator& other);
        private:
            /// iterator object of a key-value pair map
            map_t_EntryPoint::iterator _it;
    };
}

#endif // ENTRYPOINT_H
