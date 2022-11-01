#ifndef RANDOMENTRYPOINT_H
#define RANDOMENTRYPOINT_H

namespace chimera {
    namespace runtime {

        class RandomEntryPoint:
            public chimera::EntryPoint
        {
            public:
                RandomEntryPoint();
                virtual ~RandomEntryPoint();
                virtual const std::string getGUID() const override;
            protected:
                virtual bool checkModule(chimera::Module const * const module) const override;
            private:
        };
    }
}

#endif // RANDOMENTRYPOINT_H
