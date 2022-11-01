#ifndef MODULELOADER_H
#define MODULELOADER_H

//enum EntryPoint { Dynamic, Coupling, Noise, Initialsation, Integrator, Output };


namespace chimera {
    class Module;

    namespace runtime {
        class IModulePathProvider;

        typedef std::vector<std::pair<std::string, IModulePathProvider*> > vec_t_pathList;

        typedef chimera::Module* (*fn_create_mod) ();
        typedef void (*fn_delete_mod) (chimera::Module*);

        struct T_ModuleSymbols {
            void* lib;
            const fn_create_mod load;
            const fn_delete_mod unload;
        };

        typedef std::unordered_map<std::string, chimera::Module*> map_t_ModuleRegistry;
        typedef std::unordered_map<std::string, struct T_ModuleSymbols> map_t_ModuleItem;
        //typedef std::unordered_map<EntryPoint, map_t_ModuleEntry*, std::hash<int> > map_t_ModuleCategory;

        //typedef std::unordered_map<std::string,system_factory_ptr> map_t_SystemDynamics;

        class ModuleLoader
        {
            public:
                ModuleLoader(const vec_t_pathList& importLookup);
                virtual ~ModuleLoader();
                chimera::Module* const load(const std::string& name);
                void unload(const std::string& name);
                void unload(const chimera::Module* module);
                void unloadAll();
            protected:
            private:
                vec_t_pathList* _importLookup;
                map_t_ModuleItem* _modulePtr;
                map_t_ModuleRegistry* _moduleReg;
        };
    }
}

#endif // MODULELOADER_H
