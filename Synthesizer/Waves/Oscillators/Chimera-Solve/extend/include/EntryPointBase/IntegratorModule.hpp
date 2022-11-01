#ifndef INTEGRATORREGISTRY_H
#define INTEGRATORREGISTRY_H

namespace chimera {
    namespace simulation {
        class AbstractIntegrator;

        class IntegratorModule:
            public chimera::Module
        {
            public:
                IntegratorModule();
                virtual ~IntegratorModule();
                virtual bool checkFeatures(const chimera::map_t_size& features) const = 0;
                virtual const std::string getSystemName() const = 0;
                void* getInstance(EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const final;
                virtual AbstractIntegrator* getIntegratorInstance(chimera::vec_t_LuaItem& parameters) const = 0;
            protected:
            private:
        };
    }
}

#endif // INTEGRATORREGISTRY_H
