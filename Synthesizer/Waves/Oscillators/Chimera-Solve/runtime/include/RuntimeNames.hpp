#ifndef RUNTIMENAMES_HPP_INCLUDED
#define RUNTIMENAMES_HPP_INCLUDED

namespace chimera {
    namespace runtime {
        namespace Naming
        {
            const char * const Path_modules = "modules";

            const char * const Module_prefix = "";
            const char * const Module_extension = ".mod";
            const char * const Module_createFunction = "LoadModule";
            const char * const Module_deleteFunction = "UnloadModule";

            const char * const Logging_log = "";
            const char * const Logging_warning = "[WARNING] ";
            const char * const Logging_error = "[ERROR] ";

//            const char * const Lua_reg_simulation = "simulation:com:simulations";
            const char * const Lua_reg_argv = "simulation:argv";
//            const char * const Lua_reg_systemNames = "simulation:system_names";

            const char * const Lua_global_log = "log";
            const char * const Lua_global_print = "print";
            const char * const Lua_global_write = "write";
            const char * const Lua_global_writeline = "writeln";
            const char * const Lua_global_error = "error";
            const char * const Lua_global_warning = "warning";
            const char * const Lua_global_exit = "exit";
            const char * const Lua_global_simulation = "simulation";
            const char * const Lua_global_argv = "argv";
            const char * const Lua_global_complex = "complex";
            const char * const Lua_global_random = "random";
            const char * const Lua_global_math = "math";
            const char * const Lua_global_matrix = "matrix";
            const char * const Lua_global_vector = "vector";
        }
    }
}
#endif // RUNTIMENAMES_HPP_INCLUDED
