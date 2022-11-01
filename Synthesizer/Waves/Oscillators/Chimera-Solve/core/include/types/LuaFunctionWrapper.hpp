#ifndef LUAFUNCTIONWRAPPER_H
#define LUAFUNCTIONWRAPPER_H

namespace chimera {
    class ChimeraSystem;
    class ParameterTypeSystem;
    class ParameterValue;

    /**
     * Wrapper class that enables user to exchange functions between C++ und Lua context
     * without using explicit Lua function calls and without having to care about a stack.
     * This creates some overhead and sould be used wisely.
     * These objects come in 2 different usages.
     * 1. If a function was created directly in lua, it can be retrieved this way and
     *    can be called from within C++ context. These functions are called extern
     *    and a call to the intern method will return false.
     * 2. If a function was created within C++ contexts and should be passed to Lua context
     *    a new wrapper can be created with that function pointer. These functions are
     *    called intern and a call to the extern method will return true.
     *    Keep in mind that this method comes with some overhead. If you need performance
     *    and not convinience you can also pass a `lua_CFunction` directly to Lua instead of
     *    using this method. However the wrapper should be the prefered way as it will leave
     *    the stack consistent. Manually interacting with the Lua context should always be done
     *    with caution.
     */
    class LuaFunctionWrapper
    {
        friend class chimera::ChimeraContext;
        friend class chimera::ParameterTypeSystem;

        friend int chimera::luat_luafunction_push(lua_State* const L);
        friend int chimera::luat_luafunction_delete(lua_State* const L);

        public:
            /**
             * Copies an existing wrapper object. Users are not able to create new instances
             * by themselves but require system calles to do that.
             *
             * @param wrapper The origial object to copy the content from.
             */
            LuaFunctionWrapper(const LuaFunctionWrapper& wrapper);
            virtual ~LuaFunctionWrapper();
            /**
             * Returns the type of wrapper to the function. An extern function was created
             * within the Lua context and an intern function was created within C++ context.
             *
             * @return Returns true if this is an intern function and false otherwise.
             */
            bool intern() const;
            /**
             * Calls the wrapped function with the parameter list and returns its result.
             * There is no difference in calling intern or extern functions.
             *
             * @param params A list of parameters that will be passed to the wrapped function in the given order.
             *
             * @return Returns a list of result values. Note that Lua functions can return any number of result values and not just one.
             */
            vec_t_LuaItem operator()(vec_t_LuaItem& params) const;
        protected:
        private:
            /**
             * Creates a new wrapper object for an intern function within the given ChimeraSystem.
             * The pointer to this object is used a key in a hashmap that stores functions published
             * to C++ context.
             *
             * @param sys The executing ChimeraSystem as a context.
             */
            LuaFunctionWrapper(ChimeraSystem* sys);
            /**
             * Creates a new wrapper object for an extern function within the given ChimeraSystem.
             *
             * @param sys The executing ChimeraSystem as a context.
             * @param fn The function pointer
             */
            LuaFunctionWrapper(ChimeraSystem* sys, fn_luafnwrapper fn);

            /// The Executing ChimeraSystem that created this instance.
            ChimeraSystem* _chimeraSystem;
            /// if an intern (C++) function, the function pointer, if nullptr this is an extern function
            fn_luafnwrapper _fn;
            /// The original object. This is only important to intern functions as Lua saves a reference to the function mapped to this pointer.
            void* _origin;
    };
}

#endif // LUAFUNCTIONWRAPPER_H
