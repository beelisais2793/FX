#ifndef SYSTEMDYNAMICENTRYPOINT_H
#define SYSTEMDYNAMICENTRYPOINT_H

namespace chimera {
    namespace runtime {

        class SystemDynamicEntryPoint:
            public chimera::EntryPoint
        {
            public:
                SystemDynamicEntryPoint();
                virtual ~SystemDynamicEntryPoint();
                virtual const std::string getGUID() const override;
            protected:
                virtual bool checkModule(chimera::Module const * const module) const override;
                virtual void init() override;
            private:
        };
    }
}

#endif // SYSTEMDYNAMICENTRYPOINT_H
