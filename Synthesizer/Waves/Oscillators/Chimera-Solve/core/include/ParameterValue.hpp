#ifndef PARAMETERVALUE_H
#define PARAMETERVALUE_H

typedef struct lua_State lua_State;

namespace chimera {
    class ChimeraSystem;
    class ParameterReferenceInteger;
    class ParameterReferenceString;

    /**
     * Data structure that is shared for ParameterValue instances pointing to the same object.
     */
    struct ParameterValueData
    {
        /// The executing ChimeraSystem. This is set when an item is bound.
        ChimeraSystem* chimeraSystem;
        /// The ParameterType ID of the item. This value must match with the value.
        size_t type;
        /// A pointer to the actual instance. If no references remain and the object is not needed anymore, it will be destroyed.
        void* value;
        /// The number of references remaining. Whenever a ParameterValue is created with a certain (existing) value, the number is increamented. When an item is disposed, the number gets decremented.
        int references;
    };

    /**
     * Definition for items that are shared between Lua and C++ context. These items have a distinct type
     * that identifies the value, extend it with additional functionality and is able
     * to destroy the object properly.
     */
    class ParameterValue
    {
        friend class ChimeraSystem;
        friend class ChimeraContext;
        friend class ParameterValueCollection;
        friend class ParameterTypeSystem;
        friend class ParameterReferenceInteger;
        friend class ParameterReferenceString;

        friend int chimera::lua_UserData_gc(lua_State* L);
        friend std::ostream& operator<<(std::ostream& os, const ParameterValue& obj);

        public:
            /**
             * Creates a new ParameterValue with type 0.
             */
            ParameterValue();
            /**
             * Copies an existing ParameterValue. This will be bound to the same ChimeraSystem as the original value.
             *
             * @note This will only increase the reference counter to the value and use the same data object as the original.
             */
            ParameterValue(const ParameterValue& p);
            /**
             * Creates a new ParameterValue with a numeric value and not bound to any ChimeraSystem.
             *
             * @param value The value that will be assigned.
             */
            ParameterValue(const double& value);
            /**
             * Creates a new ParameterValue with a numeric value and not bound to any ChimeraSystem.
             *
             * @param value The value that will be assigned.
             */
            ParameterValue(const int& value);
            /**
             * Creates a new ParameterValue with a boolean value and not bound to any ChimeraSystem.
             *
             * @param value The value that will be assigned.
             */
            ParameterValue(const bool& value);
            /**
             * Creates a new ParameterValue with a string value and not bound to any ChimeraSystem.
             *
             * @param value The value that will be assigned.
             */
            ParameterValue(const std::string& value);
            /**
             * Creates a new ParameterValue with a string value and not bound to any ChimeraSystem.
             *
             * @param value The value that will be assigned.
             */
            ParameterValue(const char value[]);
            //ParameterValue(const std::unordered_map<std::string, ParameterValue>& value);
            //ParameterValue(const std::vector<ParameterValue>& value);
            /**
             * Creates the corresponding ParameterValue from an ParameterReferenceInteger.
             * ParameterReferenceInteger are retrieved by indexing a Lua table explictitly
             * with the [] operator using an integer index.
             *
             * @param value Looks up the actual data at the index stored in the Lua table.
             */
            ParameterValue(const ParameterReferenceInteger& value);
            /**
             * Creates the corresponding ParameterValue from an ParameterReferenceInteger.
             * ParameterReferenceInteger are retrieved by indexing a Lua table explictitly
             * with the [] operator using an integer index.
             *
             * @param value Looks up the actual data at the index stored in the Lua table.
             */
            ParameterValue(const ParameterReferenceString& value);

            virtual ~ParameterValue();
            /**
             * Gets the type of the object. If the type cannot be looked up, if the object has no
             * type or if the type was deleted meanwhile it will simply return 0.
             *
             * @return Returns the type of the object if it has one and returns 0 otherwise.
             */
            size_t getType() const;
            /**
             * Gets the pointer to the stored object. If the type cannot be looked up, if the
             * object has no type or if the type was deleted meanwhile it will simply return nullptr.
             *
             * @return Returns the value pointer of the object if the object is valid and returns nullptr otherwise.
             */
            void* getValue() const;
            /**
             * Gets the base class of the type.
             *
             * @note This call cannot be cascaded if the base type also inhiret from another type.
             * @note This can only be called if the object was bound to a ChimeraSystem instance. Otherwise it will always return 0.
             *
             * @return Returns the immediate base type of this type or 0 if it has no base type.
             */
            size_t getBaseType() const;
            /**
             * Gets the tag value for the type.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance. Otherwise it will always return 0.
             *
             * @return Returns the tag value of the type if such exists and returns 0 otherwise.
             */
            size_t getTag() const;
            /**
             * Looks up the value for an attribute set on the type.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance. Otherwise it will always return 0.
             *
             * @return Returns the value of the attribute with name attr if such exists and returns 0 otherwise.
             */
            size_t getAttribute(const std::string& attr) const;
            /**
             * Tries to cast this object to another type.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance. Otherwise it will always return an item of type 0.
             *
             * @param type The new type ID to cast to.
             *
             * @return Returns the new item if the cast was successful and an item of type 0 otherwise.
             */
            ParameterValue cast(size_t type) const;
            /**
             * Gets the pointer to the stored object. If the type cannot be looked up, if the
             * object has no type or if the type was deleted meanwhile it will simply return nullptr.
             *
             * @return Returns the value pointer of the object if the object is valid and returns nullptr otherwise.
             */
            void* const operator-> ();
            /**
             * Removes the ParameterValue object from the reference counter. If the reference counter reaches 0 and
             * if the value object is not used in Lua or as a dependency, the value object will be destroyed.
             *
             * @note Usually this function is called by the destructor of the object. However if you explicitly want to remove the reference you are free to do so. The object cannot be used anymore after it was disposed.
             */
            void dispose();

            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator+(const ParameterValue& p);  // __add
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator-(const ParameterValue& p);  // __sub
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator*(const ParameterValue& p);  // __mul
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator/(const ParameterValue& p);  // __div
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator%(const ParameterValue& p);  // __mod
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator^(const ParameterValue& p);  // __pow
            /**
             * Will push this value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator-();                         // __unm
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator&(const ParameterValue& p);  // __band
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator|(const ParameterValue& p);  // __bor
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue XOR(const ParameterValue& p);        // __bxor
            /**
             * Will push this value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator~();                         // __bnot
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator<<(const ParameterValue& p); // __shl
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param i The number of shifts of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator<<(const int& i);            // __shl
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator>>(const ParameterValue& p); // __shr
            /**
             * Will push this and the given value to the Lua state and perform
             * the given arithmethic operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param i The number of shifts of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue operator>>(const int& i);            // __shr
            /**
             * Will push this and the given value to the Lua state and perform
             * the concaternation operation using the lua_concat function and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue concat(const ParameterValue& b);     // __concat
            /**
             * Will push this value to the Lua state and perform
             * the length operation using the lua_len function and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            ParameterValue length();                            // __len
            /**
             * Will push this and the given value to the Lua state and perform
             * the given boolean operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            bool operator==(const ParameterValue& p);           // __eq
            /**
             * Will push this and the given value to the Lua state and perform
             * the given boolean operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            bool operator!=(const ParameterValue& p);           // __eq
            /**
             * Will push this and the given value to the Lua state and perform
             * the given boolean operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            bool operator<(const ParameterValue& p);            // __lt
            /**
             * Will push this and the given value to the Lua state and perform
             * the given boolean operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            bool operator>(const ParameterValue& p);            // __lt
            /**
             * Will push this and the given value to the Lua state and perform
             * the given boolean operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            bool operator<=(const ParameterValue& p);           // __le
            /**
             * Will push this and the given value to the Lua state and perform
             * the given boolean operation using Lua meta functions and
             * return the result of the operation.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p The second operand of the operation.
             *
             * @return Returns the result of the operation. If the operation is not successful it will return an object of type 0.
             */
            bool operator>=(const ParameterValue& p);           // __le
            /**
             * This will create a reference object for an item at index location idx.
             * For a lookup uses the lua_gettable function and for assignment uses the
             * lua_settable function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param idx The index to use for the look up or the assignment
             */
            ParameterReferenceInteger operator[](std::size_t idx);
            /**
             * This will create a reference object for an item at index location idx.
             * For a lookup uses the lua_gettable function and for assignment uses the
             * lua_settable function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param idx The index to use for the look up or the assignment
             */
            const ParameterReferenceInteger operator[](std::size_t idx) const;
            /**
             * This will create a reference object for an item at index location idx.
             * For a lookup uses the lua_gettable function and for assignment uses the
             * lua_settable function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param idx The index to use for the look up or the assignment
             */
            ParameterReferenceString operator[](std::string idx);
            /**
             * This will create a reference object for an item at index location idx.
             * For a lookup uses the lua_gettable function and for assignment uses the
             * lua_settable function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param idx The index to use for the look up or the assignment
             */
            const ParameterReferenceString operator[](std::string idx) const;
            /**
             * Calls the object. This uses the Lua function lua_call and therefor needs to be
             * done on a function or an object that implemented the __call meta function.
             * Can take and return any number of arguments.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param params A list of all parameters that are passed to the function call in the same order they are given in the list.
             *
             * @return Returns all results returned by the call itself in the same order they are found on the stack.
             */
            std::vector<ParameterValue> operator()(std::vector<ParameterValue>& params);
            /**
             * Convenience function. Calls the object with no parameters and 1 possible result.
             * This uses the Lua function lua_call and therefor needs to be
             * done on a function or an object that implemented the __call meta function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns the first result of the function or a ParameterValue of type 0 if no result was returned.
             */
            ParameterValue operator()();
            /**
             * Convenience function. Calls the object with 1 parameter and 1 possible result.
             * This uses the Lua function lua_call and therefor needs to be
             * done on a function or an object that implemented the __call meta function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p1 First paramter to be passed to the function call.
             *
             * @return Returns the first result of the function or a ParameterValue of type 0 if no result was returned.
             */
            ParameterValue operator()(ParameterValue p1);
            /**
             * Convenience function. Calls the object with 2 parameters and 1 possible result.
             * This uses the Lua function lua_call and therefor needs to be
             * done on a function or an object that implemented the __call meta function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p1 First paramter to be passed to the function call.
             * @param p2 Second paramter to be passed to the function call.
             *
             * @return Returns the first result of the function or a ParameterValue of type 0 if no result was returned.
             */
            ParameterValue operator()(ParameterValue p1, ParameterValue p2);
            /**
             * Convenience function. Calls the object with 3 parameters and 1 possible result.
             * This uses the Lua function lua_call and therefor needs to be
             * done on a function or an object that implemented the __call meta function.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @param p1 First paramter to be passed to the function call.
             * @param p2 Second paramter to be passed to the function call.
             * @param p3 Third paramter to be passed to the function call.
             *
             * @return Returns the first result of the function or a ParameterValue of type 0 if no result was returned.
             */
            ParameterValue operator()(ParameterValue p1, ParameterValue p2, ParameterValue p3);
            /**
             * Convenience function. Calls the object with 4 parameters and 1 possible result.
             * This uses the Lua function lua_call and therefor needs to be
             * done on a function or an object that implemented the __call meta function.
             *
             * @param p1 First paramter to be passed to the function call.
             * @param p2 Second paramter to be passed to the function call.
             * @param p3 Third paramter to be passed to the function call.
             * @param p4 Fourth paramter to be passed to the function call
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns the first result of the function or a ParameterValue of type 0 if no result was returned.
             */
            ParameterValue operator()(ParameterValue p1, ParameterValue p2, ParameterValue p3, ParameterValue p4);
            /**
             * Checks if the value can be called like a function
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns if the value object can be called.
             */
            bool isCallable() const;
            /**
             * Tries to convert the value to an interget equivalent.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns the value as an integer or 0 if the value cannot be converted.
             */
            operator int() const;
            /**
             * Tries to convert the value to a floating point equivalent.
             *
             * @note This can only be called if the object was bound to a ChimeraSystem instance.
             *
             * @return Returns the value as a double or 0 if the value cannot be converted.
             */
            operator double() const;
             /**
             * Tries to convert the value to a boolean equivalent.
             *
             * @note Follows the lua logic and all values that are not nil or false are considered true.
             * @note If the (non Lua standard) meta field __true is implemented, that value will be used instead. If the meta value is a function, it will be called and the result will be converted to a boolean value.
             *
             * @return Returns the value as a boolean.
             */
            operator bool() const;
            /**
             * Tries to convert the value to its string representation.
             *
             * @return Returns the value as a string.
             */
            operator std::string() const;
            //operator std::unordered_map<std::string, ParameterValue>() const;
            //operator std::vector<ParameterValue>() const;
            /**
             * Gets the pointer to the stored object. If the type cannot be looked up, if the
             * object has no type or if the type was deleted meanwhile it will simply return nullptr.
             *
             * @return Returns the value pointer of the object if the object is valid and returns nullptr otherwise.
             */
            explicit operator void*() const;
        protected:
            /**
             * Binds the object to an existing ChimeraSystem. Most of the functionality is only
             * available if the item is bound. Binding means that an entry in the reference list
             * for that value is created and the ChimeraSystem is added to the ParameterValue
             * so an actual type lookup and Lua operations can be perfomed.
             *
             * @note For most types it is only possible to create bound values. For convenience it is possible to create ParameterValues of basic types like number, string and boolean.
             *
             * @param chimeraSystem The executing environment
             */
            void bind(ChimeraSystem* chimeraSystem);
        private:
            /**
             * Creates a new ParameterValue with the given type and value but not bound to any ChimeraSystem.
             *
             * @param type The type of the new item.
             * @param value The pointer to the value of the given type.
             */
            ParameterValue(size_t type, void* value);
            /**
             * Creates a new ParameterValue with the given type and value and bound to the given ChimeraSystem.
             *
             * @param chimeraSystem The executing environment
             * @param type The type of the new item.
             * @param value The pointer to the value of the given type.
             */
            ParameterValue(ChimeraSystem* chimeraSystem, size_t type, void* value);
            /**
             * Executes the convenience call functions with up to 4 ParameterValue
             * parameters and 1 result.
             *
             * @param params A list of parameters to be forwarded to the actual function call.
             *
             * @return Returns the first result returned by the enclosed function.
             */
            ParameterValue call_single(std::vector<ParameterValue>& params);
            /**
             * Checks if the item is bound to any ChimeraSystem and returns it if available.
             *
             * @param data The data object that is attached to an item.
             *
             * @return Returns the pointer to the ChimeraSystem if the item was bound and nullptr otherwise.
             */
            static ChimeraSystem* getChimeraSystem(struct ParameterValueData* data);
            /**
             * Checks if the item is bound to any ChimeraSystem and returns it if available.
             *
             * @return Returns the pointer to the ChimeraSystem if the item was bound and nullptr otherwise.
             */
            ChimeraSystem* getChimeraSystem() const;

            /**
             * Performs the given unary operation on the item using the lua_arith function
             *
             * @param p1 The operand of the operation.
             * @param op A Lua constant for the operation (LUA_OPUNM or LUA_OPBNOT)
             *
             * @return Returns the Result of the operation. If the operation cannot be performed, returns an item with type 0.
             */
            static ParameterValue arith1(const ParameterValue& p1, int op);
            /**
             * Performs the given binary operation on the 2 items using the lua_arith function
             *
             * @param p1 The first operand of the operation.
             * @param p2 The second operand of the operation.
             * @param op A Lua constant for the operation.
             *
             * @return Returns the Result of the operation. If the operation cannot be performed, returns an item with type 0.
             */
            static ParameterValue arith2(const ParameterValue& p1, const ParameterValue& p2, int op);
            /**
             * Performs a comparison operation between 2 operands using the lua_compare function.
             *
             * @param p1 The first operand of the operation.
             * @param p2 The second operand of the operation.
             * @param op A Lua constant for the operation (LUA_OPEQ, LUA_OPLT or LUA_OPLE).
             */
            static bool compare(const ParameterValue& p1, const ParameterValue& p2, int op);

            /// A data object shared by all ParameterValue instances representing the same object.
            struct ParameterValueData* _data;
            /// indicates if the item was disposed already. This will remove the item from the reference counter and make this item unusable.
            bool _disposed;
    };

    /**
     * A class representing a single entry in a Lua table. Assigning the item will make
     * an assignment in the actual table and getting its value will start the actual look up.
     */
    class ParameterReferenceInteger
    {
        friend class ParameterValue;

        friend std::ostream& operator<<(std::ostream& os, const ParameterReferenceInteger& obj);

        public:
            virtual ~ParameterReferenceInteger();
            /**
             * Tries to get the value at the object index.
             *
             * @return Returns the item that is stored at index. If the lookup is not successful will return an item with type 0.
             */
            ParameterValue getValue() const;
            /**
             * This will try to set the value in the table or item. This assignment will be done
             * using the lua_settable function and needs a __newindex meta function if it is not
             * done one a table.
             *
             * @note If the object was created from an actual Lua table, it original table will not be altered by this call.
             *
             * @param other The new Value that will be assigned.
             *
             * @return Returns this object.
             */
            ParameterReferenceInteger& operator=(ParameterValue& other);
        protected:
        private:
            /**
             * Creates a new Object. This can only be created inside a operator[] call from a
             * ParameterValue object.
             *
             * @param value The table or item that should be looked up or assigned at index.
             * @param idx An index value given as an interger.
             */
            ParameterReferenceInteger(const ParameterValue& value, size_t idx);
            /// The actual object or table that should be looked up or assigned.
            ParameterValue _inner;
            /// An index value that will be used for look up or assignment.
            size_t _idx;
    };

    /**
     * A class representing a single entry in a Lua table. Assigning the item will make
     * an assignment in the actual table and getting its value will start the actual look up.
     */
    class ParameterReferenceString
    {
        friend class ParameterValue;

        friend std::ostream& operator<<(std::ostream& os, const ParameterReferenceString& obj);

        public:
            virtual ~ParameterReferenceString();
            /**
             * Tries to get the value at the object index.
             *
             * @return Returns the item that is stored at index. If the lookup is not successful will return an item with type 0.
             */
            ParameterValue getValue() const;
            /**
             * This will try to set the value in the table or item. This assignment will be done
             * using the lua_settable function and needs a __newindex meta function if it is not
             * done one a table.
             *
             * @note If the object was created from an actual Lua table, it original table will not be altered by this call.
             *
             * @param other The new Value that will be assigned.
             *
             * @return Returns this object.
             */
            ParameterReferenceString& operator=(ParameterValue& other);
        protected:
        private:
            /**
             * Creates a new Object. This can only be created inside a operator[] call from a
             * ParameterValue object.
             *
             * @param value The table or item that should be looked up or assigned at index.
             * @param idx An index value given as a string.
             */
            ParameterReferenceString(const ParameterValue& value, std::string idx);
            /// The actual object or table that should be looked up or assigned.
            ParameterValue _inner;
            /// An index value that will be used for look up or assignment.
            std::string _idx;
    };

    /**
     * Converts a ParameterValue item to its string representation and outputs it into stream.
     *
     * @param os The output stream to use.
     * @param obj The value that will be converted to a string
     *
     * @return Returns os for chaining.
     */
    std::ostream& operator<<(std::ostream& os, const chimera::ParameterValue& obj);
    /**
     * Converts an entry at index of a ParameterValue item to its string representation and outputs it into stream.
     *
     * @param os The output stream to use.
     * @param obj The value that will be converted to a string
     *
     * @return Returns os for chaining.
     */
    std::ostream& operator<<(std::ostream& os, const chimera::ParameterReferenceInteger& obj);
    /**
     * Converts an entry at index of a ParameterValue item to its string representation and outputs it into stream.
     *
     * @param os The output stream to use.
     * @param obj The value that will be converted to a string
     *
     * @return Returns os for chaining.
     */
    std::ostream& operator<<(std::ostream& os, const chimera::ParameterReferenceString& obj);
}

#endif // PARAMETERVALUE_H
