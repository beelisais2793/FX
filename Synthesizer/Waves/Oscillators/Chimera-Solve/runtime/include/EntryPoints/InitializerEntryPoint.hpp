#ifndef INITIALIZERENTRYPOINT_H
#define INITIALIZERENTRYPOINT_H

namespace chimera {
    namespace runtime {

        class InitializerEntryPoint:
            public chimera::EntryPoint
        {
            public:
                InitializerEntryPoint();
                virtual ~InitializerEntryPoint();
                virtual const std::string getGUID() const override;
            protected:
                virtual bool checkModule(chimera::Module const * const module) const override;
            private:
        };
    }
}

#endif // INITIALIZERENTRYPOINT_H
