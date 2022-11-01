#ifndef EXTENDTYPES_H
#define EXTENDTYPES_H

namespace chimera {
    namespace simulation {

        struct T_MatrixDef {
            const long unsigned int rows;
            const long unsigned int cols;
            const bool readonly;
            const bool deleteValue;
            void* const value;
        };

        struct T_MatrixRowDef {
            const long unsigned int row;
            const long unsigned int length;
            const bool readonly;
            void* const value;
        };

        struct T_VectorDef {
            const long unsigned int length;
            const bool readonly;
            const bool deleteValue;
            void* value;
        };
    }
}

#endif // EXTENDTYPES_H
