#ifndef OBSERVERENTRYPOINT_H
#define OBSERVERENTRYPOINT_H

namespace chimera {
    namespace runtime {
        class ObserverEntryPoint:
            public chimera::EntryPoint
        {
            public:
                ObserverEntryPoint();
                virtual ~ObserverEntryPoint();
                virtual const std::string getGUID() const override;
            protected:
                virtual bool checkModule(chimera::Module const * const module) const override;
            private:
        };
    }
}

#endif // OBSERVERENTRYPOINT_H
