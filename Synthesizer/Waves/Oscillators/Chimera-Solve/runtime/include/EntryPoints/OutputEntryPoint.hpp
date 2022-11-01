#ifndef OUTPUTENTRYPOINT_H
#define OUTPUTENTRYPOINT_H

namespace chimera {
    namespace runtime {

        class OutputEntryPoint:
            public EntryPoint
        {
            public:
                OutputEntryPoint();
                virtual ~OutputEntryPoint();
                virtual const std::string getGUID() const override;
            protected:
                virtual bool checkModule(chimera::Module const * const module) const override;
                virtual void init() override;
            private:

        };
    }
}
#endif // OUTPUTENTRYPOINT_H
