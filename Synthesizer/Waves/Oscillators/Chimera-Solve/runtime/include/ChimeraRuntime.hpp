#ifndef CHIMERARUNTIME_H
#define CHIMERARUNTIME_H

namespace chimera {
    namespace runtime {

        class InitializerEntryPoint;
        class IntegratorEntryPoint;
        class ObserverEntryPoint;
        class OutputEntryPoint;
        class RandomEntryPoint;
        class SystemDynamicEntryPoint;
        class UtilEntryPoint;

        class ModuleLoader;

        class ChimeraRuntime:
            public chimera::ChimeraSystem
        {
            friend int lua_TemporalSimulation_start(lua_State*);
            friend int chimera::runtime::lua_TemporalSimulation_run(lua_State*);
            friend int lua_TemporalSimulation_step(lua_State*);
            friend int lua_TemporalSimulation_current(lua_State*);

            public:
                ChimeraRuntime(const std::string& filename, std::vector<char*>& argv, ModuleLoader* loader);
                virtual ~ChimeraRuntime();
                chimera::Module* openModule(chimera::EntryPoint const * const entrypoint, const std::string& name) override;
                void closeModule(chimera::Module* module) override;
                void notifyDelete(chimera::StateSynchrony* sender) override;
                TypeLookup const * const getTypeLookup() const;
            protected:

            private:
                size_t _pid_complex;
                size_t _pid_vector;
                size_t _pid_matrix;
                size_t _pid_matrixrow;

                chimera::runtime::InitializerEntryPoint* _initializer;
                chimera::runtime::IntegratorEntryPoint* _integrator;
                chimera::runtime::ObserverEntryPoint* _observer;
                chimera::runtime::OutputEntryPoint* _output;
                chimera::runtime::RandomEntryPoint* _random;
                chimera::runtime::SystemDynamicEntryPoint* _systemDynamic;
                chimera::runtime::UtilEntryPoint* _util;

                std::string* _filename;
                ModuleLoader* _loader;
                TypeLookup* _lookup;
                size_t registerVector(const size_t& subtype, const struct T_ParameterDef& pdef);
                size_t registerMatrix(const size_t& subtype, const struct T_ParameterDef& pdef);

        };
    }
}

#endif // CHIMERARUNTIME_H
