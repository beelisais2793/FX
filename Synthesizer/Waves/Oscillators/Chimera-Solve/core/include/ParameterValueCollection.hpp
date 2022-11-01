#ifndef PARAMETERVALUECOLLECTION_H
#define PARAMETERVALUECOLLECTION_H

namespace chimera {
    class ChimeraContext;
    class ChimeraSystem;

    /**
     * Base Object for state synchonous objects that need to store a list of Lua objects.
     * This class is used for Modules and EntryPoints so they can have additional fields and
     * added to their corresponding Lua object.
     */
    class ParameterValueCollection:
        public StateSynchrony
    {
        friend class ChimeraSystem;

        public:
            /**
             * Creates a new instance.
             */
            ParameterValueCollection();
            virtual ~ParameterValueCollection();
            /**
             * Adds a new value for the given key. If the name already exists, the value will be replaced.
             *
             * @param name The name to register the object. This name will be used as the field name of the corresponding Lua object.
             * @param value The new value to store for the given name.
             */
            void setValue(const std::string& name, ParameterValue& value);
            /**
             * Get the item iterator.
             *
             * @return Returns the iterator object walking through all key-value pairs in this list.
             */
            std::unordered_map<std::string, ParameterValue>::const_iterator beginItems() const;
            /**
             * Get the iterator item after the last element in the list.
             *
             * @return Returns the iterator object pointing to the end of the list.
             */
            std::unordered_map<std::string, ParameterValue>::const_iterator endItems() const;
            /**
             * Returns the value for a given key.
             *
             * @param name The name to look for.
             * @return Returns the value if it exists. Otherwise returns an object with type `0` and value `nullptr`.
             */
            ParameterValue operator[](const std::string& name) const;
        protected:
            // removes all items from the list
            //void truncate();
        private:
            /// Key-value pair list to store Lua names and values.
            map_t_LuaItem* _items;
    };
}

#endif // PARAMETERVALUECOLLECTION_H
