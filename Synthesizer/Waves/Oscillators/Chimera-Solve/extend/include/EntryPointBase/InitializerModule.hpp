#ifndef INITIALIZERMODULE_H
#define INITIALIZERMODULE_H

namespace chimera {
    namespace simulation {
        class InitializerModule:
            public chimera::Module
        {
            public:
                InitializerModule();
                virtual ~InitializerModule();
                void* getInstance(EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const final;
                virtual AbstractInitializer* getInitializerInstance(chimera::vec_t_LuaItem& parameters) const = 0;
            protected:
            private:
        };
    }
}

#endif // INITIALIZERMODULE_H
