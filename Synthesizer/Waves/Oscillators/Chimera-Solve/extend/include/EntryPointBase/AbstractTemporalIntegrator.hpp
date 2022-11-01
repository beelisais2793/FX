#ifndef ABSTRACTTEMPORALINTEGRATOR_H
#define ABSTRACTTEMPORALINTEGRATOR_H

namespace chimera {
    namespace simulation {
        class AbstractTemporalIntegrator:
            public AbstractIntegrator
        {
            public:
                AbstractTemporalIntegrator();
                virtual ~AbstractTemporalIntegrator();

                virtual size_t getTimeType() const = 0;
                virtual size_t getStateType() const = 0;
                virtual void * currentTime() = 0;
                virtual void * currentState() = 0;
                virtual void nextStep() = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTTEMPORALINTEGRATOR_H
