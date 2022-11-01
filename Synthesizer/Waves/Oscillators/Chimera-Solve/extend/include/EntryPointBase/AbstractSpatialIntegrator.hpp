#ifndef ABSTRACTSPATIALINTEGRATOR_H
#define ABSTRACTSPATIALINTEGRATOR_H

namespace chimera {
    namespace simulation {
        class AbstractSpatialIntegrator:
            public AbstractIntegrator
        {
            public:
                AbstractSpatialIntegrator();
                virtual ~AbstractSpatialIntegrator();
            protected:
            private:
        };
    }
}

#endif // ABSTRACTSPATIALINTEGRATOR_H
