#ifndef ABSTRACTINITIALIZER_H
#define ABSTRACTINITIALIZER_H

namespace chimera {
    namespace simulation {
        class AbstractInitializer
        {
            public:
                AbstractInitializer();
                virtual ~AbstractInitializer();
                virtual void initialize(AbstractSystemDynamic* system, void* state) = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTINITIALIZER_H
