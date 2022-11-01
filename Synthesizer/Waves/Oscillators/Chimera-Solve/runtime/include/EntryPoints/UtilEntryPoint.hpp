#ifndef UTILENTRYPOINT_H
#define UTILENTRYPOINT_H

namespace chimera {
    namespace runtime {

        class UtilEntryPoint:
            public chimera::EntryPoint
        {
            public:
                UtilEntryPoint();
                virtual ~UtilEntryPoint();
                virtual const std::string getGUID() const override;
        };

    }
}

#endif // UTILENTRYPOINT_H
