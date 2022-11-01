#ifndef EXTENSIONNAMES_HPP_INCLUDED
#define EXTENSIONNAMES_HPP_INCLUDED

namespace chimera {
    namespace simulation {
        namespace Naming
        {
            const char * const EntryPoint_initilizer = "init";
            const char * const EntryPoint_dynamics = "dynamics";
            const char * const EntryPoint_integrator = "integrator";
            const char * const EntryPoint_output = "output";
            const char * const EntryPoint_random = "random";
            const char * const EntryPoint_observer = "observer";
            const char * const EntryPoint_util = "util";

            const char * const Feature_state_type = "state_type";
            const char * const Feature_time_type = "time_type";
            const char * const Feature_size = "size";
            const char * const Feature_unit_size = "unit_size";

            const char * const Type_Simulation = "Simulation";
            const char * const Type_Complex = "Complex";
            const char * const Type_Vector = "Vector";
            const char * const Type_Matrix = "Matrix";
            const char * const Type_MatrixRow = "MatrixRow";

            const char * const Attribute_Observable = "observable";
            const char * const Attribute_Observer = "observe-single";
            const char * const Attribute_MultiObserver = "observe-multi";
        }
    }
}
#endif // EXTENSIONNAMES_HPP_INCLUDED
