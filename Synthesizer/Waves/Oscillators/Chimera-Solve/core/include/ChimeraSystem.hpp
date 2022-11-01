#ifndef CHIMERASYSTEM_H
#define CHIMERASYSTEM_H

namespace chimera
{
    class EntryPointSystem;
    class LoggingSystem;
    class ParameterTypeSystem;

    class ChimeraSystem:
        public StateSynchrony
    {
        friend class chimera::EntryPoint;
        friend class chimera::EntryPointSystem;
        friend class chimera::ParameterTypeSystem;
        friend class chimera::ChimeraContext;

        public:
            /**
             * Creates a new ChimeraSystem instance.
             */
            ChimeraSystem();
            virtual ~ChimeraSystem();
            /**
             * Returns the LoggingSystem bundeling functionality for logging and output.
             *
             * @return Returns the LoggingSystem bundeling functionality for logging and output.
             */
            LoggingSystem* getLoggingSystem();
            /**
             * Returns the ParameterSystem bundeling functionality for types and values shared with Lua.
             *
             * @return Returns the ParameterSystem bundeling functionality for Types and Values shared with Lua.
             */
            ParameterTypeSystem* getTypeSystem();
            /**
             * Returns the EntryPointSystem bundeling functionality for loading EntryPoints and Modules.
             *
             * @return Returns the EntryPointSystem bundeling functionality for loading EntryPoints and Modules.
             */
            EntryPointSystem* getEntryPointSystem();
            /**
             * Attempts to load the Module with the given name into Entrypoint.
             *
             * @param entrypoint The EntryPoint used for loading the Module.
             * @param name The name of the EntryPoint to check for.
             *
             * @return Returns a pointer to the new Module instance if it could be loaded successfully and nullptr otherwise.
             */
            Module* includeModule(EntryPoint* entrypoint, const std::string& name);
            /**
             * Creates a new instance for a Module with the given name.
             * The derived instance of the ChimeraSystem is responsible for
             * handling the loading of the Module and creating the instance.
             * If loading fails this function will return nullpts.
             *
             * @param entrypoint The EntryPoint used for loading the Module.
             * @param name The name of the EntryPoint to check for.
             *
             * @return Returns a pointer to the new Module instance if it could be loaded successfully and nullptr otherwise.
             */
            virtual Module* openModule(EntryPoint const * const entrypoint, const std::string& name);
            /**
             * Closes a previously opened Module
             *
             * @param module The pointer to a Module instance that will be removed from the current context
             */
            virtual void closeModule(Module* module);
            /**
             * Returns the original Lua state.
             *
             * @return Returns the original Lua state.
             */
            lua_State* getLuaState();
            /**
             * Returns the current Lua state. This state can change, if a new Lua thread is currently
             * running.
             *
             * @return Returns the original Lua state.
             */
            lua_State* getCurrentState();
            /**
             * Sets the current Lua state. Before you call lua_resume, make sure to update
             * the current Lua state. Also after returning from lua_resume reset to the old state.
             *
             * @param next The new Lua state for the started thread.
             */
            void setCurrentState(lua_State* next);
            /**
             * Returns the name of this class.
             *
             * @return Returns the name of this class.
             */
            virtual const std::string getGUID() const override;
        protected:
            virtual ChimeraContext* createContext(StateSynchrony* referrer);
            void pushEntryPoint(const std::string& name, EntryPoint* ep);
            void popEntryPoint(EntryPoint* ep);
            void pushModule(EntryPoint* ep, const std::string& name, Module* module);
            void popModule(EntryPoint* ep, Module* module);
            void pushType(ParameterType const * const p);
            void popType(const ParameterType& type);

            void pushLibrary(const std::string& name, lua_CFunction createMetaTable);

            void setLogLevel(const unsigned int& loglevel);
        private:
            lua_State* _baseState;
            lua_State* _currentState;

            void init();
            virtual void notifyItemAdded(StateSynchrony* sender, void* item, void const * const data) override;
            virtual void notifyItemRemoved(StateSynchrony* sender, void* item, void const * const data) override;
            virtual void notifyItemLoaded(StateSynchrony* sender, void* item, void const * const data) override;
            virtual void notifyItemUnloaded(StateSynchrony* sender, void* item, void const * const data) override;

            void chainLoad(ParameterType const * const p, const int& id) const;

            LoggingSystem* _loggingSys;
            ParameterTypeSystem* _typeSys;
            EntryPointSystem* _epSys;
    };
}

#endif // CHIMERASYSTEM_H
