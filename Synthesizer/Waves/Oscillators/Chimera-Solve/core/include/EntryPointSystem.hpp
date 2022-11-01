#ifndef ENTRYPOINTSYSTEM_H
#define ENTRYPOINTSYSTEM_H

namespace chimera {
    class ChimeraSystem;

    /**
     * Managemant class to create and register all EntryPoints with their Modules.
     * An EntryPointSystem requires an existing ChimeraSystem and is only used to seperate
     * the class of methods that deal with EntryPoints and Modules, their loading and unloading
     * and helps to keep the ChimeraSystem class on a smaller size.
     */
    class EntryPointSystem final:
        public StateSynchrony
    {
        friend class EntryPoint;
        friend class ChimeraSystem;

        public:
            virtual ~EntryPointSystem();
            /**
             * Adds a new EntryPoint with the given name to the system. Creating an EntryPoint will
             * create a global variable in the lua context with the same name.
             *
             * @note Each EntryPoint should only be added once. Otherwise the system might get confused of where to add or remove new Modules.
             *
             * @param name The name to publish the new EntryPoint. This name needs to be unique.
             * @param ep The new EntryPoint to add.
             */
            void addEntryPoint(const std::string& name, EntryPoint* const ep);
            /**
             * Removes the EntryPoint from the system. If the name cannot be found nothing will be removed.
             *
             * @param name The name of the EntryPoint to remove.
             */
            void removeEntryPoint(const std::string& name);
            /**
             * Returns the EntryPoint instance for a given name.
             *
             * @param name The name of an EntryPoint to look for.
             *
             * @return Returns the pointer to an EntryPoint that was registered with the given name and nullptr if no EntryPoint was found.
             */
            EntryPoint* const getEntryPoint(const std::string& name);
            /**
             * Checks if the given EntryPoint was added and returns the name of it.
             *
             * @param module A pointer to an EntryPoint object to look up.
             *
             * @return Returns the name of the EntryPoint if it was found and an empty string otherwise.
             */
            const std::string findEntryPoint(EntryPoint const * const entrypoint) const;
            /**
             * Returns the name of this class.
             *
             * @return Returns the name of this class.
             */
            virtual const std::string getGUID() const override;

            /**
             * Provides an iterator object used to walk though all EntryPoints
             * that were added to the EntryPointSystem
             *
             * @return Returns an iterator pointing to the first EntryPoint found in the EntryPointSystem.
             */
            EntryPoint::iterator begin();
            /**
             * Provides an iterator object pointing to the end of the EntryPoint list
             *
             * @return Returns a readonly iterator pointing one past the end of the list of EntryPoints.
             */
            EntryPoint::iterator end();
        protected:
        private:
            /**
             * Notifies the EntryPointSystem about all deletions of EntryPoints.
             * The destructor of a registered EntryPoints will call this function
             * using sender to pass its pointer so it can be removed from the EntryPoint list.
             *
             * @param sender A pointer to a EntryPoint object that was destroyed.
             */
            void notifyDelete(StateSynchrony* sender) override;
            /**
             * Creates a new EntryPointSystem instance. Only ChimeraSystem instances are
             * allowed to call this constructor.
             *
             * @param chimeraSystem The parent ChimeraSystem that created this subsystem
             */
            EntryPointSystem(ChimeraSystem* chimeraSystem);

            /// A map saving the names and pointers to all added EntryPoints.
            map_t_EntryPoint* _hooks;
    };
}

#endif // ENTRYPOINTSYSTEM_H
