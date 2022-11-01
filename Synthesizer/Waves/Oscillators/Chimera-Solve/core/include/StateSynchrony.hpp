#ifndef STATESYNCHRONY_H
#define STATESYNCHRONY_H

namespace chimera
{
    class Module;
    class EntryPoint;
    class ChimeraContext;
    class ChimeraSystem;

    /**
     * Base class for containers and container items that need to be
     * synchronized throughout the system. This is used to add EntryPoints
     * to the system, Modules to these EntryPoints and new ParameterTypes
     * for Lua objects.
     */
    class StateSynchrony
    {
        friend class ChimeraContext;
        friend class ChimeraSystem;
        friend class ParameterValueCollection;
        friend class EntryPointSystem;
        friend class ParameterTypeSystem;
        friend class LoggingSystem;
        friend class EntryPoint;
        friend class Module;

        public:
            /**
             * Creates a new StateSynchrony instance.
             */
            StateSynchrony();
            virtual ~StateSynchrony();
            /**
             * Adds a new listener that is informed about all loading state changes and items
             * that are added, removed, loaded, unloaded and destroyed.
             *
             * @param listener The StateSynchrony object to notify about all changes.
             */
            void addListener(StateSynchrony* listener);
            /**
             * Removes a previously added listener. This will stop all notifications.
             *
             * @param listener The StateSynchrony object that was previously added. If the object does not exist the call simply does nothing.
             */
            void removeListener(StateSynchrony* listener);
            /**
             * Checks if this instance was loaded to a specific context.
             * Note that an item can be loaded to different contexts at the same time.
             *
             * @param context The context to check for.
             *
             * @return Returns true if the object was loaded with the supplied context. Returns false otherwise.
             */
            bool isLoaded(StateSynchrony const * const context) const;
            /**
             * Returns the ChimeraContext to access ChimeraSystem functionality
             * inside derived EntryPoints and Modules.
             *
             * @return Returns the ChimeraContext that was created by the running ChimeraSystem.
             */
            ChimeraContext* getContext() const;
            /**
             * Gets called from another StateSynchrony instance whenever it gets loaded into a context.
             * StateSynchrony instances can be loaded for multiple contexts at the same time.
             *
             * @param sender The StateSynchrony object that changes its state.
             * @param context The context that got loaded.
             * @param data A pointer to a data object to supply additional data for the event.
             */
            virtual void notifyLoad(StateSynchrony* sender, StateSynchrony* context, void const * const data);
            /**
             * Gets called from another StateSynchrony instance whenever it gets unloaded from a context.
             * StateSynchrony instances can be loaded for multiple contexts at the same time.
             *
             * @param sender The StateSynchrony object that changes its state.
             * @param context The context that got unloaded.
             * @param data A pointer to a data object to supply additional data for the event.
             */
            virtual void notifyUnload(StateSynchrony* sender, StateSynchrony* context, void const * const data);
            /**
             * Gets called from another StateSynchrony instance whenever an item gets added.
             *
             * @param sender The StateSynchrony object that changes its state.
             * @param item The new item that was added.
             * @param data A pointer to a data object to supply additional data for the event.
             */
            virtual void notifyItemAdded(StateSynchrony* sender, void* item, void const * const data);
            /**
             * Gets called from another StateSynchrony instance whenever an item is removed.
             *
             * @param sender The StateSynchrony object that changes its state.
             * @param item The item that was removed.
             * @param data A pointer to a data object to supply additional data for the event.
             */
            virtual void notifyItemRemoved(StateSynchrony* sender, void* item, void const * const data);
            /**
             * Gets called from another StateSynchrony instance whenever an item gets loaded.
             *
             * @note When an item gets loaded the loading context is identical to the sender object.
             *
             * @param sender The StateSynchrony object that changes its state.
             * @param item The item that was loaded with sender as context.
             * @param data A pointer to a data object to supply additional data for the event.
             */
            virtual void notifyItemLoaded(StateSynchrony* sender, void* item, void const * const data);
            /**
             * Gets called from another StateSynchrony instance whenever an item gets unloaded.
             *
             * @note When an item gets unloaded the loading context is identical to the sender object.
             *
             * @param sender The StateSynchrony object that changes its state.
             * @param item The item that was unloaded with sender as context.
             * @param data A pointer to a data object to supply additional data for the event.
             */
            virtual void notifyItemUnloaded(StateSynchrony* sender, void* item, void const * const data);
            /**
             * Gets called from another StateSynchrony instance whenever its destructor is called.
             *
             * @param sender The StateSynchrony object that gets destroyed.
             */
            virtual void notifyDelete(StateSynchrony* sender);
            /**
             * A unique sting to represent the StateSynchrony instance.
             *
             * @return Returns a unique string to represent the StateSynchrony instance.
             */
            virtual const std::string getGUID() const = 0;
        protected:
        private:
            /**
             * Triggers the loading into the given context and
             * notifies all registered listeners about the event.
             *
             * @param context The context to load.
             */
            void stateLoaded(StateSynchrony* context);
            /**
             * Triggers the loading into the given context and
             * notifies all registered listeners about the event.
             * If the context is already loaded this call will do nothing.
             *
             * @param context The context to load.
             * @param data Additional data that will be send to all listeners through the notification call.
             */
            void stateLoaded(StateSynchrony* context, void const * const data);
            /**
             * Triggers the unloading from the given context and
             * notifies all registered listeners about the event.
             * If the context was not loaded this call will do nothing.
             *
             * @param context The context to load.
             */
            void stateUnloaded(StateSynchrony* context);
            /**
             * Triggers the unloading from the given context and
             * notifies all registered listeners about the event.
             * If the context was not loaded this call will do nothing.
             *
             * @param context The context to load.
             * @param data Additional data that will be send to all listeners through the notification call.
             */
            void stateUnloaded(StateSynchrony* context, void const * const data);
            /**
             * Notifies all registered listeners about a newly added item.
             *
             * @param item The new item that was added.
             * @param data Additional data that will be send to all listeners through the notification call.
             */
            void itemAdded(void* item, void const * const data);
            /**
             * Notifies all registered listeners about an item that was removed.
             *
             * @param item The existing item that was removed.
             * @param data Additional data that will be send to all listeners through the notification call.
             */
            void itemRemoved(void* item, void const * const data);
            /**
             * Notifies all registered listeners about an item that was loaded.
             *
             * @param item The item that was loaded with this object as context.
             * @param data Additional data that will be send to all listeners through the notification call.
             */
            void itemLoaded(void* item, void const * const data);
            /**
             * Notifies all registered listeners about an item that was unloaded.
             *
             * @param item The item that was unloaded from this object as context.
             * @param data Additional data that will be send to all listeners through the notification call.
             */
            void itemUnloaded(void* item, void const * const data);

            /// pointer to the main execution system
            ChimeraContext* _context;
            /// list of all listeners that were registered
            std::unordered_set<StateSynchrony*>* _listeners;
            /// reverse map of all registered listeners to enable state consistency in case of destruction of items
            std::unordered_set<StateSynchrony*>* _linkedItems;
            /// a list of all contexts that were loaded for this item
            std::unordered_set<StateSynchrony*>* _loadStates;
    };
}

#endif // STATESYNCHRONY_H
