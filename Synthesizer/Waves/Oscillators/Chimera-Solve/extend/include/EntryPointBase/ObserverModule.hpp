#ifndef OBSERVERMODULE_H
#define OBSERVERMODULE_H

namespace chimera {
    namespace simulation {
        class ObserverModule:
            public chimera::Module
        {
            public:
                ObserverModule();
                virtual ~ObserverModule();
                void* getInstance(EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const final;
                virtual AbstractEventManager* getEventInstance(chimera::vec_t_LuaItem& parameters) const = 0;
                virtual size_t getAttribute(const std::string& attr) const override;
            protected:
            private:
        };
    }
}

#endif // OBSERVERMODULE_H
